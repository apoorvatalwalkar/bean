#include "bvh.h"
#include "utils/sceneparser.h"

Volume::Volume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

bool Volume::checkIntersection(){
    float t1 = 0;

    //+ xy plane intersection
    t1 = (1.0 - p[2]) / d[2];
    if(t1 >= 0 && -1.0 <= p[0] + t1 * d[0] && 1.0 >= p[0] + t1 * d[0] && -1.0 <= p[1] + t1 * d[1] && 1.0 >= p[1] + t1 * d[1]){
        return true;
    }

    // - xy plane intersection
    t1 = (-1.0 - p[2]) / d[2];
    if(t1 >= 0 && -1.0 <= p[0] + t1 * d[0] && 1.0 >= p[0] + t1 * d[0] && -1.0 <= p[1] + t1 * d[1] && 1.0 >= p[1] + t1 * d[1]){
        return true;
    }

    // + xz plane intersection
    t1 = (1.0 - p[1]) / d[1];
    if(t1 >= 0 && -1.0 <= p[0] + t1 * d[0] && 1.0 >= p[0] + t1 * d[0] && -1.0 <= p[2] + t1 * d[2] && 1.0 >= p[2] + t1 * d[2]){
        return true;
    }

    // - xz plane intersection
    t1 = (-1.0 - p[1]) / d[1];
    if(t1 >= 0 && -1.0 <= p[0] + t1 * d[0] && 1.0 >= p[0] + t1 * d[0] && -1.0 <= p[2] + t1 * d[2] && 1.0 >= p[2] + t1 * d[2]){
        return true;
    }

    // + yz plane intersection
    t1 = (1.0 - p[0]) / d[0];
    if(t1 >= 0 && -1.0 <= p[1] + t1 * d[1] && 1.0 >= p[1] + t1 * d[1] && -1.0 <= p[2] + t1 * d[2] && 1.0 >= p[2] + t1 * d[2]){
        return true;
    }

    // - yz plane intersection
    t1 = (-1.0 - p[0]) / d[0];
    if(t1 >= 0 && -1.0 <= p[1] + t1 * d[1] && 1.0 >= p[1] + t1 * d[1] && -1.0 <= p[2] + t1 * d[2] && 1.0 >= p[2] + t1 * d[2]){
        return true;
    }

    return false;
}
