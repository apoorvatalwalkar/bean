#include "raytracer.h"
#include "raytracescene.h"
#include "shapes/sphere.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/cone.h"
#include "shapes/bvh.h"
#include "shapes/mesh_volume.h"
#include "shapes/mesh.h"
#include <cmath>
#include <optional>
#include <iostream>
#include "utils/phongillumination.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"
#include <random>


RayTracer::RayTracer(Config config) :
    m_config(config)
{}

RGBA RayTracer::toRGBA(const glm::vec4 &illumination) {
    // Task 1
    float red = 255 * fmin(fmax(illumination[0], 0), 1);
    float green = 255 * fmin(fmax(illumination[1], 0), 1);
    float blue = 255 * fmin(fmax(illumination[2], 0), 1);

    return RGBA{(std::uint8_t)red, (std::uint8_t)green, (std::uint8_t)blue, 255};
}


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

    glm::mat4 inverseCamera = camera.getInverseViewMatrix();

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            // if ((i * width + j) % 1024 == 0) {
            //     std::cout << "Working on pixel: (" << i << ", " << j << ")" << std::endl;
            // }
            glm::vec4 pixelVal = glm::vec4(0.f, 0.f, 0.f, 1.f);
            // supersampling
            for (int a = 0; a < numSamples; a++) {

            // if (i == 630 && j == 296) {
            // if ((i > 277 && i < 296) && (j > 599 && j < 629)) {

                //shoot ray through each pixel
                float x = 2 * k * tan(thetaW / 2.0) * ((j + 0.5)/width - 0.5);
                float y = 2 * k * tan(thetaH / 2.0) * ((height - i - 1 + 0.5)/height - 0.5);
                float z = 0 - k;
                float time = static_cast<float>(std::rand()) / RAND_MAX;
                PhongIllumination ph {lights, shapes, globalData, m_config, time};

                //eye + t * direction is the ray (in camera space)
                glm::vec4 cEye = {0.0, 0.0, 0.0, 1.0};
                glm::vec4 cDirection = {x, y, z, 0.0};

                glm::vec4 wEye = inverseCamera * cEye;
                glm::vec4 wDirection = inverseCamera * glm::normalize(cDirection);

                //check if ray hits any object
                std::optional<Intersection> result = checkIntersection(wEye, wDirection, shapes, time);

                if(result.has_value()){
                    Intersection intr = result.value();

                    glm::vec4 position = {wEye[0] + intr.t * wDirection[0], wEye[1] + intr.t * wDirection[1], wEye[2] + intr.t * wDirection[2], 1};

                    pixelVal += ph.phong(
                        position,
                        intr.normal,
                        camPosition - position,
                        intr.u, intr.v,
                        intr.material);
                }
            }
            imageData[i * width + j] = toRGBA(pixelVal / float(numSamples));
            // }
        }
    }
}

std::optional<Intersection> checkIntersection(glm::vec4 p, glm::vec4 d, std::vector<RenderShapeData> shapes, float time){
    Intersection closest = {float(INT_MAX), glm::vec3(0.0), shapes[0].primitive.material, 0, 0, std::nullopt};

    for(int shape = shapes.size() - 1; shape >= 0; shape--){

        RenderShapeData curr = shapes[shape];
        std::optional<Intersection> result;

        if (curr.primitive.type == PrimitiveType::PRIMITIVE_CUBE){
            Cube cube {shapes[shape], p, d};
            result = cube.checkIntersection();
        }

        if (curr.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            MeshVolume meshVolume {shapes[shape], p, d};
            if(meshVolume.checkIntersection()){
                Mesh mesh {shapes[shape], p, d};
                result = mesh.checkIntersection();
            }
        }


        // if (curr.primitive.type == PrimitiveType::PRIMITIVE_SPHERE ||
        //     curr.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER ||
        //     curr.primitive.type == PrimitiveType::PRIMITIVE_CONE){

            // Volume volume {shapes[shape], p, d};
            // if(volume.checkIntersection()){
                if (curr.primitive.type == PrimitiveType::PRIMITIVE_SPHERE){
                    Sphere sphere {shapes[shape], p, d};
                    result = sphere.checkIntersection(time);
                }
                if (curr.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER){
                    Cylinder cylinder {shapes[shape], p, d};
                    result = cylinder.checkIntersection();
                }
                if (curr.primitive.type == PrimitiveType::PRIMITIVE_CONE){
                    Cone cone {shapes[shape], p, d};
                    result = cone.checkIntersection(time);
                }
            // }
        // }

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


