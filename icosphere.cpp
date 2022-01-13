#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>

struct Vertex {
    float x;
    float y;
    float z;
};
typedef std::vector<Vertex> VertexBuffer;

struct Triangle {
    size_t i0;
    size_t i1;
    size_t i2;
};
typedef std::vector<Triangle> IndexBuffer;

Vertex extrusion(float r, const Vertex& v0, const Vertex& v1) {
    float x, y, z;
    x = (v0.x + v1.x) / 2;
    y = (v0.y + v1.y) / 2;
    z = (v0.z + v1.z) / 2;
    
    float ratio = r / std::sqrt((x * x + y * y + z * z));

    return Vertex {x * ratio, y * ratio, z * ratio};
}

// return vertex buffer and index buffer
void init_ico(float r,
              VertexBuffer& vb,
              IndexBuffer& ib) {
    vb.clear();
    ib.clear();
    
    // top-most
    vb.push_back({0.0, 0.0, r});

    // top layer
    float z = std::sin(std::atan(0.5));
    float xy = std::cos(std::atan(0.5));
    for (int i = 0; i < 5; ++i) {
        vb.push_back({
                float(xy * std::cos(72.0 * i)),
                float(xy * std::sin(72.0 * i)),
                z});
    }
    
    // bottom layer
    for (int i = 0; i < 5; ++i) {
        vb.push_back({
                float(xy * std::cos(36.0 + 72.0 * i)),
                float(xy * std::sin(36.0 + 72.0 * i)),
                -z});
    }
    
    // bottom-most
    vb.push_back({0.0, 0.0, -r});

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
                   VertexBuffer& vb,
                   IndexBuffer& ib) {
    // traverse triangles
    IndexBuffer ib_new;
    for (size_t k = 0; k < ib.size(); ++k) {
        Triangle i = ib[k];

        Vertex v0 = vb[i.i0];
        Vertex v1 = vb[i.i1];
        Vertex v2 = vb[i.i2];
        
        Vertex v01 = extrusion(r, v0, v1);
        Vertex v12 = extrusion(r, v1, v2);
        Vertex v20 = extrusion(r, v2, v0);
        
        // add new vertices to vb
        vb.push_back(v01);
        vb.push_back(v12);
        vb.push_back(v20);
        
        // add new indices
        size_t pos01 = vb.size() - 3;
        size_t pos12 = vb.size() - 2;
        size_t pos20 = vb.size() - 1;
        ib_new.push_back({i.i0, pos01, pos20});
        ib_new.push_back({i.i1, pos12, pos01});
        ib_new.push_back({i.i2, pos20, pos12});
        ib_new.push_back({pos20, pos01, pos12});
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
    
    VertexBuffer vb;
    IndexBuffer ib;
    init_ico(radius, vb, ib);
    for (int l = 0; l < level; ++l) {
        add_ico_level(radius, vb, ib);
    }
    
    return 0;
}
