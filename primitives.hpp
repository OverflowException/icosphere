#include <vector>
#include "glm/glm.hpp"

typedef std::vector<glm::vec3> PositionBuffer;
typedef std::vector<glm::vec3> IndexBuffer;
typedef std::vector<glm::vec3> NormalBuffer;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};
typedef std::vector<Vertex> VertexArray;

glm::vec3 extrusion(float r, const glm::vec3& p0, const glm::vec3& p1) {
    glm::vec3 mid = (p0 + p1) * 0.5f;
    return r * glm::normalize(mid);
}

void normals(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    glm::vec3& n0,
    glm::vec3& n1,
    glm::vec3& n2) {

    n0 = glm::normalize(glm::cross(p1 - p0, p2 - p0));
    n1 = glm::normalize(glm::cross(p2 - p1, p0 - p1));
    n2 = glm::normalize(glm::cross(p0 - p2, p1 - p2));
}