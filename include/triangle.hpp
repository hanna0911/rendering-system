#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vecmath.h>
#include <cmath>
#include <iostream>
#include "object3d.hpp"
#include "aabb.hpp"

using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->vertices[0] = a; this->vertices[1] = b; this->vertices[2] = c;
		Vector3f P1P2 = b - a, P1P3 = c - a;
		this->normal = Vector3f::cross(P1P2, P1P3);
		useVN = false;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f P0 = vertices[0], P1 = vertices[1], P2 = vertices[2]; // 三角形顶点
		Vector3f R0 = ray.getOrigin(); // 光线起始点
        Vector3f Rd = ray.getDirection(); Rd.normalize(); // 光线单位方向向量
		Vector3f E1 = P0 - P1, E2 = P0 - P2, S = P0 - R0;
        Matrix3f denominator(Rd, E1, E2, true);
		Matrix3f t_(S, E1, E2, true);
		Matrix3f b_(Rd, S, E2, true);
		Matrix3f r_(Rd, E1, S, true);
		float det_denominator = denominator.determinant();
		float t = t_.determinant() / det_denominator, b = b_.determinant() / det_denominator, r = r_.determinant() / det_denominator;
		t = t / ray.getDirection().length(); // Transform
		
		if (t > 0 && 0 <= b && b <= 1 && 0 <= r && r <= 1 && b + r <= 1 && tmin < t && t < hit.getT()) {
			Vector3f n = normal;
			if (useVN) {
				Vector3f point = ray.pointAtParameter(t);
				Vector3f va = (vertices[0] - point), vb = (vertices[1] - point), vc = (vertices[2] - point);
				float ra = Vector3f::cross(vb, vc).length(),
					rb = Vector3f::cross(vc, va).length(),
					rc = Vector3f::cross(va, vb).length();
				n = (ra * vn[0] + rb * vn[1] + rc * vn[2]).normalized();
			}

			hit.set(t, material, n); // 是否要判断normal的朝向？
			// std::cout << "intersect tri" << std::endl;
			return true;
		}
		// std::cout << "no intersect tri" << std::endl;
		return false;
	}
	
	// 何苦给三角形做包围盒呢？怀疑人生ing 不能直接拿三角形作为对象建树吗，还拿AABB过渡一下？？？
	bool bounding_box(float time0, float time1, aabb& output_box) const {
        // 计算包围盒的范围
		Vector3f minimum(std::numeric_limits<float>::max()),
				 maximum(std::numeric_limits<float>::min());
		for (int i = 0; i < 3; i++) { // 3 points
			for (int j = 0; j < 3; j++) { // x, y, z
				if (vertices[i][j] < minimum[j]) minimum[j] = vertices[i][j];
				if (vertices[i][j] > maximum[j]) maximum[j] = vertices[i][j];
			}
		}
		// bug: 1. not int, float  2. min, max only have 2 parameters
		/*
		int xmin = std::min(vertices[0].x(), vertices[1].x(), vertices[2].x());
		int xmax = std::max(vertices[0].x(), vertices[1].x(), vertices[2].x());
		int ymin = std::min(vertices[0].y(), vertices[1].y(), vertices[2].y());
		int ymax = std::max(vertices[0].y(), vertices[1].y(), vertices[2].y());
		int zmin = std::min(vertices[0].z(), vertices[1].z(), vertices[2].z());
		int zmax = std::max(vertices[0].z(), vertices[1].z(), vertices[2].z());
		output_box = aabb(Vector3f(xmin, ymin, zmin), Vector3f(xmax, ymax, zmax));
		*/
        output_box = aabb(minimum, maximum);
		return true;
    }
		
	void setVN(const Vector3f& an, const Vector3f& bn, const Vector3f& cn) {
		useVN = true;
		vn[0] = an; vn[1] = bn; vn[2] = cn;
		// vn[0] = an.normalized(); vn[1] = bn.normalized(); vn[2] = cn.normalized();
	}

	Vector3f normal;
	Vector3f vertices[3];
	Vector3f vn[3];
	bool useVN;

protected:
};

#endif //TRIANGLE_H
