#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#include "scene_parser.hpp"
#include "ray.hpp"
#include "hit.hpp"

// class Hit;
// class Vector3f;
// class Ray;

class Renderer {
public:
    // Instantiates a renderer for the given scene.
    Renderer(char *input_file, char *output_file) :
        _scene(input_file), 
        output_file(output_file) {}
    
    virtual void Render() = 0;
  
    // rendering output
    char *output_file;
    SceneParser _scene;
};

class RayCaster : public Renderer {
public:
    RayCaster(char *input_file, char *output_file) : 
        Renderer(input_file, output_file) {}
    void Render();
};

class RayTracer : public Renderer {
public:
    RayTracer(char *input_file, char *output_file, int bounces, bool transShad, bool antiAlias) : 
        Renderer(input_file, output_file), 
        _bounces(bounces),
        useTransparentShadows(transShad),
        anti_aliasing(antiAlias) {}
    void Render();

private:
    Vector3f traceRay(const Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const;
    Vector3f traceRay(const Ray &ray, int depth) const;
    
    Vector3f calReflection(const Ray& r, Hit& h, int depth) const;
    Vector3f calRefraction(const Ray& r, Hit& h, int depth) const;
    Vector3f calDiffusion(const Ray& r, Hit& h) const;
    bool lightIntersect(const Ray& r, Hit& h, double tmin) const;
    // rendering options
    int _bounces;
    bool useTransparentShadows;
    bool anti_aliasing;
};

#endif // RENDERER_H
