#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include "AABB.hpp"

// Base class for all 3d entities.
class Object3D {
public:
    // Object3D() : material(nullptr) {}
    Object3D() {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material) {
        this->material = material;
    }

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    
    //  compute the bounding boxes of all the hittable objects
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;

protected:

    Material *material;
};

#endif

