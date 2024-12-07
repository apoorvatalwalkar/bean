#ifndef MESH_VOLUME_H
#define MESH_VOLUME_H

#include "utils/sceneparser.h"

class MeshVolume {
public:
    MeshVolume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    bool checkIntersection();
private:
    glm::vec4 p;
    glm::vec4 d;
};

#endif // MESH_VOLUME_H
