
template <typename T>
struct RingBuffer<T> : public std::vector<T> {
    T& operator[] (size_t n) {
        return n %= this->size();
    }

    const T& operator[] (size_t n) {
        return n %= this->size();
    }
};

typedef RingBuffer<glm::vec3> LatiLine;

int main (int argc, char** argv) {
    size_t n_stacks;
    size_t n_sectors;
    
    std::vector<LatiLine> latis(n_stacks + 1);

    float delta_u = M_PI * 2.0f / n_sectors;
    float delta_v = M_PI * 2.0f / n_stacks;
    
    for (size_t v = 0; v < latis.size(); ++v) {
        for (size_t u = 0; u < n_sectors; ++u) {
            float xy = r * sin(v * delta_v);
            float z = r * cos(v * delta_v);
            latis[v].push_back(glm::vec3(
                                   xy * cos(u * delta_u),
                                   xy * sin(u * delta_u),
                                   z));
        }
    }
    
    for (size_t v = 0; v < latis.size() - 1; ++v) {
        for (size_t u = 0; u < n_sectors; ++u) {
            glm::vec3& p00 = latis[v][u];
            glm::vec3& p10 = latis[v + 1][u];
            glm::vec3& p01 = latis[v][u + 1];
            glm::vec3& p11 = latis[v + 1][u + 1];
        }
        
    }

    
}
