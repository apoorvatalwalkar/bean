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


RayTracer::RayTracer(Config config, QImage image, QString imagePath):
    m_config(config)
{
    m_image = image;
    m_imagePath = imagePath;
}

void RayTracer::render(const RayTraceScene &scene) {

    SceneCameraData original = scene.cameraData;
    SceneCameraData currData = scene.cameraData;

    int frames = 1;
    if(m_config.cameraMovement){ frames = 160;}

    for(int i = 0; i < frames; i++){
        m_image.fill(Qt::black);
        RGBA *imageData = reinterpret_cast<RGBA *>(m_image.bits());

        SceneCameraData newData = original;
        RayTraceScene newScene = scene;

        // camera movement code
        if(m_config.cameraMovement) {

            glm::vec3 up3 = glm::vec3(currData.up);
            glm::vec3 look3 = glm::vec3(currData.look);
            glm::vec3 perp = glm::cross(up3, look3);

            glm::mat4 rotationX = myRotate(-0.6, glm::vec3(0, 1, 0));
            newData.look = rotationX * currData.look;
            newData.up = rotationX * currData.up;

            newData.pos = currData.pos - float(i / 180.f) * glm::normalize(currData.look) - float(i/180.f) * glm::vec4(perp, 0.f);

            newScene.cameraData = newData;
            currData = newData;
        }

        // render scene
        renderOneScene(imageData, newScene);

        //save image
        bool success;
        if(m_imagePath.contains("png")){
            success = m_image.save(m_imagePath);
            if (success) {
                std::cout << "Saved rendered image to \"" << m_imagePath.toStdString() << "\"" << std::endl;
            } else {
                std::cerr << "Error: failed to save image to \"" << m_imagePath.toStdString() << "\"" << std::endl;
            }
        }
        else{
            success = m_image.save(m_imagePath + QString::fromStdString(std::to_string(i)) + ".png");

            if (success) {
                std::cout << "Saved rendered image to \"" << m_imagePath.toStdString() << i << ".png\"" << std::endl;
            } else {
                std::cerr << "Error: failed to save image to \"" << m_imagePath.toStdString() << i << ".png\"" << std::endl;
            }
        }
        if (!success) {
            success = m_image.save(m_imagePath, "PNG");
            if (success) {
                std::cout << "Saved rendered image to \"" << m_imagePath.toStdString() << "\"" << std::endl;
            } else {
                std::cerr << "Error: failed to save image to \"" << m_imagePath.toStdString() << "\"" << std::endl;
            }
        }

    }
}

RGBA RayTracer::toRGBA(const glm::vec4 &illumination) {
    // Task 1
    float red = 255 * fmin(fmax(illumination[0], 0), 1);
    float green = 255 * fmin(fmax(illumination[1], 0), 1);
    float blue = 255 * fmin(fmax(illumination[2], 0), 1);

    return RGBA{(std::uint8_t)red, (std::uint8_t)green, (std::uint8_t)blue, 255};
}


void RayTracer::renderOneScene(RGBA *imageData, const RayTraceScene &scene) {

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
                    float occlusion = calcOcclusion(position, intr.normal, shapes, time);

                    pixelVal += ph.phong(
                        position,
                        intr.normal,
                        camPosition - position,
                        intr.u, intr.v,
                        intr.material,
                        occlusion);
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

glm::vec3 RayTracer::hemisphereSample(glm::vec3 normal) {
    float theta = 2 * M_PI * ((float)rand()) / RAND_MAX;
    float phi = acos(2 * ((float)rand()) / RAND_MAX - 1);
    glm::vec3 coords = glm::vec3(sin(phi) * cos(theta),
                                 sin(phi) * sin(theta),
                                 cos(phi));

    glm::vec3 up = glm::vec3(0, 1, 0);
    if (std::abs(dot(normal, up)) > 0.99f) {
        up = glm::vec3(1, 0, 0);
    }

    glm::vec3 right = normalize(cross(normal, up));
    glm::vec3 forward = normalize(cross(right, normal));

    glm::vec3 raydir = glm::vec3(
        coords.x * right.x + coords.y * forward.x + coords.z * normal.x,
        coords.x * right.y + coords.y * forward.y + coords.z * normal.y,
        coords.x * right.z + coords.y * forward.z + coords.z * normal.z
    );


    return glm::normalize(raydir);
}

float RayTracer::calcOcclusion(glm::vec4 surfacePos, glm::vec3 surfaceNormal, std::vector<RenderShapeData> shapes, float time) {
    int numSamples = 128;
    float occlusion = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        glm::vec4 d = glm::vec4(hemisphereSample(surfaceNormal), 0.0f);
        std::optional<Intersection> result = checkIntersection(surfacePos + 0.01f * d, d, shapes, time);
        if (!result.has_value()) {
            occlusion += 1.0f;
        }
    }

    return occlusion / numSamples;
}

glm::mat4 RayTracer::myRotate(float angleDegrees, glm::vec3 axis) {
    float angleRadians = glm::radians(angleDegrees);
    axis = glm::normalize(axis);

    float cosTheta = cos(angleRadians);
    float sinTheta = sin(angleRadians);
    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];

    glm::mat4 rotationMatrix(1.f);
    rotationMatrix[0][0] = cosTheta + pow(ux, 2) * (1 - cosTheta);
    rotationMatrix[0][1] = ux * uy * (1 - cosTheta) - uz * sinTheta;
    rotationMatrix[0][2] = ux * uz * (1 - cosTheta) + uy * sinTheta;
    rotationMatrix[1][0] = uy * ux * (1 - cosTheta) + uz * sinTheta;
    rotationMatrix[1][1] = cosTheta + pow(uy, 2) * (1 - cosTheta);
    rotationMatrix[1][2] = uy * uz * (1 - cosTheta) - ux * sinTheta;
    rotationMatrix[2][0] = uz * ux * (1 - cosTheta) - uy * sinTheta;
    rotationMatrix[2][1] = uz * uy * (1 - cosTheta) + ux * sinTheta;
    rotationMatrix[2][2] = cosTheta + pow(uz, 2) * (1 - cosTheta);

    return rotationMatrix;
}


