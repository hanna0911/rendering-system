#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    // Plane() {
    //     this->normal = Vector3f(0, 0, 0);
    //     this->D = 0;
    // }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->normal = normal; 
        this->D = -d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        if(!material) std::cout << "plane no material" << std::endl;
        Vector3f R0 = r.getOrigin(); // 光线起始点
        Vector3f Rd = r.getDirection(); Rd.normalize(); // 光线单位方向向量
        float t = - ((D + normal.dot(normal, R0)) / normal.dot(normal, Rd));
        t = t / r.getDirection().length(); // Transform
        if(tmin < t && t < h.getT()){
            h.set(t, material, normal);
            if(!h.getMaterial()) std::cout << "plane no material" << std::endl;
            return true; // 不管光源在球体内部还是外部，都必定有交点
        }
        else return false;
    }

protected:
    float D; // n·P + D = 0 
    Vector3f normal; // 法向量
};

#endif //PLANE_H
		

