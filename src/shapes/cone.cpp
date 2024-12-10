#include "utils/rgba.h"
#include "cone.h"
#include "utils/sceneparser.h"

Cone::Cone(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    coneShape = shape;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

std::optional<Intersection> Cone::checkIntersection(float time){
    glm::vec4 newCenter = centerStart + (centerEnd - centerStart) * time;
    // calculate relative camera position for the intersection after the sphere moves
    // p -= newCenter;

    Intersection intr = {.t = float(INT_MAX), .u = float(INT_MAX), .v = float(INT_MAX)};
    float t1;

    // - xz plane intersection
    t1 = (-0.5 - p[1]) / d[1];
    float x = p[0] + t1 * d[0];
    float z = p[2] + t1 * d[2];
    if(t1 < intr.t && pow(x, 2) + pow(z, 2) < 0.25){
        intr.t = t1;
        intr.normal = {0.0, -1.0, 0.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 + (p[2] + t1 * d[2]);
    }

    // round section
    float px2 = p[0] * p[0];
    float py2 = p[1] * p[1];
    float pz2 = p[2] * p[2];

    float dx2 = d[0] * d[0];
    float dy2 = d[1] * d[1];
    float dz2 = d[2] * d[2];

    float A = dx2 + dz2 - 0.25 * dy2;
    float B = 2 * (p[0] * d[0] + p[2] * d[2]) - 0.5 * p[1] * d[1] + 0.25 * d[1];
    float C = px2 + pz2 - 0.25 * py2 + 0.25 * p[1] - 0.0625;

    float discriminant = (pow(B, 2) - 4 * A * C);

    if(discriminant >= 0){
        float tfirst =( -B + pow(discriminant, 0.5)) / (2 * A);
        float y = p[1] + tfirst * d[1];
        if(-0.5 <= y && y <= 0.5 && tfirst < intr.t){
            intr.t = tfirst;
            float x = p[0] + intr.t * d[0];

            float z = p[2] + intr.t * d[2];
            intr.normal = {2 * x, (0.5 - y) / 2, 2 * z};
            intr.normal = glm::normalize(intr.normal);

            float theta = atan2(z, x);
            if(theta >= 0){ intr.u = 1 - theta / (2 * M_PI); }
            else { intr.u = - theta / (2 * M_PI); }
            intr.v = y + 0.5;
        }

        float tsecond =( -B - pow(discriminant, 0.5)) / (2 * A);
        y = p[1] + tsecond * d[1];
        if(-0.5 <= y && y <= 0.5 && tsecond < intr.t){
            intr.t = tsecond;
            float x = p[0] + intr.t * d[0];
            float z = p[2] + intr.t * d[2];
            intr.normal = {2 * x, (0.5 - y) / 2, 2 * z};
            intr.normal = glm::normalize(intr.normal);

            float theta = atan2(z, x);
            if(theta >= 0){ intr.u = 1 - theta / (2 * M_PI); }
            else { intr.u = - theta / (2 * M_PI); }
            intr.v = y + 0.5;
        }
    }

    if(intr.t < float(INT_MAX)){

        glm::mat3 m33;
        glm::mat4 ctm = coneShape.ctm;
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
