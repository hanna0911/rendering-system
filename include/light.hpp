#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include <limits>
#include "object3d.hpp"

#define eps 1e-8

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distToLight) const = 0; // 增加到光距离distToLight
    virtual void getIllumination(Object3D* group, const Vector3f &p, const Vector3f &n,
                                 Vector3f &dir, Vector3f &col) const = 0; // for shading
    virtual bool intersect(const Ray &r, Hit &h, double tmin) = 0;  // 增加是否与光源相交，如果相交则返回相交的射线r和打到光上的h信息
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distToLight) const override { // 增加到光距离distToLight
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
        distToLight = std::numeric_limits<float>::max(); // 非点光源（平行光）：无限远
    }

    // For shading and hashing
    void getIllumination(Object3D* group, const Vector3f &p, const Vector3f &n, 
                         Vector3f &dir, Vector3f &col) const override {
        dir = -direction;
        Ray light_ray(p + 2 * eps * n, dir);
		Hit light_hit;
        bool isIntersect = group->intersect(light_ray, light_hit, eps);
        col = isIntersect ? Vector3f::ZERO : color;
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        return false;
    }
private:

    Vector3f direction;
    Vector3f color;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3f &p, const Vector3f &c, float f) {
        position = p;
        color = c;
        falloff = f;
    }

    ~PointLight() override = default;

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distToLight) const override { // 增加到光距离distToLight
        // the direction to the light is the opposite of the
        // direction of the directional light source
        /*
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
        */
        dir = (position - p).normalized();
        distToLight = (position - p).length();
        col = color / (pow(distToLight, 2) * falloff); // 避免物理模型过于陡峭 分母是否需要+1?
    }

    // For shading and hashing
    void getIllumination(Object3D* group, const Vector3f &p, const Vector3f &n, 
                         Vector3f &dir, Vector3f &col) const override {
        dir = (position - p).normalized();
        Ray light_ray(p + 2 * eps * n, dir);
		Hit light_hit;
        bool isIntersect = group->intersect(light_ray, light_hit, eps);
        if (!isIntersect || light_hit.getT() >= (position - p).length())
            col = color;
        else{
            col = Vector3f::ZERO;
        }
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        return false;
    }

private:

    Vector3f position;
    Vector3f color;
    float falloff; // 平方反比衰减常数
};

#endif // LIGHT_H
