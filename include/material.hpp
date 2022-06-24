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

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }

    virtual Vector3f getSpecularColor() const {
        return specularColor;
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
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
};


#endif // MATERIAL_H
