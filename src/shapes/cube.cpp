#include "utils/rgba.h"
#include "cube.h"
#include "utils/sceneparser.h"
#include "utils/phongillumination.h"
#include "raytracer/raytracescene.h"

Cube::Cube(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    cubeShape = shape;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

std::optional<Intersection> Cube::checkIntersection(){

    Intersection intr = {.t = float(INT_MAX), .u = float(INT_MAX), .v = float(INT_MAX)};
    float t1;

    //+ xy plane intersection
    t1 = (0.5 - p[2]) / d[2];
    if(-0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1]){
        intr.t = t1;
        intr.normal = {0.0, 0.0, 1.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 + (p[1] + t1 * d[1]);
    }

    // - xy plane intersection
    t1 = (-0.5 - p[2]) / d[2];
    if(t1 < intr.t && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1]){
        intr.t = t1;
        intr.normal = {0.0, 0.0, -1.0};
        intr.u = 0.5 - (p[0] + t1 * d[0]);
        intr.v = 0.5 + (p[1] + t1 * d[1]);
    }

    // + xz plane intersection
    t1 = (0.5 - p[1]) / d[1];
    if(t1 < intr.t && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        intr.t = t1;
        intr.normal = {0.0, 1.0, 0.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 - (p[2] + t1 * d[2]);
    }

    // - xz plane intersection
    t1 = (-0.5 - p[1]) / d[1];
    if(t1 < intr.t && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        intr.t = t1;
        intr.normal = {0.0, -1.0, 0.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 + (p[2] + t1 * d[2]);
    }

    // + yz plane intersection
    t1 = (0.5 - p[0]) / d[0];
    if(t1 < intr.t && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        intr.t = t1;
        intr.normal = {1.0, 0.0, 0.0};
        intr.u = 0.5 - (p[2] + t1 * d[2]);
        intr.v = 0.5 + (p[1] + t1 * d[1]);
    }

    // - yz plane intersection
    t1 = (-0.5 - p[0]) / d[0];
    if(t1 < intr.t && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        intr.t = t1;
        intr.normal = {-1.0, 0.0, 0.0};
        intr.u = 0.5 + (p[2] + t1 * d[2]);
        intr.v = 0.5 + (p[1] + t1 * d[1]);
    }

    if(intr.t < float(INT_MAX)){
        glm::mat4 ctm = cubeShape.ctm;
        glm::mat3 m33;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                m33[i][j] = ctm[i][j];
            }
        }

        intr.normal = glm::normalize(glm::transpose(glm::inverse(m33)) * intr.normal);
        return intr;
    }

    return std::nullopt;
}
