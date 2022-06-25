#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include <limits>

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

// 正交相机
class OrthographicCamera :
	public Camera
{
	float size;

public:
	OrthographicCamera(Vector3f &tcenter, Vector3f &tdirection, Vector3f &tup, int imgW, int imgH, float tsize) 
        : Camera(center, direction, up, imgW, imgH)
    {
        center = tcenter;
        direction = tdirection;
        size = tsize;
        horizontal = tup * tdirection;
        // up = horizontal * tdirection;
        up = tdirection * horizontal;
        up.normalize();
        horizontal.normalize();
        direction.normalize();
    }

    // point vary from (0,0) to (1,1)
	virtual Ray generateRayFromCamera(const Vector2f &point) override {
        // TODO1
        /*
        Vector3f pos = center + size * up * (point.x() - 0.5) + size * horizontal * (point.y() - 0.5);
        return Ray(pos, direction);
        */
        Vector3f start = this->center + this->up * point[0] * this->size / 2.0f + this->horizontal * point[1] * this->size / 2.0f;
	    Ray r = Ray(start, this->direction);
	    return r;
    }

    virtual Ray generateRayFromLight(const Vector2f &point) override {
        // TODO1
        /*
        Vector3f pos = center + size * up * (point.x() - 0.5) + size * horizontal * (point.y() - 0.5);
        return Ray(pos, direction);
        */
        Vector3f start = this->center + this->up * point[0] * this->size / 2.0f + this->horizontal * point[1] * this->size / 2.0f;
	    Ray r = Ray(start, this->direction);
	    return r;
    }


	virtual float getTMin() const override {
        return -std::numeric_limits<float>::max();
    }

	// dollyCamera, truckCamera, and RotateCamera //
    // Asumptions: 
    // - up is really up (i.e., it hasn't been changed 
    // to point to "screen up") 
    // - up and direction are normalized 
    // Special considerations:
    // - If your constructor precomputes any vectors for
    // use in 'generateRay', you will likely to recompute those
    // values at athe end of the these three routines //

    // dollyCamera: Move camera along the direction vector 
	void dollyCamera(float dist) {
        center += direction * dist;
    }

    // truckCamera: Translate camera perpendicular to the direction vector
	void truckCamera(float dx, float dy) {
        Vector3f screenUp;
        screenUp = horizontal * direction;
        center += horizontal * dx + screenUp * dy;
    }

    // rotateCamera: Rotate around the up and horizontal vectors
	void rotateCamera(float rx, float ry) {
        // Don't let the model flip upside-down (There is a singularity 
		// at the poles when 'up' and 'direction' are aligned)
        float tiltAngle = acos(Vector3f::dot(up, direction));
        if (tiltAngle - ry > 3.13) ry = tiltAngle - 3.13;
        else if (tiltAngle - ry < 0.01) ry = tiltAngle - 0.01;
        Matrix4f rotMat = Matrix4f::rotation(up, rx);
        rotMat = rotMat * Matrix4f::rotation(horizontal, ry);
        rotMat.Transform(center);
        rotMat.TransformDirection(direction);

        rotMat.TransformDirection(up);
        up.normalize();
        rotMat.TransformDirection(horizontal);
        horizontal.normalize();
    }
};

#endif //CAMERA_H
