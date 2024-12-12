#pragma once

#include <glm/glm.hpp>
#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include <optional>
#include "raytracescene.h"

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
        int maxRecursiveDepth    = 2;
        bool onlyRenderNormals   = false;
        bool cameraMovement      = false;
        bool enableSoftShadows   = false;
        bool enableOcclusion     = false;
    };

public:
    RayTracer(Config config, QImage image, QString imagePath);

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(/*RGBA *imageData, */ const RayTraceScene &scene);

private:
    const Config m_config;
    QImage m_image;
    QString m_imagePath;
    SceneGlobalData globalData;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;

    RGBA parseShapes();
    RGBA cube();
    RGBA sphere();
    RGBA cylinder();
    RGBA cone();
    float calcOcclusion(glm::vec4 surfacePos, glm::vec3 surfaceNormal, std::vector<RenderShapeData> shapes, float time);
    glm::vec3 hemisphereSample(glm::vec3 surfaceNormal);
    void renderOneScene(RGBA *imageData, const RayTraceScene &scene);
    glm::mat4 myRotate(float angleDegrees, glm::vec3 axis);
    RGBA toRGBA(const glm::vec4 &illumination);

    int maxSamples = 10;
    float sampleThreshold = 1.1f;
};

struct Intersection {
    float t = float(INT_MAX);
    glm::vec3 normal = glm::vec3(0.0);
    SceneMaterial material = SceneMaterial();
    float u = float(INT_MAX);
    float v = float(INT_MAX);
    std::optional<Image> map = std::nullopt;
};

std::optional<Intersection> checkIntersection(glm::vec4 p, glm::vec4 d, std::vector<RenderShapeData> shapes, float time);

