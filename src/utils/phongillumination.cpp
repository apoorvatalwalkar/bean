#include "phongillumination.h"
#include <cmath>
#include <math.h>
#include <iostream>
#include "sceneparser.h"
#include "raytracer/raytracer.h"

RGBA PhongIllumination::toRGBA(const glm::vec4 &illumination) {
    // Task 1
    float red = 255 * fmin(fmax(illumination[0], 0), 1);
    float green = 255 * fmin(fmax(illumination[1], 0), 1);
    float blue = 255 * fmin(fmax(illumination[2], 0), 1);

    return RGBA{(std::uint8_t)red, (std::uint8_t)green, (std::uint8_t)blue, 255};
}

PhongIllumination::PhongIllumination(std::vector<SceneLightData> &inLights,
                                     std::vector<RenderShapeData> inShapes,
                                     SceneGlobalData globalData,
                                     RayTracer::Config inConfig){
    lights = inLights;
    shapes = inShapes;
    gd = globalData;
    config = inConfig;
}


// Calculates the RGBA of a pixel from intersection infomation and globally-defined coefficients
RGBA PhongIllumination::phong(glm::vec4  position,
                              glm::vec3  normal,
                              glm::vec3  incidentRay,
                              float u, float v,
                              SceneMaterial  &material) {
    return toRGBA(phongLogic(position, normal, incidentRay, material, u, v, 0));
}

glm::vec4 PhongIllumination::phongLogic(glm::vec4  position,
                                        glm::vec3  normal,
                                        glm::vec3  incidentRay,
                                        SceneMaterial  &material,
                                        float u, float v,
                                        int recur) {

    normal = glm::normalize(normal);
    glm::vec3 iRay = {incidentRay[0], incidentRay[1], incidentRay[2]};
    iRay = glm::normalize(iRay);

    glm::vec4 illumination(0, 0, 0, 1);
    illumination += gd.ka * material.cAmbient;

    for (const SceneLightData &light : lights) {

        glm::vec4 diffuse = gd.kd * material.cDiffuse;
        //texture
        if(config.enableTextureMap && material.map.has_value() && u < float(INT_MAX) && v < float(INT_MAX) && material.blend > 0){
            Image map =  material.map.value();
            int c = int(floor(u * map.width * material.textureMap.repeatU)) % map.width;
            int r = int((1 - v) * material.textureMap.repeatV * map.height) % map.height;
            if(floor(u) == 1){ c = map.width - 1; }
            if(v == 0){ r = map.height - 1; }
            RGBA pixel = map.data[r * map.width + c];
            glm::vec4 pix = {pixel.r / 255.f, pixel.g / 255.f, pixel.b / 255.f, 1};
            diffuse = (1 - material.blend) * gd.kd * material.cDiffuse + material.blend * pix;
        }

        //directional & point lights
        if (light.type == LightType::LIGHT_DIRECTIONAL || light.type == LightType::LIGHT_POINT){

            glm::vec3 toLight;
            if(light.type == LightType::LIGHT_DIRECTIONAL){
                toLight = -light.dir;
            } else{
                toLight = light.pos - position;
            }
            float distance = glm::length(toLight);
            toLight = glm::normalize(toLight);

            float facing = glm::dot(toLight, normal);
            float maxT = float(INT_MAX);
            if(light.type == LightType::LIGHT_POINT){ maxT = distance; }
            if(facing > 0 && config.enableShadow && checkShadow(position, toLight, maxT)){

                glm::vec3 reflection= 2 * facing * normal - toLight;
                float close = glm::dot(iRay, glm::normalize(reflection));

                float fatt = 1;
                if (light.type == LightType::LIGHT_POINT){
                    float denominator = light.function[0] + distance * light.function[1] + pow(distance, 2) * light.function[2];
                    fatt = fmin(1, 1.0 / denominator);
                }

                illumination += fatt * light.color * (diffuse * facing + gd.ks * material.cSpecular * pow(close, material.shininess));
            }
        }

        //spot light
        if(light.type == LightType::LIGHT_SPOT){
            glm::vec3 toLight = light.pos - position;
            glm::vec3 fromLight = -light.dir;

            float distance = glm::length(toLight);
            toLight = glm::normalize(toLight);
            fromLight = glm::normalize(fromLight);

            float angle = acos(glm::dot(fromLight, toLight));

            if(angle < light.angle && config.enableShadow && checkShadow(position, toLight, distance)){

                float facing = glm::dot(toLight, normal);
                glm::vec3 reflection= 2 * facing * normal - toLight;

                float close = glm::dot(iRay, glm::normalize(reflection));
                float denominator = light.function[0] + distance * light.function[1] + pow(distance, 2) * light.function[2];
                float fatt = fmin(1, 1.0 / denominator);

                float falloff = 0;
                float inner = light.angle - light.penumbra;
                if (angle > inner){
                    falloff = -2 * pow(((angle - inner) / light.penumbra), 3) + 3 * pow(((angle - inner) / light.penumbra), 2);
                }

                illumination+= fatt * light.color * (1-falloff) * (diffuse * facing +  gd.ks * material.cSpecular * pow(close, material.shininess));
            }
        }
    }

    if(config.enableShadow && glm::length(material.cReflective) > 0 && recur <= config.maxRecursiveDepth){
        illumination += gd.ks * material.cReflective * reflection(position, normal, iRay, material, recur);
    }

    return illumination;
}

glm::vec4 PhongIllumination::reflection(glm::vec4  position,
                                        glm::vec3  normal,
                                        glm::vec3  incidentRay,
                                        SceneMaterial  &material,
                                        int recur) {

    incidentRay = glm::normalize(incidentRay);
    normal = glm::normalize(normal);
    glm::vec3 reflection = 2 * glm::dot(incidentRay, normal) * normal - incidentRay;
    reflection = glm::normalize(reflection);

    glm::vec4 d = {reflection[0], reflection[1], reflection[2], 0};
    std::optional<Intersection> result = checkIntersection(position + float(0.01) * d, d, shapes);

    if(result.has_value()){
        Intersection intr = result.value();
        glm::vec4 pNew = position + (intr.t + float(0.01)) * d;

        return phongLogic(
            pNew,
            intr.normal,
            reflection,
            intr.material,
            intr.u, intr.v,
            recur + 1);
    }
    return {0, 0, 0, 1};
}


bool PhongIllumination::checkShadow(glm::vec3 p, glm::vec3 d, float maxT){
    glm::vec4 p4 = {p[0], p[1], p[2], 1};
    glm::vec4 d4 = {d[0], d[1], d[2], 0};

    std::optional<Intersection> result = checkIntersection(p4 + float(0.01) * d4, d4, shapes);
    return !result.has_value() || result.value().t > maxT;
}

