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
    std::optional<Intersection> checkIntersection();
private:
    RenderShapeData coneShape;
    glm::vec4 p;
    glm::vec4 d;
};

#endif // CONE_H
