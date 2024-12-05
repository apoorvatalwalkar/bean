#include "raytracer.h"
#include "raytracescene.h"
#include "shapes/sphere.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/cone.h"
#include "shapes/mesh.h"
#include <cmath>
#include <optional>
#include <iostream>
#include "utils/phongillumination.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"


RayTracer::RayTracer(Config config) :
    m_config(config)
{}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {

    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`

    // TODO: Implement the ray tracing algorithm. Good luck!

    Camera camera = scene.getCamera();

    globalData = scene.getGlobalData();
    lights = scene.getLights();
    shapes = scene.getShapes();


    glm::vec4 camCamView = glm::vec4{0, 0, 0, 1};
    glm::vec4 camPosition = camera.getInverseViewMatrix() * camCamView;

    int width = scene.width();
    int height = scene.height();

    float thetaH = camera.getHeightAngle();
    float thetaW = camera.getWidthAngle();
    int k = 1;

    PhongIllumination ph {lights, shapes, globalData, m_config};

    glm::mat4 inverseCamera = camera.getInverseViewMatrix();
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){

            if ((i > 409 && i < 420) && (j > 489 && j < 500)) {

                //shoot ray through each pixel
                float x = 2 * k * tan(thetaW / 2.0) * ((j + 0.5)/width - 0.5);
                float y = 2 * k * tan(thetaH / 2.0) * ((height - i - 1 + 0.5)/height - 0.5);
                float z = 0 - k;

                //eye + t * direction is the ray (in camera space)
                glm::vec4 cEye = {0.0, 0.0, 0.0, 1.0};
                glm::vec4 cDirection = {x, y, z, 0.0};

                glm::vec4 wEye = inverseCamera * cEye;
                glm::vec4 wDirection = inverseCamera * glm::normalize(cDirection);

                //check if ray hits any object
                std::optional<Intersection> result = checkIntersection(wEye, wDirection, shapes);

                if(result.has_value()){
                    Intersection intr = result.value();

                    glm::vec4 position = {wEye[0] + intr.t * wDirection[0], wEye[1] + intr.t * wDirection[1], wEye[2] + intr.t * wDirection[2], 1};

                    RGBA pixelVal = ph.phong(
                        position,
                        intr.normal,
                        camPosition - position,
                        intr.u, intr.v,
                        intr.material);

                    imageData[i * width + j] = pixelVal;
                }
            }
        }
    }
}

std::optional<Intersection> checkIntersection(glm::vec4 p, glm::vec4 d, std::vector<RenderShapeData> shapes){
    Intersection closest = {float(INT_MAX)};

    for(int shape = shapes.size() - 1; shape >= 0; shape--){

        RenderShapeData curr = shapes[shape];
        std::optional<Intersection> result;
        if (curr.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            Mesh mesh {shapes[shape], p, d};
            result = mesh.checkIntersection();
        }

        if (curr.primitive.type == PrimitiveType::PRIMITIVE_SPHERE){
            Sphere sphere {shapes[shape], p, d};
            result = sphere.checkIntersection();
        }
        if (curr.primitive.type == PrimitiveType::PRIMITIVE_CUBE){
            Cube cube {shapes[shape], p, d};
            result = cube.checkIntersection();
        }
        if (curr.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER){
            Cylinder cylinder {shapes[shape], p, d};
            result = cylinder.checkIntersection();
        }
        if (curr.primitive.type == PrimitiveType::PRIMITIVE_CONE){
            Cone cone {shapes[shape], p, d};
            result = cone.checkIntersection();
        }

        if(result.has_value() && result.value().t < closest.t && result.value().t >= 0){
            closest.t = result->t;
            closest.normal = result->normal;
            closest.material = curr.primitive.material;
            closest.u = result->u;
            closest.v = result->v;
            if(curr.image.has_value()){
                closest.material.map = *(curr.image.value());
            } else {
                closest.material.map = std::nullopt;
            }
        }
    }

    if(closest.t < float(INT_MAX)){
        return closest;
    }

    return std::nullopt;
}


