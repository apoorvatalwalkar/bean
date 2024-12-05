#pragma once

#include <glm/glm.hpp>
#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include <optional>

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        bool enableTextureFilter = false;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
        int maxRecursiveDepth    = 4;
        bool onlyRenderNormals   = false;
    };

public:
    RayTracer(Config config);

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(RGBA *imageData, const RayTraceScene &scene);

private:
    const Config m_config;
    SceneGlobalData globalData;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;

    RGBA parseShapes();
    RGBA cube();
    RGBA sphere();
    RGBA cylinder();
    RGBA cone();
};

struct Intersection{
    float t = float(INT_MAX);
    glm::vec3 normal = glm::vec3(0.0);
    SceneMaterial material = SceneMaterial();
    float u = float(INT_MAX);
    float v = float(INT_MAX);
    std::optional<Image> map = std::nullopt;
};

std::optional<Intersection> checkIntersection(glm::vec4 p, glm::vec4 d, std::vector<RenderShapeData> shapes);

