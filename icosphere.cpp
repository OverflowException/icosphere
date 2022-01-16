#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>

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

// return vertex buffer and index buffer
void init_ico(float r,
              PositionBuffer& pb,
              IndexBuffer& ib) {
    pb.clear();
    ib.clear();
    
    // top-most
    pb.push_back(glm::vec3(0.0f, 0.0f, r));

    // top layer
    float z = std::sin(std::atan(0.5));
    float xy = std::cos(std::atan(0.5));
    for (int i = 0; i < 5; ++i) {
        pb.push_back(glm::vec3(
                float(xy * std::cos(glm::radians(72.0 * i))),
                float(xy * std::sin(glm::radians(72.0 * i))),
                z));
    }
    
    // bottom layer
    for (int i = 0; i < 5; ++i) {
        pb.push_back(glm::vec3(
                float(xy * std::cos(glm::radians(36.0 + 72.0 * i))),
                float(xy * std::sin(glm::radians(36.0 + 72.0 * i))),
                -z));
    }
    
    // bottom-most
    pb.push_back(glm::vec3(0.0f, 0.0f, -r));

    // construct triangles
    // the 'roof'
    for (size_t i = 1; i <= 4; ++i) {
        ib.push_back({0, i, i + 1});
    }
    ib.push_back({0, 5, 1});
    
    // the 'walls'
    for (size_t i = 1; i <= 4; ++i) {
        ib.push_back({i, i + 5, i + 1});
    }
    ib.push_back({5, 10, 1});
    
    for (size_t i = 2; i <= 5; ++i) {
        ib.push_back({i, i + 4, i + 5});
    }
    ib.push_back({1, 10, 6});

    // the 'bottom'
    for (size_t i = 6; i <= 9; ++i) {
        ib.push_back({11, i + 1, i});
    }
    ib.push_back({11, 6, 10});
}

void add_ico_level(float r,
                   PositionBuffer& pb,
                   IndexBuffer& ib) {
    // traverse triangles
    IndexBuffer ib_new;
    for (size_t k = 0; k < ib.size(); ++k) {
        glm::vec3& i = ib[k];

        glm::vec3 p0 = pb[i.x];
        glm::vec3 p1 = pb[i.y];
        glm::vec3 p2 = pb[i.z];
        
        glm::vec3 p01 = extrusion(r, p0, p1);
        glm::vec3 p12 = extrusion(r, p1, p2);
        glm::vec3 p20 = extrusion(r, p2, p0);
        
        // add new vertices to vb
        pb.push_back(p01);
        pb.push_back(p12);
        pb.push_back(p20);
        
        // add new indices
        size_t pos01 = pb.size() - 3;
        size_t pos12 = pb.size() - 2;
        size_t pos20 = pb.size() - 1;
        ib_new.push_back(glm::vec3(i.x, pos01, pos20));
        ib_new.push_back(glm::vec3(i.y, pos12, pos01));
        ib_new.push_back(glm::vec3(i.z, pos20, pos12));
        ib_new.push_back(glm::vec3(pos20, pos01, pos12));
    }

    ib.clear();
    ib = std::move(ib_new);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " radius level" << std::endl;
        return 0;
    }

    float radius;
    int level;
    std::istringstream iss(argv[1]);
    iss >> radius;
    iss.clear();
    iss.str(argv[2]);
    iss >> level;
    
    PositionBuffer pb;
    IndexBuffer ib;
    init_ico(radius, pb, ib);
    for (int l = 0; l < level; ++l) {
        add_ico_level(radius, pb, ib);
    }

    std::ofstream ofs;
    ofs.open("ico_pos.data");
    for (const auto& p : pb) {
        ofs << p.x << " " << p.y << " " << p.z << std::endl;
    }
    ofs.close();

    ofs.open("ico_index.data");
    for (const auto& i : ib) {
        ofs << i.x << " " << i.y << " " << i.z << std::endl;
    }
    ofs.close();

    // generate normals with position buffer and index buffer
    NormalBuffer nb;
    VertexArray va;
    for (const auto& i : ib) {
        const glm::vec3& p0 = pb[i.x];
        const glm::vec3& p1 = pb[i.y];
        const glm::vec3& p2 = pb[i.z];

        const glm::vec3& n0 = glm::normalize(glm::cross(p1 - p0, p2 - p0));
        const glm::vec3& n1 = glm::normalize(glm::cross(p2 - p1, p0 - p1));
        const glm::vec3& n2 = glm::normalize(glm::cross(p0 - p2, p1 - p2));

        nb.push_back(n0);
        nb.push_back(n1);
        nb.push_back(n2);

        va.push_back({p0, n0});
        va.push_back({p1, n1});
        va.push_back({p2, n2});
    }

    ofs.open("ico_vertex.data");
    for (const auto& v : va) {
        ofs << v.position.x << " " << v.position.y << " " << v.position.z << " "
            << v.normal.x << " " << v.normal.y << " " << v.normal.z << " "
            << std::endl;
    }
    ofs.close();

    return 0;
}
