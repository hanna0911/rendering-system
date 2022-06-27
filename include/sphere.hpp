#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    // Sphere() {
    //     // unit ball at the center
    //     center = Vector3f(0, 0, 0);
    // }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // TODO
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // TODO
        if(!material) std::cout << "sphere no material" << std::endl;
        Vector3f R0 = r.getOrigin(); // 光线起始点
        Vector3f Rd = r.getDirection(); Rd.normalize(); // 光线单位方向向量
        Vector3f l = center - R0; // 由光源指向球心的向量
        if(l.squaredLength() == radius * radius){
            if(tmin <= 0 && 0 <= h.getT()){
                h.set(0, material, (R0 - center).normalized()); // ????
                return true; // 光源位于球面上，算作相交（光源即相交点）
            }
        }
        
        float tp = l.dot(l, Rd); // 球心到光线所在直线的投影点 (垂足)
        if(l.squaredLength() > radius * radius && tp < 0) return false; // 如果光源在球体外部并且 tp < 0，那么光线与球面不相交

        float d = sqrt(l.squaredLength() - tp * tp); // 球心到光线所在直线的距离
        if(d > radius) return false; // 如果 d > r，那么光线与球面不相交
        
        float t_ = sqrt(radius * radius - d * d); // 投影点到光线与球面的交点的距离
        float t;
        Vector3f N; // 相交处法向（指向球外）
        if(l.squaredLength() > radius * radius){ // 光源在球体外部
            t = tp - t_;
            N = R0 + t * Rd - center;
        }
        else{ // 光源在球体内部
            t = tp + t_;
            N = R0 + t * Rd - center;
        }
        t = t / r.getDirection().length(); // Transform
        if(tmin < t && t < h.getT()){
            h.set(t, material, N);
            return true; // 不管光源在球体内部还是外部，都必定有交点
        }
        else return false;
    }

    /*
    bool bounding_box(float time0, float time1, aabb& output_box) const {
        output_box = aabb(
            center - Vector3f(radius, radius, radius),
            center + Vector3f(radius, radius, radius));
        return true;
    }
    */

protected:
    Vector3f center; // 球心
    float radius; // 半径
};


class MovingSphere : public Sphere {

public:
    MovingSphere(const Vector3f &center_0, const Vector3f &center_1, float tm0, float tm1, float radius, Material *material) : 
        Sphere(Vector3f::ZERO, radius, material), 
            center0(center_0),
            center1(center_1),
            time0(tm0),
            time1(tm1) {}
    
    Vector3f center (float time) const {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // The moving_sphere::hit() function is almost identical to the sphere::hit() function: 
        // center just needs to become a function center(time)
        
        float time = r.getTime();
        Vector3f R0 = r.getOrigin(); // 光线起始点
        Vector3f Rd = r.getDirection(); Rd.normalize(); // 光线单位方向向量
        Vector3f l = center(time) - R0; // 由光源指向球心的向量
        if(l.squaredLength() == radius * radius){
            if(tmin <= 0 && 0 <= h.getT()){
                h.set(0, material, (R0 - center(time)).normalized()); // ????
                return true; // 光源位于球面上，算作相交（光源即相交点）
            }
        }
        
        float tp = l.dot(l, Rd); // 球心到光线所在直线的投影点 (垂足)
        if(l.squaredLength() > radius * radius && tp < 0) return false; // 如果光源在球体外部并且 tp < 0，那么光线与球面不相交

        float d = sqrt(l.squaredLength() - tp * tp); // 球心到光线所在直线的距离
        if(d > radius) return false; // 如果 d > r，那么光线与球面不相交
        
        float t_ = sqrt(radius * radius - d * d); // 投影点到光线与球面的交点的距离
        float t;
        Vector3f N; // 相交处法向（指向球外）
        if(l.squaredLength() > radius * radius){ // 光源在球体外部
            t = tp - t_;
            N = R0 + t * Rd - center(time);
        }
        else{ // 光源在球体内部
            t = tp + t_;
            N = R0 + t * Rd - center(time);
        }
        t = t / r.getDirection().length(); // Transform
        if(tmin < t && t < h.getT()){
            h.set(t, material, N);
            return true; // 不管光源在球体内部还是外部，都必定有交点
        }
        else return false;
    }
    
    /*
    bool bounding_box(float _time0, float _time1, aabb& output_box) const {
        aabb box0(
            center(_time0) - Vector3f(radius, radius, radius),
            center(_time0) + Vector3f(radius, radius, radius));
        aabb box1(
            center(_time1) - Vector3f(radius, radius, radius),
            center(_time1) + Vector3f(radius, radius, radius));
        output_box = aabb::surrounding_box(box0, box1);
        return true;
    }
    */

protected:
    float time0, time1;
    Vector3f center0, center1;
};

#endif
