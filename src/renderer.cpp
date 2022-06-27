#include "renderer.hpp"
#include "camera.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "vec_utils.hpp"
#include "group.hpp"
#include "light.hpp"
#include <limits>
#include "Vector3f.h"

#define eps 1e-5

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
    if(!anti_aliasing){
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
                // Hit h = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
                // std::cout << x << ", " << y << std::endl;
                Vector3f color = traceRay(r, cam->getTMin(), 0, 0, 1, h);
                // Vector3f color = traceRay(r, 0);
                image.SetPixel(x, y, color);
            }
        }
        
    }
    
    // anti-aliasing
    else{ 
        
        // Jitter Sampling: High Res
        int widthHighRes = w * 3;
        int heightHighRes = h * 3;
        std::vector<Vector3f> pixColors;
        std::vector<Vector3f> pixColorsBlurH;
        std::vector<Vector3f> pixColorsBlurV;
        const float K[5] = {0.1201f, 0.2339f, 0.2931f, 0.2339f, 0.1201f};
        
        time_t start = time(NULL);
        pixColors.resize(widthHighRes * heightHighRes);
        for (int i = 0; i < widthHighRes; i++) {
            float elapsed = (time(NULL) - start), progress = (1. + i) / widthHighRes;
            fprintf(stderr, "\rRendering %5.2f%% Time: %.2f/%.2f sec", progress * 100., elapsed, elapsed / progress);
#pragma omp parallel for schedule(dynamic, 1)
            for (int j = 0; j < heightHighRes; j++) {
                Vector2f coord = Vector2f((2 * float(i) / (widthHighRes - 1)) - 1, (2 * float(j) / (heightHighRes - 1)) - 1);
                float r_i = (float)rand() / (float)RAND_MAX - 0.5f;
                float r_j = (float)rand() / (float)RAND_MAX - 0.5f;
                Vector2f newCoord = Vector2f(coord[0] + r_i, coord[1] + r_j);
                Ray r = _scene.getCamera()->generateRayFromCamera(coord);
                Hit h = Hit(FLT_MAX, NULL, Vector3f(0));
                Vector3f origColor = traceRay(r, _scene.getCamera()->getTMin(), 0, 0, 1, h);
                pixColors[i * widthHighRes + j] = origColor;
            }
        }

        // Gaussian Filter: High Res horizontal blur
// #pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < widthHighRes; i++) {
            for (int j = 0; j < heightHighRes; j++){
                if (j == 0) {
                    pixColorsBlurH.push_back(pixColors[i * heightHighRes] * K[2] + 
                                             pixColors[i * heightHighRes + 1] * K[3] + 
                                             pixColors[i * heightHighRes + 2] * K[4]); 
                } else if (j == 1) {
                    pixColorsBlurH.push_back(pixColors[i * heightHighRes] * K[1] +
                                             pixColors[i * heightHighRes + 1] * K[2] + 
                                             pixColors[i * heightHighRes + 2] * K[3] + 
                                             pixColors[i * heightHighRes + 3] * K[4]);
                } else if (j == heightHighRes - 1) {
                    pixColorsBlurH.push_back(pixColors[i * heightHighRes + j] * K[2] + 
                                             pixColors[i * heightHighRes + j - 1] * K[1] + 
                                             pixColors[i * heightHighRes + j - 2] * K[0]);   
                } else if (j == heightHighRes - 2) {
                    pixColorsBlurH.push_back(pixColors[i * heightHighRes + j + 1] * K[3] +
                                             pixColors[i * heightHighRes + j] * K[2] + 
                                             pixColors[i * heightHighRes + j - 1] * K[1] + 
                                             pixColors[i * heightHighRes + j - 2] * K[0]);   
                }
                else{
                    pixColorsBlurH.push_back(pixColors[i * heightHighRes + j - 2] * K[0] +
                                             pixColors[i * heightHighRes + j - 1] * K[1] +
                                             pixColors[i * heightHighRes + j] * K[2] + 
                                             pixColors[i * heightHighRes + j + 1] * K[3] + 
                                             pixColors[i * heightHighRes + j + 2] * K[4]);   
                }
            }
        }

        // Gaussian Filter: High Res vertical blur
