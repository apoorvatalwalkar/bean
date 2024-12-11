#ifndef SPHERE_H
#define SPHERE_H

#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include <optional>
#include "raytracer/raytracer.h"

class Sphere {
public:
    Sphere(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    std::optional<Intersection> checkIntersection(float time);
private:
    RenderShapeData sphereShape;
    glm::vec4 p;
    glm::vec4 d;

    SceneGlobalData globalData;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
    glm::mat4 directionToCamera;

    // for defining start and end of sphere movement
    glm::vec4 centerStart = glm::vec4(0.f, 0.f, 0.f, 1.f);
    glm::vec4 centerEnd = glm::vec4 (0.f, 0.f, 0.5f, 1.f);
};

#endif // SPHERE_H
