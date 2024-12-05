#ifndef BVH_H
#define BVH_H

#include "utils/sceneparser.h"

class Volume {
public:
    Volume(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    bool checkIntersection();
private:
    RenderShapeData volume;
    glm::vec4 p;
    glm::vec4 d;
};

#endif // BVH_H
