#pragma once

#include <raylib.h>
#include <string>

namespace cw {

struct TagComponent {
    std::string Tag;
};

struct SelectedComponent {

};

struct ColorComponent {
    Color Color;
};

struct LineComponent {
    Vector2 Begin;
    Vector2 End;
    float   Thickness;
};

struct RectangleComponent {
    Vector2 Begin;
    Vector2 End;
};

struct CircleComponent {
    Vector2 Center;
    float Radius;
};

struct EllipseComponent {
    Vector2 Center;
    float RadiusX;
    float RadiusY;
};

}