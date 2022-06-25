#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <cmath>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, 
                      const Vector3f &s_color = Vector3f::ZERO, 
                      float s = 0,
                      const Vector3f &r_Color = Vector3f::ZERO, 
                      const Vector3f &t_Color = Vector3f::ZERO, 
                      float ior = 1) :
            diffuseColor(d_color), 
            specularColor(s_color), 
            shininess(s),
            reflectiveColor(r_Color),
            transparentColor(t_Color),
            indexOfRefraction(ior) {}

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }

    virtual Vector3f getSpecularColor() const {
        return specularColor;
    }

    virtual Vector3f getReflectiveColor() const {
        return reflectiveColor;
    }

    virtual Vector3f getTransparentColor() const {
        return transparentColor;
    }

    virtual float getIndexOfRefraction() const {
        return indexOfRefraction;
    }

    virtual float getShininess() const {
        return shininess;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        // 
        
        Vector3f N = hit.getNormal(); N.normalize(); // bug solved: 单位化!
        Vector3f L = dirToLight; L.normalize();
        Vector3f V = -ray.getDirection(); V.normalize();
        Vector3f R = 2 * N.dot(N, L) * N - L; R.normalize();
        float diffuse = L.dot(L, N), specular = V.dot(V, R);
        if(diffuse < 0) diffuse = 0;
        if(specular < 0) specular = 0;
        else specular = pow(specular, shininess);
        shaded = lightColor * (diffuseColor * diffuse + specularColor * specular);
        return shaded;
        /*
        Vector3f L = dirToLight.normalized();
        Vector3f N = hit.getNormal().normalized();

        // Diffuse term
        float dotProd = Vector3f::dot(L, N);
        float diffuseShading = dotProd > 0 ? dotProd : 0;
        Vector3f IDiffuse = diffuseShading * lightColor * diffuseColor;

        // Specular term
        Vector3f E = -ray.getDirection().normalized();
        // See L13 slides for perfect reflection vector formula.
        Vector3f R = (-E + (2 * (Vector3f::dot(E, N)) * N)).normalized();
        dotProd = Vector3f::dot(L, R);
        float specularShading = dotProd > 0 ? dotProd : 0;
        Vector3f ISpecular = pow(specularShading, shininess) * lightColor * specularColor;
        return IDiffuse + ISpecular;
        */
    }

protected:
    Vector3f diffuseColor; // 漫反射（固有色）
    Vector3f specularColor; // 高光颜色
    float shininess; // 镜面反射exponent系数
    Vector3f reflectiveColor; // 反射度
    Vector3f transparentColor; // 透明度
    float indexOfRefraction; // 折射率
};


#endif // MATERIAL_H
