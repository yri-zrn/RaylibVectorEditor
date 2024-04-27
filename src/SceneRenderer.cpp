#include "SceneRenderer.hpp"

#include "Entity.hpp"
#include "ViewportWindow.hpp"
#include "ViewportCamera.hpp"

namespace cw {

void SceneRenderer::OnSetup() {
    m_View        = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_RawRender   = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_FXAA_Shader = LoadShader(nullptr, "../assets/shaders/FXAA.fs");
}

void SceneRenderer::OnUpdate() {
    if (m_Context->Resized())
        HandleViewportResize(m_Context->GetSize());
}

void SceneRenderer::OnShutdown() {
    UnloadRenderTexture(m_View);
    UnloadRenderTexture(m_RawRender);
}

void SceneRenderer::OnSceneLoad(Scene* scene) {
    m_SceneContext = scene;
}

void SceneRenderer::OnSceneUnload() {

}

void SceneRenderer::HandleViewportResize(const Vector2& size) {
    UnloadRenderTexture(m_View);
    m_View = LoadRenderTexture((int)size.x, (int)size.y);
    m_RawRender  = LoadRenderTexture((int)size.x, (int)size.y);
}

void SceneRenderer::Render(Scene& scene, const ViewportCamera& camera) {
    const auto entity_stack = scene.GetEntityStack();
    auto raylib_camera = (Camera2D)camera;

    BeginTextureMode(m_RawRender);
        BeginMode2D((Camera2D)camera);
            ClearBackground(Color{40, 40, 40, 255});

            // Vector2 viewport_size = ;

            Vector2 viewport_p0 = (GetScreenToWorld2D({0.0f, 0.0f}, camera));
            Vector2 viewport_p1 = (GetScreenToWorld2D(m_Context->GetSize(), camera));
            Vector2 viewport_size = Vector2Subtract(viewport_p1, viewport_p0);

            Color grid_color = Color{200, 200, 200, 40};
            float grid_size = 100.0f;

            // DrawRectanglePro({viewport_p0.x+50, viewport_p0.y+50, viewport_size.x-100, viewport_size.y-100}, {0.0f, 0.0f}, 0.0f, {50, 200, 100, 200});
            // DrawRectanglePro({viewport_p0.x+10/raylib_camera.zoom, viewport_p0.y+10/raylib_camera.zoom, viewport_size.x-20/raylib_camera.zoom, viewport_size.y-20/raylib_camera.zoom}, {0.0f, 0.0f}, 0.0f, grid_color);
            
            for (float x = -4000; x <= 4000; x += grid_size)
                DrawLine(0+x, -4000, 0+x, 4000, grid_color );
            for (float y = -4000; y <= 4000; y += grid_size)
                DrawLine(-4000, 0+y, 4000, 0+y, grid_color );

            // for (float x = fmodf(viewport_p0.x, grid_size); x < viewport_p1.x-viewport_p0.x; x += grid_size)
            //     DrawLine(viewport_p0.x + x, viewport_p0.y, viewport_p0.x + x, viewport_p1.y, grid_color);
            // for (float y = fmodf(viewport_p0.y, grid_size); y < viewport_p1.y-viewport_p0.y; y += grid_size)
            //     DrawLine(viewport_p0.x, viewport_p0.y + y, viewport_p1.x, viewport_p0.y + y, grid_color);

            // for (float x = fmodf(viewport_p0.x, grid_size); x < viewport_p1.x - viewport_p0.x; x += grid_size)
            //     DrawLine(viewport_p0.x + x, viewport_p0.y, viewport_p0.x + x, viewport_p1.y, grid_color);
            // for (float y = fmodf(viewport_p0.y, grid_size); y < viewport_p1.y - viewport_p0.y; y += grid_size)
            //     DrawLine(viewport_p0.x, viewport_p0.y + y, viewport_p1.x, viewport_p0.y + y, grid_color);

            // DrawRectangle(0, 0, 105, 105, GRAY);
            DrawLineV({0, 0}, {100, 0}, RED  );
            DrawLineV({0, 0}, {0, 100}, GREEN);

            for (auto iter = entity_stack.begin(); iter != entity_stack.end(); ++iter) {
                auto& entity = *iter;
                Draw(entity);
            }

        EndMode2D();
    EndTextureMode();

    Vector2 resolution = Vector2{(float)m_RawRender.texture.width, (float)m_RawRender.texture.height};
    SetShaderValue(m_FXAA_Shader, GetShaderLocation(m_FXAA_Shader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);

    BeginTextureMode(m_View);
        BeginShaderMode(m_FXAA_Shader);
            DrawTextureRec(m_RawRender.texture, (Rectangle){ 0, 0, (float)m_RawRender.texture.width, (float)-m_RawRender.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();
    EndTextureMode();
}

void SceneRenderer::Draw(Entity entity) {
    Color color = entity.GetComponent<ColorComponent>().Color;

    if (entity.HasComponent<LineComponent>()) {
        auto& lc = entity.GetComponent<LineComponent>();
        DrawLineV(lc.Begin, lc.End, color);
        DrawLineEx(lc.Begin, lc.End, lc.Thickness, color);
    }
    else if (entity.HasComponent<RectangleComponent>()) {
        auto& rc = entity.GetComponent<RectangleComponent>();
        DrawRectangle(rc.Begin.x, rc.Begin.y, rc.End.x - rc.Begin.x, rc.End.y - rc.Begin.y, color);
    }
    else if (entity.HasComponent<EllipseComponent>()) {
        auto& ec = entity.GetComponent<EllipseComponent>();
        DrawEllipse((int)ec.Center.x, (int)ec.Center.y, ec.RadiusX, ec.RadiusY, color);
    }
}

}