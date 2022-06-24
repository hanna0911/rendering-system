#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include <limits>
#include "object3d.hpp"

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distToLight) const = 0; // 增加到光距离distToLight
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
        col = color / (pow(distToLight, 2) * falloff); // 避免物理模型过于陡峭
    }

private:

    Vector3f position;
    Vector3f color;
    float falloff; // 平方反比衰减常数
};

#endif // LIGHT_H
