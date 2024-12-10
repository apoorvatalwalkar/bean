#include "mesh_volume.h"
#include "utils/sceneparser.h"

MeshVolume::MeshVolume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

bool MeshVolume::checkIntersection(){
    float t1 = 0;

    float xmax = 3.1, xmin = -3.8;
    float ymax = 5.3, ymin = -0.1;
    float zmax = 4.0, zmin = -6.5;

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
