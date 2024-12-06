#include "mesh.h"

Mesh::Mesh(RenderShapeData shape, glm::vec4 wEye, glm::vec4 wDirection){
    mesh = shape;
    pWorld = wEye;
    dWorld = wDirection;
    p = shape.inverse * wEye;
    d = shape.inverse * wDirection;
    load(shape.primitive.meshfile);
}


std::optional<Intersection> Mesh::checkIntersection(){
    Intersection intr = {.t = float(INT_MAX), .u = float(INT_MAX), .v = float(INT_MAX)};
    for (int i = 0; i < m_triangles.size(); ++i) {
        std::optional<Intersection> result = m_triangles[i].checkIntersection(p, d, mesh.ctm);
        // may need to check result value
        if (result.has_value() && result->t < intr.t && result->t >= 1e-4) {
            intr.t = result->t;
            intr.normal = result->normal;
            intr.u = result->u;
            intr.v = result->v;
        }
    }
    if (intr.t < float(INT_MAX)) {
        intr.normal = glm::normalize(glm::transpose(mesh.inverse) * glm::vec4(intr.normal, 0.0f));
        return intr;
    }
    return std::nullopt;
}

void Mesh::load(const std::string& filepath) {
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return;
    }
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces (triangles)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            glm::vec3 v0;
            glm::vec3 v1;
            glm::vec3 v2;
            glm::vec3 normal;

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                switch (v) {
                case 0: {
                    v0 = glm::vec3(
                            attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                            attrib.vertices[3 * size_t(idx.vertex_index) + 1],
                            attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
                    break;
                }
                case 1: {
                    v1 = glm::vec3(
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
                    break;
                }
                case 2: {
                    v2 = glm::vec3(
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1],
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
                    break;
                }
                }
                if (idx.normal_index >= 0) {
                    normal = glm::vec3(
                        attrib.normals[3*size_t(idx.normal_index)+0],
                        attrib.normals[3*size_t(idx.normal_index)+1],
                        attrib.normals[3*size_t(idx.normal_index)+2]);
                } else {
                    break;
                }
                m_triangles.push_back(Triangle{v0, v1, v2, normal});
            }
            index_offset += fv;
        }
    }
}

Mesh::Triangle::Triangle(/*glm::vec4 wEye, glm::vec4 wDirection,*/ glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 normal)
    : m_v0(v0), m_v1(v1), m_v2(v2), m_normal(normal) {}

std::optional<Intersection> Mesh::Triangle::checkIntersection(glm::vec4 p, glm::vec4 d, glm::mat4 ctm) {
    float epsilon = 1e-7;
    glm::vec3 edge1 = m_v1 - m_v0;
    glm::vec3 edge2 = m_v2 - m_v0;
    glm::vec3 ray_cross_e2 = glm::cross(glm::vec3(d), edge2);
    float det = glm::dot(edge1, ray_cross_e2);
    if (det > -epsilon && det < epsilon) {
        return std::nullopt;
    }

    float inv_det = 1.f / det;
    glm::vec3 s = glm::vec3(p) - m_v0;
    float u = inv_det * glm::dot(s, ray_cross_e2);

    if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u-1) > epsilon)) {
        return std::nullopt;
    }

    glm::vec3 s_cross_e1 = glm::cross(s, edge1);
    float v = inv_det * glm::dot(glm::vec3(d), s_cross_e1);
    if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon)) {
        return std::nullopt;
    }
    float t = inv_det * dot(edge2, s_cross_e1);
    if (t > epsilon) {
        glm::mat3 m33;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                m33[i][j] = ctm[i][j];
            }
        }
        Intersection intr;
        intr.t = t;
        // intr.normal = glm::normalize(glm::transpose(glm::inverse(m33)) * m_normal);
        intr.normal = glm::normalize(glm::cross(m_v1 - m_v0, m_v2 - m_v0));
        intr.u = u;
        intr.v = v;
        return intr;
    } else {
        return std::nullopt;
    }
}
