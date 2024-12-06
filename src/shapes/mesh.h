#ifndef MESH_H
#define MESH_H

#include "utils/rgba.h"
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include "raytracer/raytracer.h"
#include <optional>
#include <utils/tiny_obj_loader.h>
// #include "utils/OBJ_Loader.h"

class Mesh
{
public:
    Mesh(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection);
    std::optional<Intersection> checkIntersection();

private:
    class Triangle {
    public:
        Triangle(/*glm::vec4 wEye, glm::vec4 wDirection,*/ glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 normal);
        std::optional<Intersection> checkIntersection(glm::vec4 p, glm::vec4 d, glm::mat4 ctm);
    private:
        glm::vec4 p;
        glm::vec4 d;
        glm::vec3 m_v0;
        glm::vec3 m_v1;
        glm::vec3 m_v2;
        glm::vec3 m_normal;
    };

    RenderShapeData mesh;
    glm::vec4 pWorld;
    glm::vec4 dWorld;
    glm::vec4 p;
    glm::vec4 d;

    void load(const std::string &filePath);
    std::vector<Triangle> m_triangles;
};

#endif // MESH_H
