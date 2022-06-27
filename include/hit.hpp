#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include <limits>

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        // t = 1e38;
        t = std::numeric_limits<float>::max();
    }

    Hit(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    void set(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        if(!material) std::cout << "set material failed" << std::endl;
        normal = n;
    }

    void initialize() { t = std::numeric_limits<float>::max(); }

private:
    float t;
    Material *material;
    Vector3f normal;

};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
