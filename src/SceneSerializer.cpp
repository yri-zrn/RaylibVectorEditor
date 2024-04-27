#include "SceneSerializer.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "ViewportWindow.hpp"
#include "ViewportCamera.hpp"
#include "Components.hpp"

// #include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp> 
#include <cereal/types/string.hpp> 
#include <entt/entity/snapshot.hpp>
#include <fstream>
#include <raylib.h>

template<typename Archive>
static void serialize(Archive &archive, Color& color) {
    archive( CEREAL_NVP(color.r), CEREAL_NVP(color.g), CEREAL_NVP(color.b), CEREAL_NVP(color.a) );
}

template<typename Archive>
static void serialize(Archive &archive, Vector2& vector) {
    archive( CEREAL_NVP(vector.x), CEREAL_NVP(vector.y) );
}

template<typename Archive>
void serialize(Archive &archive, Camera2D& camera) {
    archive( CEREAL_NVP(camera.offset), CEREAL_NVP(camera.rotation), CEREAL_NVP(camera.target), CEREAL_NVP(camera.zoom) );
}

namespace cw {

template<typename Archive>
void serialize(Archive &archive, TagComponent& tag) {
    archive( CEREAL_NVP(tag.Tag) );
}

template<typename Archive>
static void serialize(Archive &archive, ColorComponent& color) {
    archive( CEREAL_NVP(color.Color) );
}

template<typename Archive>
void serialize(Archive &archive, LineComponent& line) {
    archive( CEREAL_NVP(line.Begin), CEREAL_NVP(line.End), CEREAL_NVP(line.Thickness) );
}

template<typename Archive>
void serialize(Archive &archive, RectangleComponent& rectangle) {
    archive( CEREAL_NVP(rectangle.Begin), CEREAL_NVP(rectangle.End) );
}

template<typename Archive>
void serialize(Archive &archive, EllipseComponent& ellipse) {
    archive( CEREAL_NVP(ellipse.Center), CEREAL_NVP(ellipse.RadiusX), CEREAL_NVP(ellipse.RadiusY) );
}

template<typename Archive>
void serialize(Archive &archive, ViewportCamera& camera) {
    archive( CEREAL_NVP(camera.m_Camera) );
}

void SceneSerializer::Serialize(const std::string& filepath, Scene& scene) {
    std::ofstream output_stream{ filepath, std::ios::out };
    cereal::BinaryOutputArchive oarchive(output_stream);
    
    entt::snapshot{scene.m_Registry}
        .get<entt::entity>(oarchive)
        .get<TagComponent>(oarchive)
        .get<ColorComponent>(oarchive)
        .get<LineComponent>(oarchive)
        .get<RectangleComponent>(oarchive)
        .get<EllipseComponent>(oarchive);
    
    oarchive(scene.m_EntityStack);

    // oarchive(scene.m_ViewportContext->m_Camera);
}

void SceneSerializer::Deserialize(const std::string& filepath, Scene& scene) {
    std::ifstream input_stream{ filepath, std::ios::in };
    cereal::BinaryInputArchive iarchive(input_stream);
    
    entt::snapshot_loader{scene.m_Registry}
        .get<entt::entity>(iarchive)
        .get<TagComponent>(iarchive)
        .get<ColorComponent>(iarchive)
        .get<LineComponent>(iarchive)
        .get<RectangleComponent>(iarchive)
        .get<EllipseComponent>(iarchive);
    
    iarchive(scene.m_EntityStack);

    // iarchive(scene.m_ViewportContext->m_Camera);
}

}