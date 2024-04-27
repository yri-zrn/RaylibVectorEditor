#pragma once

#include "Core.hpp"

#include <string>

namespace cw {

class Scene;

enum class SerializationType {
    JSON,
    Binary
};

class SceneSerializer {
public:
    static void Serialize(const std::string& filepath, Scene& scene);
    static void Deserialize(const std::string& filepath, Scene& scene);

private:

};

}

