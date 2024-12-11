#ifndef CONE_H
#define CONE_H

#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include "raytracer/raytracer.h"
#include <optional>


class Cone {
public:
    Cone(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    std::optional<Intersection> checkIntersection(float time);
private:
    RenderShapeData coneShape;
    glm::vec4 p;
    glm::vec4 d;

    // for defining start and end of cone movement
    glm::vec4 centerStart = glm::vec4(0.2f, 0.f, 0.f, 1.f);
    glm::vec4 centerEnd = glm::vec4 (0.f, 0.f, 0.5f, 1.f);
};

#endif // CONE_H
