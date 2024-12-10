#ifndef PHONGILLUMINATION_H
#define PHONGILLUMINATION_H
#include "rgba.h"
#include "scenedata.h"
#include "sceneparser.h"
#include "raytracer/raytracer.h"

#endif // PHONGILLUMINATION_H

class PhongIllumination {
public:
    PhongIllumination(std::vector<SceneLightData> &inLights,
                      std::vector<RenderShapeData> inShapes,
                      SceneGlobalData gd,
                      RayTracer::Config inConfig,
                      float time);

    glm::vec4 phong(glm::vec4  position,
               glm::vec3  normal,
               glm::vec3  incdidentRay,
               float u, float v,
               SceneMaterial  &material,
               float occlusion);

private:
    RayTracer::Config config;
    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
    SceneGlobalData gd;
    int maxDepth;
    float time;

    RGBA toRGBA(const glm::vec4 &illumination);
    glm::vec4 phongLogic(glm::vec4  position,
                         glm::vec3  normal,
                         glm::vec3  incidentRay,
                         SceneMaterial  &material,
                         float u, float v,
                         int recur,
                         float occlusion);
    bool checkShadow(glm::vec3 p, glm::vec3 d, float maxT);

    glm::vec4 reflection(glm::vec4  position,
                         glm::vec3  normal,
                         glm::vec3  incidentRay,
                         SceneMaterial  &material,
                         int recur,
                         float occlusion);

    float calculateShadowFactor(SceneLightData light, glm::vec4 position);
};

