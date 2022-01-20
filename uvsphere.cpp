#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "glm/glm.hpp"

#include "primitives.hpp"

template <typename T>
struct RingBuffer : public std::vector<T> {
    T& operator[] (size_t n) {
        n %= this->size();
        return *(this->_M_impl._M_start + n);
    }

    const T& operator[] (size_t n) const {
        n %= this->size();
        return *(this->_M_impl._M_start + n);
    }
};
typedef RingBuffer<glm::vec3> LatiLine;

int main (int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " radius n_stacks n_sectors" << std::endl;
        return 0;
    }

    float radius;
    size_t n_stacks;
    size_t n_sectors;

    std::istringstream iss(argv[1]);
    iss >> radius;
    iss.clear();
    iss.str(argv[2]);
    iss >> n_stacks;
    iss.clear();
    iss.str(argv[3]);
    iss >> n_sectors;
    iss.clear();

    std::vector<LatiLine> latis(n_stacks + 1);

    float delta_u = M_PI * 2.0f / n_sectors;
    float delta_v = M_PI / n_stacks;
    
    for (size_t v = 0; v < latis.size(); ++v) {
        if (v == 0) {
            latis[v].push_back(glm::vec3(0.0f, 0.0f, 1.0f));
            continue;
        } else if (v == n_stacks) {
            latis[v].push_back(glm::vec3(0.0f, 0.0f, -1.0f));
            continue;
        } else {}

        for (size_t u = 0; u < n_sectors; ++u) {
            float xy = radius * sin(float(v) * delta_v);
            float z = radius * cos(float(v) * delta_v);
            latis[v].push_back(glm::vec3(
                                   xy * cos(float(u) * delta_u),
                                   xy * sin(float(u) * delta_u),
                                   z));
        }
    }
    
    VertexArray va;

    for (size_t v = 0; v < latis.size() - 1; ++v) {
        for (size_t u = 0; u < n_sectors; ++u) {
            glm::vec3& p00 = latis[v][u];
            glm::vec3& p10 = latis[v + 1][u];
            glm::vec3& p01 = latis[v][u + 1];
            glm::vec3& p11 = latis[v + 1][u + 1];

            glm::vec3 n00;
            glm::vec3 n10;
            glm::vec3 n01;
            glm::vec3 n11;

            if (p00 != p01) {
                normals(p00, p10, p01, n00, n10, n01);
                va.push_back({p00, n00});
                va.push_back({p10, n10});
                va.push_back({p01, n01});
            }

            if (p10 !=p11) {
                normals(p01, p10, p11, n01, n10, n11);
                va.push_back({p01, n01});
                va.push_back({p10, n10});
                va.push_back({p11, n11});
            }
        }
    }

    std::ofstream ofs;
    ofs.open("uv_vertex.data");
    for (const auto& v : va) {
        ofs << v.position.x << " " << v.position.y << " " << v.position.z << " "
            << v.normal.x << " " << v.normal.y << " " << v.normal.z << " "
            << std::endl;
    }
    ofs.close();

    return 0;
}
