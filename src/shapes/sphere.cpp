#include "utils/rgba.h"
#include "sphere.h"
#include "utils/sceneparser.h"

Sphere::Sphere(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    sphereShape = shape;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

std::optional<Intersection> Sphere::checkIntersection(float time) {
    glm::vec4 newCenter = centerStart + (centerEnd - centerStart) * time;
    // calculate relative camera position for the intersection after the sphere moves
    glm::vec4 newPos = p - newCenter;

    Intersection intr = {.t = float(INT_MAX), .u = float(INT_MAX), .v = float(INT_MAX)};

    float px2 = newPos[0] * newPos[0];
    float py2 = newPos[1] * newPos[1];
    float pz2 = newPos[2] * newPos[2];

    float dx2 = d[0] * d[0];
    float dy2 = d[1] * d[1];
    float dz2 = d[2] * d[2];

    float A = dx2 + dy2 + dz2;
    float B = 2 * (p[0] * d[0] + newPos[1] * d[1] + newPos[2] * d[2]);
    float C = px2 + py2 + pz2 - 0.25;

    float discriminant = (pow(B, 2) - 4 * A * C);

    if(discriminant >= 0){
        float tfirst =( -B + pow(discriminant, 0.5)) / (2 * A);
        float tsecond =( -B - pow(discriminant, 0.5)) / (2 * A);
        intr.t = fmin(tfirst, tsecond);

        float x = p[0] + intr.t * d[0];
        float y = p[1] + intr.t * d[1];
        float z = p[2] + intr.t * d[2];
        intr.normal = {x, y, z};
        intr.normal = glm::normalize(intr.normal);

        float phi = asin(y / 0.5);
        intr.v = phi / M_PI + 0.5;

        if(intr.v == 0 || intr.v == 1){
            intr.u = 0.5;
        }
        else{
            float theta = atan2(z, x);
            if(theta >= 0){ intr.u = 1 - theta / (2 * M_PI); }
            else { intr.u = - theta / (2 * M_PI); }
        }
    }

    if(intr.t < float(INT_MAX)){

        glm::mat4 ctm = sphereShape.ctm;
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
