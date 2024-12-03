#include "utils/rgba.h"
#include "cylinder.h"
#include "utils/sceneparser.h"
#include <iostream>


Cylinder::Cylinder(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    cylinderShape = shape;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
}

std::optional<Intersection> Cylinder::checkIntersection(){

    Intersection intr = {.t = float(INT_MAX), .u = float(INT_MAX), .v = float(INT_MAX)};
    float t1;

    // + xz plane intersection
    t1 = (0.5 - p[1]) / d[1];
    float x = p[0] + t1 * d[0];
    float z = p[2] + t1 * d[2];
    if(pow(x, 2) + pow(z, 2) < 0.25){
        intr.t = t1;
        intr.normal = {0.0, 1.0, 0.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 - (p[2] + t1 * d[2]);
    }

    // - xz plane intersection
    t1 = (-0.5 - p[1]) / d[1];
    x = p[0] + t1 * d[0];
    z = p[2] + t1 * d[2];
    if(t1 < intr.t && pow(x, 2) + pow(z, 2) < 0.25){
        intr.t = t1;
        intr.normal = {0.0, -1.0, 0.0};
        intr.u = 0.5 + (p[0] + t1 * d[0]);
        intr.v = 0.5 + (p[2] + t1 * d[2]);
    }

    // round section
    float A = pow(d[0], 2) + pow(d[2], 2);
    float B = 2.0 * (p[0] * d[0] + p[2] * d[2]);
    float C = pow(p[0], 2) + pow(p[2] , 2) - 0.25;

    float discriminant = (pow(B, 2) - 4 * A * C);

    if(discriminant >= 0){

        float tfirst =( -B + pow(discriminant, 0.5)) / (2 * A);
        float tsecond =( -B - pow(discriminant, 0.5)) / (2 * A);
        float y = p[1] + fminf(tfirst, tsecond) * d[1];

        if(fmin(tfirst, tsecond) < intr.t && -0.5 <= y && y <= 0.5){
            intr.t = fminf(tfirst, tsecond);
            x = p[0] + intr.t * d[0];
            z = p[2] + intr.t * d[2];
            intr.normal = {x/0.5, 0, z/0.5};

            float theta = atan2(z, x);
            if(theta >= 0){ intr.u = 1 - theta / (2 * M_PI); }
            else { intr.u = - theta / (2 * M_PI); }
            intr.v = y + 0.5;
        }
    }

    if(intr.t < float(INT_MAX)){
        glm::mat3 m33;
        glm::mat4 ctm = cylinderShape.ctm;

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
