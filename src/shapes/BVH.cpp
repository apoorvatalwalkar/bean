#include "utils/rgba.h"
#include "bvh.h"
#include "utils/sceneparser.h"
#include "utils/phongillumination.h"
#include "raytracer/raytracescene.h"

Volume::Volume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    volume = shape;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

bool Volume::checkIntersection(){
    float t1 = 0;

    //+ xy plane intersection
    t1 = (0.5 - p[2]) / d[2];
    if(t1 >= 0 && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1]){
        return true;
    }

    // - xy plane intersection
    t1 = (-0.5 - p[2]) / d[2];
    if(t1 >= 0 && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1]){
        return true;
    }

    // + xz plane intersection
    t1 = (0.5 - p[1]) / d[1];
    if(t1 >= 0 && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        return true;
    }

    // - xz plane intersection
    t1 = (-0.5 - p[1]) / d[1];
    if(t1 >= 0 && -0.5 <= p[0] + t1 * d[0] && 0.5 >= p[0] + t1 * d[0] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        return true;
    }

    // + yz plane intersection
    t1 = (0.5 - p[0]) / d[0];
    if(t1 >= 0 && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        return true;
    }

    // - yz plane intersection
    t1 = (-0.5 - p[0]) / d[0];
    if(t1 >= 0 && -0.5 <= p[1] + t1 * d[1] && 0.5 >= p[1] + t1 * d[1] && -0.5 <= p[2] + t1 * d[2] && 0.5 >= p[2] + t1 * d[2]){
        return true;
    }

    return false;
}