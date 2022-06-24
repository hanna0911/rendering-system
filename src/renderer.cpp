#include "renderer.hpp"

#include "camera.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "vec_utils.hpp"
#include "group.hpp"
#include "light.hpp"

#include <limits>

void RayCaster::Render() {
    Camera* camera = _scene.getCamera();
    Image renderedImg(camera->getWidth(), camera->getHeight());
    // 循环屏幕空间的像素
    time_t start = time(NULL);
#pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < camera->getWidth(); ++x) {
        float elapsed = (time(NULL) - start), progress = (1. + x) / camera->getWidth();
        fprintf(stderr, "\rRendering %5.2f%% Time: %.2f/%.2f sec", progress * 100., elapsed, elapsed / progress);
        for (int y = 0; y < camera->getHeight(); ++y) {
            // 计算当前像素(x,y)处相机出射光线camRay
            Ray camRay = camera->generateRayFromLight(Vector2f(x, y)); 
            Group* baseGroup = _scene.getGroup();
            Hit hit;
            // 判断camRay是否和场景有交点，并返回最近交点的数据，存储在hit中
            bool isIntersect = baseGroup->intersect(camRay, hit, 0);
            if (isIntersect) {
                Vector3f finalColor = Vector3f::ZERO;
                // 找到交点之后，累加来自所有光源的光强影响
                for (int li = 0; li < _scene.getNumLights(); ++li) {
                    Light* light = _scene.getLight(li);
                    Vector3f L, lightColor;
                    float distToLight;
                    // 获得光照强度
                    light->getIllumination(camRay.pointAtParameter(hit.getT()), L, lightColor, distToLight); // 计算局部光强
                    finalColor += hit.getMaterial()->Shade(camRay, hit, L, lightColor);
                }
                renderedImg.SetPixel(x, y, finalColor);
            } else {
            // 不存在交点，返回背景色
                renderedImg.SetPixel(x, y, _scene.getBackgroundColor());
            }
        }
    }
    renderedImg.SaveImage(output_file);
}

void RayTracer::Render() {
    Camera *cam = _scene.getCamera();
    int w = cam->getWidth();
    int h = cam->getHeight();

    Image image(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    time_t start = time(NULL);
#pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < h; ++y) {
        float elapsed = (time(NULL) - start), progress = (1. + y) / h;
        fprintf(stderr, "\rRendering %5.2f%% Time: %.2f/%.2f sec", progress * 100., elapsed, elapsed / progress);
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRayFromCamera(Vector2f(ndcx, ndcy));
            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _bounces, h);
            image.SetPixel(x, y, color);
        }
    }
    // END SOLN

    // save the files
    // image.savePNG(output_file);
    image.SaveImage(output_file);
}

Vector3f RayTracer::traceRay(const Ray &r, float tmin, int bounces, Hit &h) const {
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT
    if (_scene.getGroup()->intersect(r, h, tmin)) {
        Vector3f I = _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
        Vector3f p = r.pointAtParameter(h.getT());
        for (int i = 0; i < _scene.getNumLights(); ++i) {
            Vector3f tolight;
            Vector3f intensity;
            float distToLight;
            _scene.getLight(i)->getIllumination(p, tolight, intensity, distToLight);
            Vector3f ILight = h.getMaterial()->Shade(r, h, tolight, intensity); // bug!
            // To compute cast shadows, you will send rays from the surface point to each
            // light source. If an intersection is reported, and the intersection is closer
            // than the distance to the light source, the current surface point is in shadow
            // and direct illumination from that light source is ignored. Note that shadow
            // rays must be sent to all light sources.
            
            // 阴影shadows
            Vector3f shadowRayOrigin = p + 0.05 * tolight;
            Ray shadowRay(shadowRayOrigin, tolight);
            Hit shadowHit = Hit();
            Vector3f shadowTrace = traceRay(shadowRay, 0, 0, shadowHit);
            bool shadowIntersectedSomething = shadowHit.getT() < std::numeric_limits<float>::max();
            float distToIntersection = (shadowRay.pointAtParameter(shadowHit.getT()) - shadowRayOrigin).length();
            if (
                shadowIntersectedSomething && distToIntersection < distToLight
            ) {
                ILight = Vector3f(0); // Object in shadow from this light, discount light.
            }
            I += ILight;
        }
        // Reflections.
        if (bounces > 0) {
            // Recursive call.
            Vector3f V = r.getDirection();
            Vector3f N = h.getNormal().normalized();
            Vector3f R = (V - (2 * Vector3f::dot(V, N) * N)).normalized();
            Hit hPrime = Hit();
            // Add a little epsilon to avoid noise.
            Ray rPrime(p + 0.01 * R, R);
            Vector3f IIndirect = traceRay(rPrime, 0.0f, bounces - 1, hPrime);
            I += (h.getMaterial()->getSpecularColor() * IIndirect);
        }
        return I;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    };
}

