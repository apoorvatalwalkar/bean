#include "mesh_volume.h"
#include "utils/sceneparser.h"

MeshVolume::MeshVolume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

bool MeshVolume::checkIntersection(){
    float t1 = 0;

    float xmax = 0.55, xmin = -0.55;
    float ymax = 0.39, ymin = -0.39;
    float zmax = 0.24, zmin = -0.27;

    //+ xy plane intersection
    t1 = (zmax - p[2]) / d[2];
    if(t1 >= 0 && xmin <= p[0] + t1 * d[0] && xmax >= p[0] + t1 * d[0] && ymin <= p[1] + t1 * d[1] && ymax >= p[1] + t1 * d[1]){
        return true;
    }

    // - xy plane intersection
    t1 = (zmin - p[2]) / d[2];
    if(t1 >= 0 && xmin <= p[0] + t1 * d[0] && xmax >= p[0] + t1 * d[0] && ymin <= p[1] + t1 * d[1] && ymax >= p[1] + t1 * d[1]){
        return true;
    }

    // + xz plane intersection
    t1 = (ymax - p[1]) / d[1];
    if(t1 >= 0 && xmin <= p[0] + t1 * d[0] && xmax >= p[0] + t1 * d[0] && zmin <= p[2] + t1 * d[2] && zmax >= p[2] + t1 * d[2]){
        return true;
    }

    // - xz plane intersection
    t1 = (ymin - p[1]) / d[1];
    if(t1 >= 0 && xmin <= p[0] + t1 * d[0] && xmax >= p[0] + t1 * d[0] && zmin <= p[2] + t1 * d[2] && zmax >= p[2] + t1 * d[2]){
        return true;
    }

    // + yz plane intersection
    t1 = (xmax - p[0]) / d[0];
    if(t1 >= 0 && ymin <= p[1] + t1 * d[1] && ymax >= p[1] + t1 * d[1] && zmin <= p[2] + t1 * d[2] && zmax >= p[2] + t1 * d[2]){
        return true;
    }

    // - yz plane intersection
    t1 = (xmin - p[0]) / d[0];
    if(t1 >= 0 && ymin <= p[1] + t1 * d[1] && ymax >= p[1] + t1 * d[1] && zmin <= p[2] + t1 * d[2] && zmax >= p[2] + t1 * d[2]){
        return true;
    }

    return false;
}
