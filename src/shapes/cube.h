#ifndef CUBE_H
#define CUBE_H

#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include <optional>
#include "raytracer/raytracer.h"


class Cube {
public:
    Cube(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    std::optional<Intersection> checkIntersection();
private:
    RenderShapeData cubeShape;
    glm::vec4 p = glm::vec4(0.f, 0.f, 0.f, 0.f);
    glm::vec4 d  = glm::vec4(0.f, 0.f, 0.f, 0.f);
};

#endif // CUBE_H
