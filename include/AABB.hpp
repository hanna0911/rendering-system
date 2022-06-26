#ifndef AABB_H
#define AABB_H

#include <iostream>
#include <cmath>
#include "Vector3f.h"
#include "Ray.hpp"

class aabb {
public:
    aabb() {}
    aabb(const Vector3f& a, const Vector3f& b) { minimum = a; maximum = b;}

    Vector3f min() const { return minimum; }
    Vector3f max() const { return maximum; }

    bool hit(const Ray& r, float t_min, float t_max) const {
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.getDirection()[a];
        auto t0 = (min()[a] - r.getOrigin()[a]) * invD;
        auto t1 = (max()[a] - r.getOrigin()[a]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
            return false;
    }
    return true;
}

protected:
        Vector3f minimum;
        Vector3f maximum;
};

#endif