// #pragma omp parallel for schedule(dynamic, 1)
        for (int j = 0; j < heightHighRes; j++) {
            for (int i = 0; i < widthHighRes; i++) {
                if (i == 0) {
                    pixColorsBlurV.push_back(pixColorsBlurH[j * widthHighRes] * K[2] +
                                             pixColorsBlurH[j * widthHighRes + 1] * K[3] +
                                             pixColorsBlurH[j * widthHighRes + 2] * K[4]);
                }
                else if (i == 1) {
                    pixColorsBlurV.push_back(pixColorsBlurH[j * widthHighRes ] * K[1] +
                                             pixColorsBlurH[j * widthHighRes + 1] * K[2] +
                                             pixColorsBlurH[j * widthHighRes + 2] * K[3] +
                                             pixColorsBlurH[j * widthHighRes + 3] * K[4]);
                } else if (i == widthHighRes - 1) {
                    pixColorsBlurV.push_back(pixColorsBlurH[j * widthHighRes + i] * K[2] +
                                             pixColorsBlurH[j * widthHighRes + i - 1] * K[1] +
                                             pixColorsBlurH[j * widthHighRes + i - 2] * K[0]);
                } else if (i == widthHighRes - 2) {
                    pixColorsBlurV.push_back(pixColorsBlurH[j * widthHighRes + i + 1] * K[3] +
                                             pixColorsBlurH[j * widthHighRes + i] * K[2] +
                                             pixColorsBlurH[j * widthHighRes + i - 1] * K[1] +
                                             pixColorsBlurH[j * widthHighRes + i - 2] * K[0]);
                } else {
                    pixColorsBlurV.push_back(pixColorsBlurH[j * widthHighRes + i - 2] * K[0] +
                                             pixColorsBlurH[j * widthHighRes + i - 1] * K[1] +
                                             pixColorsBlurH[j * widthHighRes + i] * K[2] +
                                             pixColorsBlurH[j * widthHighRes + i + 1] * K[3] +
                                             pixColorsBlurH[j * widthHighRes + i + 2] * K[4]);
                }
            }
        }

        // DownSample
// #pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                Vector3f finalPixCol = pixColorsBlurV[3 * (i + widthHighRes * j) + 0] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + 1] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + 2] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + widthHighRes + 0] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + widthHighRes + 1] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + widthHighRes + 2] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + 2 * widthHighRes + 0] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + 2 * widthHighRes + 1] +
                                       pixColorsBlurV[3 * (i + widthHighRes * j) + 2 * widthHighRes + 2];
                Vector3f finalPixColAVG = finalPixCol / 9.0f;
                image.SetPixel(j, i, finalPixColAVG);
            }
        }
    }
    // END SOLN

    // save the file
    image.SaveImage(output_file);
}

Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming) {
    // Implement mirror reflections for reflective materials (getReflectiveColor() > (0,0,0)) 
    // by sending a ray from the current intersection point in the mirror direction. 
    // return (normal * (2 * (Vector3f::dot(normal, incoming))) - incoming).normalized();
    Vector3f normal_t = normal, incoming_t = incoming;
	normal_t.normalize();
	incoming_t.normalize();
	float t = Vector3f::dot(incoming_t, normal_t);
	return -2 * t * normal_t + incoming_t;
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming, 
                          float index_i, float index_t, Vector3f &transmitted) {
    // Implement transparency effects by sending rays recursively in the refracted direction. 
    // If the material is transparent (getTransparentColor() > (0,0,0)), 
    // trace a new ray in the transmitted direction. 
    // Given an incident vector, a normal and the indices of refraction, 
    // returns the transmitted direction.
    /*
    double cosI = Vector3f::dot(normal, -incoming);
    double nIT = index_i / index_t;
    double cosT2 = 1 - (nIT * nIT) * (1 - cosI * cosI);
    if (cosT2 > eps) 
        transmitted = incoming * nIT + normal * (nIT * cosI - sqrt(cosT2));
    else 
        transmitted = 2 * Vector3f::dot(normal, -incoming) * normal + incoming;
    */
    
    /*
    float n_r = index_i / index_t;
    float NDotD = Vector3f::dot(normal, incoming);
    float radical = (1.0f - n_r * n_r * (1.0f - NDotD * NDotD));
    
    if (radical < 0) return false;
    
    transmitted = normal * (n_r * NDotD - sqrt(radical)) - (incoming * n_r);
    transmitted.normalized();

    return true;
    */

    Vector3f normal_t = normal, incoming_t = incoming;
	normal_t.normalize();
	incoming_t.normalize();
	float t = Vector3f::dot(incoming_t, normal_t);
	Vector3f m = -t*normal_t + incoming_t;
	float insin = m.length();
	float outsin = insin * index_i / index_t;

	if (abs(outsin) > 1)
		return false;

	float outcos = sqrt(1 - outsin * outsin);
	m.normalize();
	if (t < 0)
		transmitted = -outcos * normal_t + outsin * m;
	else
		transmitted = outcos * normal_t + outsin * m;

	return true;
}

