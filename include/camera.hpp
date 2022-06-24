#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = up;
        // this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRayFromLight(const Vector2f &point) = 0; // Ray Casting
    virtual Ray generateRayFromCamera(const Vector2f &point) = 0; // Ray Tracing
    virtual float getTMin() const = 0; // 获取Tmin
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center; // 视线原点
    Vector3f direction; // 视点朝向
    Vector3f up; // 画布竖直轴
    Vector3f horizontal; // 画布水平轴
    // Intrinsic parameters
    int width; // 画布大小
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        this->angle = angle;
    }

    Ray generateRayFromLight(const Vector2f &point) override { // Ray Casting
        // point为(u, v)
        float fx = height / (2 * tan(angle / 2)), fy = height / (2 * tan(angle / 2));
        float u = point.x(), v = point.y();
        float cx = width / 2, cy = height / 2;
        Vector3f dRc((u - cx) / fx, (cy - v) / fy, 1);
        dRc.normalize();
        Matrix3f R(horizontal, -up, direction, true);
        Vector3f dRw = R * dRc;
        Ray ray(center, dRw);
        return ray;
    }

    Ray generateRayFromCamera(const Vector2f &point) override { // Ray Tracing
        float d = 1.0f / (float)std::tan(angle / 2.0f);
        Vector3f newDir = d * direction + point[0] * horizontal + point[1] * up;
        newDir = newDir.normalized();
        return Ray(center, newDir); 
    }

    virtual float getTMin() const override { // 获取Tmin
        return 0.0f;
    }

protected:
    float angle;
};

#endif //CAMERA_H
