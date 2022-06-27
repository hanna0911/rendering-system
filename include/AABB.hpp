#ifndef AABB_H
#define AABB_H

#include <iostream>
#include <cmath>
#include <limits>
#include "Vector3f.h"
#include "Ray.hpp"

class aabb {
public:
    aabb() {}
    aabb(const Vector3f& a, const Vector3f& b) { minimum = a; maximum = b;}

    void set(const Vector3f& a, const Vector3f& b) { minimum = a; maximum = b;}

    Vector3f min() const { return minimum; }
    Vector3f max() const { return maximum; }

    bool intersect(const Ray& r, float &t_hit, float tmin) const {
        /*
        float t_min = 0, 
              t_max = std::numeric_limits<float>::max(); // put here
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
        t_hit = t_min; // added
        return true;
        */
        float t_min = tmin, 
              t_max = std::numeric_limits<float>::max(); // put here
        for (int a = 0; a < 3; a++) {
            auto t0 = std::min((minimum[a] - r.getOrigin()[a]) / r.getDirection()[a],
                           (maximum[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            auto t1 = std::max((minimum[a] - r.getOrigin()[a]) / r.getDirection()[a],
                           (maximum[a] - r.getOrigin()[a]) / r.getDirection()[a]);
            t_min = std::max(t0, t_min);
            t_max = std::min(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        t_hit = t_min; // added
        return true;
    }

    // computes the bounding box of two boxes
    static aabb surrounding_box(aabb box0, aabb box1) { 
        Vector3f small(fmin(box0.min().x(), box1.min().x()),
            fmin(box0.min().y(), box1.min().y()),
            fmin(box0.min().z(), box1.min().z()));
        Vector3f big(fmax(box0.max().x(), box1.max().x()),
            fmax(box0.max().y(), box1.max().y()),
            fmax(box0.max().z(), box1.max().z()));
        return aabb(small, big);
    }

protected:
    Vector3f minimum; // (minx, miny, minz)
    Vector3f maximum; // (maxx, maxy, maxz)
};

#endif