Ray getTransmitted(const double t, const Ray &r, const Vector3f& normal, const double refr_iI, const double refr_iT) {
    double cosI = Vector3f::dot(normal, -r.getDirection());
    double nIT = refr_iI / refr_iT;
    double cosT2 = 1 - (nIT * nIT) * (1 - cosI * cosI);
    if (cosT2 > eps){
        Vector3f orig = r.pointAtParameter(t) - normal * 2 * eps;
        Vector3f dir = r.getDirection() * nIT + normal * (nIT * cosI - sqrt(cosT2));
        return Ray(orig, dir);
    }
    else {
        Vector3f orig = r.pointAtParameter(t) + normal * 2 * eps;
        Vector3f dir = 2 * Vector3f::dot(normal, -r.getDirection()) * normal + r.getDirection();
        return Ray(orig, dir);
    }
}

Ray getReflected(const double t, const Ray &r, const Vector3f& normal){
    Vector3f orig = r.pointAtParameter(t) + normal * 2 * eps;
    Vector3f dir = 2 * Vector3f::dot(normal, -r.getDirection()) * normal + r.getDirection();
    return Ray(orig, dir);
}

Vector3f RayTracer::traceRay(const Ray &r, float tmin, int bounces, float weight, 
                             float indexOfRefraction, Hit &h) const {
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.
    // if(bounces <= 0) return _scene.getBackgroundColor(r.getDirection()); // Stop after a number of bounces
    // if(weight < eps) return _scene.getBackgroundColor(r.getDirection()); // Stop if reflected / transmitted contribution becomes too small
    if (bounces > _bounces) return Vector3f::ZERO;
    // TODO: IMPLEMENT
    if (_scene.getGroup()->intersect(r, h, tmin)) {
        assert(NULL != h.getMaterial());
        Vector3f I = _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor(); // add Ambient Light Out the material
        
        // Calculate direct light
        // Diffuse color + shadows
        // Vector3f p = r.pointAtParameter(h.getT());
        // std::cout << "Diffuse color + shadows" << std::endl;
        for (int i = 0; i < _scene.getNumLights(); ++i) {
            Vector3f tolight;
            Vector3f intensity;
            float distToLight;
            _scene.getLight(i)->getIllumination(r.pointAtParameter(h.getT()), tolight, intensity, distToLight);
            // Vector3f ILight = h.getMaterial()->Shade(r, h, tolight, intensity); // bug!
            
            // To compute cast shadows, you will send rays from the surface point to each
            // light source. If an intersection is reported, and the intersection is closer
            // than the distance to the light source, the current surface point is in shadow
            // and direct illumination from that light source is ignored. Note that shadow
            // rays must be sent to all light sources.
            
            Vector3f mask = Vector3f(1.0, 1.0, 1.0);
            // Vector3f shadowRayOrigin = r.pointAtParameter(h.getT()) + eps * tolight;
            Vector3f shadowRayOrigin = r.pointAtParameter(h.getT());
            Ray shadowRay(shadowRayOrigin, tolight);
            // Hit shadowHit = Hit(distToLight, nullptr, NULL);
            Hit shadowHit;
            /*
            if(!_scene.getGroup()->intersect(shadowRay, shadowHit, eps)) {
                Vector3f ILight = h.getMaterial()->Shade(r, h, tolight, intensity); // shading color
                I += ILight;
            }
            */
           
            if(_scene.getGroup()->intersect(shadowRay, shadowHit, eps)) {
                
                float getT = shadowHit.getT(); // bug...
                if (getT < distToLight) continue; // ?

                // useTransparentShadows
                // 不会是只有特定物体可以做这个效果吧（和物体挂钩的？如何处理？）
                // soft shadow my ass
                if(useTransparentShadows) { // huge bug

                    std::cout << "usetrans???" << std::endl;
                    float lastT = -1;
                    Vector3f lastColor = Vector3f(-1, -1, -1);
                    
                    do {
                        getT = shadowHit.getT();
                        if (lastColor == shadowHit.getMaterial()->getTransparentColor())
                        {
                            Vector3f mask_temp = Vector3f(1, 1, 1) * (1 - (getT - lastT)) + shadowHit.getMaterial()->getTransparentColor() * (getT - lastT);
                            mask = mask * mask_temp;
                        }
                        // Hit
                        shadowHit.initialize();
                        lastT = getT;
                        lastColor = shadowHit.getMaterial()->getTransparentColor();
                    } while (_scene.getGroup()->intersect(shadowRay, shadowHit, getT + eps));
                    
                }
                // else if (getT < distToLight) continue;

            }
            I += h.getMaterial()->Shade(r, h, tolight, intensity) * mask;

            /*
            Vector3f shadowTrace = traceRay(shadowRay, 0, 0, weight, indexOfRefraction, shadowHit);
            bool shadowIntersectedSomething = shadowHit.getT() < std::numeric_limits<float>::max();
            float distToIntersection = (shadowRay.pointAtParameter(shadowHit.getT()) - shadowRayOrigin).length();
            if (shadowIntersectedSomething && distToIntersection < distToLight) {
                ILight = Vector3f(0); // Object in shadow from this light, discount light.
            }
            */

            // else {
                // I = Vector3f::ZERO;
                // Vector3f p = r.pointAtParameter(h.getT());
                // Reflections.
                /*
                if (h.getMaterial()->getReflectiveColor().length() > 0 && bounces > 0 && weight >= eps) {
                    // std::cout << "seg of reflect? " << bounces << " " << weight << std::endl;
                    // Implement mirror reflections for reflective materials
                    // by sending a ray from the current intersection point in the mirror direction.

                    // Recursive call.
                    Vector3f V = r.getDirection();
                    Vector3f N = h.getNormal().normalized();
                    Vector3f R = (V - (2 * Vector3f::dot(V, N) * N)).normalized(); // 镜面反射公式R = V – 2 (V · N) N
                    Hit hPrime = Hit();
                    // Add a little epsilon to avoid noise.
                    Ray rPrime(p + eps * R, R);
                    // Trace the secondary ray with a recursive call to traceRay 
                    // using modified values for the recursion depth and ray weight. 
                    // The ray weight is simply multiplied by the magnitude of the reflected color. 
                    // Make sure that traceRay checks the appropriate stopping conditions. 
                    // Add the reflected contribution to the color computed for the current ray. 
                    // Don't forget to take into account the reflection coefficient of the material.
                    if(bounces - 1 == 0) {
                        I += _scene.getBackgroundColor();
                    }
                    else {
                        Vector3f IIndirect = traceRay(rPrime, 0.0f, bounces - 1, weight * h.getMaterial()->getReflectiveColor().length(), indexOfRefraction, hPrime);
                        I += h.getMaterial()->getReflectiveColor() * IIndirect;
                        // I += IIndirect;
                        // I += (h.getMaterial()->getSpecularColor() * IIndirect);
                    }
                }
                */
                
                /*
                if (h.getMaterial()->getTransparentColor().length() > 0 && bounces > 0 && weight >= eps) {
                    // std::cout << "seg of trans?" << bounces << " " << weight << std::endl;
                    if(bounces - 1 == 0) {
                        I += _scene.getBackgroundColor();
                    }
                    else {
                        // Implement transparency effects by sending rays recursively in the refracted direction. 
                        // If the material is transparent, trace a new ray in the transmitted direction. 
                        // given an incident vector, a normal and the indices of refraction, 
                        // returns the transmitted direction.
                        float index_i = 1.0, index_t = 1.0;
                        // Vector3f transmitted;
                        // The dot product of the normal and ray direction is negative when we are outside the object
                        if (Vector3f::dot(h.getNormal(), r.getDirection()) < 0) index_t = h.getMaterial()->getIndexOfRefraction();
                        else index_i = h.getMaterial()->getIndexOfRefraction(); 
                        // transmittedDirection(h.getNormal(), r.getDirection(), index_i, index_t, transmitted);
                        Hit hit_new = Hit();
                        Ray transmitted = getTransmitted(h.getT(), r, h.getNormal(), index_i, index_t);
                        I += h.getMaterial()->getTransparentColor() * traceRay(transmitted, tmin, bounces - 1, weight * h.getMaterial()->getTransparentColor().length(), index_t, hit_new); // bug: h?
                        // I += traceRay(transmitted, tmin, bounces - 1, weight * h.getMaterial()->getTransparentColor().length(), index_t, hit_new);
                        // Hit hit_new = Hit();
                        // I += h.getMaterial()->getTransparentColor() * traceRay(Ray(r.getOrigin(), transmitted), 0.0f, bounces - 1, weight * h.getMaterial()->getTransparentColor().length(), index_t, hit_new);
                    }
                }
                */
            // }
            // I += ILight; // elseif
        }

        // if (bounces > _bounces - 1) return I;

        // Reflectance
        /*
        Vector3f mirrDir = mirrorDirection(h.getNormal().normalized(), -r.getDirection());
        Ray mirrRay = Ray(r.pointAtParameter(h.getT()), mirrDir);
        Hit mirrHit = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
        // Vector3f mirrColor = h.getMaterial()->getSpecularColor() * traceRay(mirrRay, eps, bounces + 1, 1.0, indexOfRefraction, mirrHit); // delete weight?
        Vector3f mirrColor = h.getMaterial()->getReflectiveColor() * traceRay(mirrRay, eps, bounces + 1, 1.0, indexOfRefraction, mirrHit); // delete weight?
        std::cout << "die reflect " << bounces << std::endl;
        */
        if (bounces < _bounces && h.getMaterial()->getReflectiveColor().length() > 0) {
			Ray reflectRay(r.pointAtParameter(h.getT()), mirrorDirection(h.getNormal(), r.getDirection()));
			Hit reflectHit;
			I += traceRay(reflectRay, eps, bounces + 1, weight, indexOfRefraction, reflectHit) * h.getMaterial()->getReflectiveColor();
		}
        
        // Refraction
        /*
        float new_refr_index;
        Vector3f normal = h.getNormal().normalized();
        if (Vector3f::dot(r.getDirection(), normal) > 0){
            new_refr_index = 1.0f;
            normal = -normal;
        }
        else{
            new_refr_index = h.getMaterial()->getIndexOfRefraction();
        }
        std::cout << "die refract " << bounces << std::endl;

        //check if not total reflection
        Vector3f transmitted;
        if (transmittedDirection(normal, -r.getDirection(), indexOfRefraction, new_refr_index, transmitted)){

            Ray transRay = Ray(r.pointAtParameter(h.getT()), transmitted);
            Hit transHit = Hit();

            // Vector3f transColor = h.getMaterial()->getSpecularColor() * traceRay(transRay, eps, bounces + 1, 1.0, new_refr_index, transHit);	
            Vector3f transColor = h.getMaterial()->getTransparentColor() * traceRay(transRay, eps, bounces + 1, 1.0, new_refr_index, transHit);	
            
            float R_0 = pow((new_refr_index - indexOfRefraction) / (new_refr_index + indexOfRefraction), 2);
            
            float c;
            if (indexOfRefraction <= new_refr_index) 
                c = abs(Vector3f::dot(-r.getDirection(), normal));
            else
                c = abs(Vector3f::dot(transmitted, normal));

            float R = R_0 + (1 - R_0) * pow(1 - c, 5);

            I += (1 - R) * transColor + R * mirrColor;
        }
        
        // total reflection
        else I += mirrColor;
        std::cout << "die total reflect " << bounces << std::endl;
        */
        if (bounces < _bounces && h.getMaterial()->getTransparentColor().length() > 0) {
			Hit transHit;
			Vector3f transDir;
			bool inside = Vector3f::dot(h.getNormal(), r.getDirection()) > 0;
			bool check = false;
			if (inside)
				check = transmittedDirection(h.getNormal(), r.getDirection(), indexOfRefraction, 1.0, transDir);
			else
				check = transmittedDirection(h.getNormal(), r.getDirection(), 1.0, h.getMaterial()->getIndexOfRefraction(), transDir);

			Ray transRay(r.pointAtParameter(h.getT()), transDir);
			if (check)
			{
				if (inside)
					I += traceRay(transRay, eps, bounces + 1, weight, 1.0, transHit) * h.getMaterial()->getTransparentColor();
				else
					I += traceRay(transRay, eps, bounces + 1, weight, h.getMaterial()->getIndexOfRefraction(), transHit) * h.getMaterial()->getTransparentColor();

			}
		}
        return I;
    }  
    // 都没交点
    return _scene.getBackgroundColor(r.getDirection());
}

