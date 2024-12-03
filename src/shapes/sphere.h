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
    std::optional<Intersection> checkIntersection();
private:
    RenderShapeData sphereShape;
    glm::vec4 p;
    glm::vec4 d;

    SceneGlobalData globalData;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
    glm::mat4 directionToCamera;
};

#endif // SPHERE_H
