#include <stdexcept>
#include "camera.h"
#include <cmath>

glm::mat4 Camera::getViewMatrix() const {

    // Optional TODO: implement the getter or make your own design
    glm::vec3 pos = cameraData.pos;
    glm::vec3 look = cameraData.look;
    glm::vec3 up = cameraData.up;

    glm::mat4 Mtranslate = glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -pos[0], -pos[1], -pos[2], 1);

    glm::vec3 w = - normalize(look);
    glm::vec3 temp = up - dot(w, up)* w;
    glm::vec3 v = normalize(temp);
    glm::vec3 u = cross(v, w);

    glm::mat4 Mrotate = glm::mat4(
        u[0], v[0], w[0], 0,
        u[1], v[1], w[1], 0,
        u[2], v[2], w[2], 0,
        0,   0,    0,  1);

    return Mrotate * Mtranslate;
}
glm::mat4 Camera::getInverseViewMatrix() const {
    glm::mat4 viewMatrix = getViewMatrix();
    return glm::inverse(viewMatrix);
}

float Camera::getAspectRatio() const {
    return float(width)/float(height);
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return cameraData.heightAngle;
}

float Camera::getWidthAngle() const{
    float tanWidth = getAspectRatio() * tan(cameraData.heightAngle / 2.f);
    float widthAngle = 2 * atan(tanWidth);
    return widthAngle;

}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}