bool RayTracer::lightIntersect(const Ray& r, Hit& h, double tmin) const {
	bool isIntersect = false;
	for (int li = 0; li < _scene.getNumLights(); ++li) {
        Light* light = _scene.getLight(li);
		if (light->intersect(r, h, tmin))
			isIntersect = true;
	}
	return isIntersect;
}

Vector3f RayTracer::calReflection(const Ray& r, Hit& h, int depth) const {
	if (depth > _bounces) return _scene.getBackgroundColor();
	Ray reflected = getReflected(h.getT(), r, h.getNormal());
	return traceRay(reflected, depth);
}

Vector3f RayTracer::calRefraction(const Ray& r, Hit& h, int depth) const {
	if (depth > _bounces) return _scene.getBackgroundColor();

    float index_i = 1.0, index_t = 1.0;
    // Vector3f transmitted;
    // The dot product of the normal and ray direction is negative when we are outside the object
    if (Vector3f::dot(h.getNormal(), r.getDirection()) < 0) index_t = h.getMaterial()->getIndexOfRefraction();
    else index_i = h.getMaterial()->getIndexOfRefraction(); 
    Ray transmitted = getTransmitted(h.getT(), r, h.getNormal(), index_i, index_t);
	return traceRay(transmitted, depth);
}

Vector3f RayTracer::calDiffusion(const Ray& r, Hit& h) const {
    Vector3f hit_point = r.pointAtParameter(h.getT()), color = Vector3f::ZERO;
	Material *material = h.getMaterial();
	for (int li = 0; li < _scene.getNumLights(); ++li) {
        Light* light = _scene.getLight(li);
        Vector3f dirToLight, lightColor;
		light->getIllumination(_scene.getGroup(), hit_point, h.getNormal(), dirToLight, lightColor);
		color += std::max(Vector3f::dot(dirToLight, h.getNormal()), 0.0f) * material->getDiffuseColor() * lightColor ;
		Vector3f R = 2 * Vector3f::dot(h.getNormal(), dirToLight) * h.getNormal() - dirToLight;
		color += pow(std::max(Vector3f::dot(-r.getDirection(), R), 0.0f), material->getShininess()) * lightColor;
	}
	return color;
}


Vector3f RayTracer::traceRay(const Ray &r, int depth) const {
	Hit hit, light_hit;
	Vector3f color = Vector3f::ZERO;
	bool isIntersect = _scene.getGroup()->intersect(r, hit, eps);
	bool isLightIntersect = lightIntersect(r, light_hit, eps); // only directional + pointlight, always false
	// bool isLightIntersect = false;
    if (!isIntersect && !isLightIntersect){ // 都没交点
		color += _scene.getBackgroundColor();
	}
	else if(!isIntersect || (isIntersect && isLightIntersect && hit.getT() >= light_hit.getT() - eps)){
		// color += light_hit.getColor();
        std::cout << "never come in" << std::endl;
	}
	else{ // 
		Material *material = hit.getMaterial();
		if(material->getSpecularColor().length() > eps)
			color += calReflection(r, hit, depth + 1) * material->getSpecularColor();
		if(material->getDiffuseColor().length() > eps)
			color += calDiffusion(r, hit) * material->getDiffuseColor();
		if(material->getTransparentColor().length() > eps)
			color += calRefraction(r, hit, depth + 1) * material->getTransparentColor();
	}
	return color;
}
