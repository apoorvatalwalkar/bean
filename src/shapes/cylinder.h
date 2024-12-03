#ifndef CYLINDER_H
#define CYLINDER_H

#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include <optional>
#include "raytracer/raytracer.h"

class Cylinder {
public:
    Cylinder(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    std::optional<Intersection> checkIntersection();
private:
    RenderShapeData cylinderShape;
    glm::vec4 p;
    glm::vec4 d;
};

#endif // CYLINDER_H
