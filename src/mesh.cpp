#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {

    // Optional: Change this brute force method into a faster one.
    /*
    bool result = false;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle triangle(v[triIndex[0]],
                          v[triIndex[1]], v[triIndex[2]], material);
        triangle.normal = n[triId];
        result |= triangle.intersect(r, h, tmin);
    }
    return result;
    */
    
    float t;
    bool boxhit = box.intersect(r, t, tmin);
    // if(boxhit) std::cout << "box intersected" << std::endl;
    if(!boxhit || t > h.getT()) return false;

    /*
    bool result = false;
    for (int i = 0; i < faces.size(); i++) {
        result |= faces[i]->intersect(r, h, tmin);
    }
    return result;
    */

    
    // change into BVH
    // still bug (in transparent objects)
    // std::cout << "start tree intersect" << std::endl;
    bool treehit = bvhtree->intersect(r, h, tmin);
    // if(treehit) std::cout << "tree intersected" << std::endl;
    // else std::cout << "tree no intersected" << std::endl;
    return treehit; // tmin?
    
    
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v"); // Vertices
    std::string fTok("f"); // Faces: 1000
    std::string texTok("vt"); // ?? didn't save, texture?
    std::string normalTok("vn"); // TODO: normal
    char bslash = '/', space = ' ';
    std::string tok;
    int texID; // ?? no use
    
    box_minimum =  std::numeric_limits<float>::max();
    box_maximum = std::numeric_limits<float>::min();
    bool input_vn = false; // 是否输入vn这一量

    while (true) {
        if(input_vn) std::cout << "input_vn" << std::endl;
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) { // "v" -> v
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            // 更新aabb 大bug!!!
            for (int j = 0; j < 3; j++) { // x, y, z
                if (vec[j] < box_minimum[j]) box_minimum[j] = vec[j];
                if (vec[j] > box_maximum[j]) box_maximum[j] = vec[j];
            }
            v.push_back(vec);
        } else if (tok == fTok) { // "f" -> t
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig, norm; // if input normal
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                    if(input_vn) {
                        facess >> norm[ii];
                        norm[ii]--;
                    }
                }
                t.push_back(trig);
                if(input_vn) vnIndex.push_back(norm);
            } else { 
                TriangleIndex trig, norm;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                    if(input_vn) {
                        ss >> norm[ii];
                        norm[ii]--;
                    }
                }
                t.push_back(trig);
                if(input_vn) vnIndex.push_back(norm);
            }
        } else if (tok == texTok) { // "vt" (unused)
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        } else if (tok == normalTok) { // "vn" -> n
            input_vn = true;
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            vn.push_back(vec);
        }
    }
    if(!input_vn) computeNormal(); // 如果没有输入normal，则计算normal

    f.close();

    // BVH
    
    // shift the center to (0, 0, 0)
    /*
    Vector3f center = 0.5 * (box_minimum + box_maximum); // 中点
    box_maximum -= center; 
    box_minimum -= center;
    for(int i = 0; i < v.size(); i++)
        v[i] -= center;
    */
    // set aabb
    box.set(box_minimum, box_maximum);    

    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        TriangleIndex& normalIndex = vnIndex[triId];
        Triangle *triangle = new Triangle(v[triIndex[0]], 
            v[triIndex[1]], v[triIndex[2]], material);
        if (input_vn) triangle->setVN(vn[normalIndex[0]], 
            vn[normalIndex[1]], vn[normalIndex[2]]);
        else triangle->normal = n[triId];
        faces.push_back(triangle);
    }
    
    // build tree
    bvhtree = new bvh_node(faces, 0, faces.size(), 0, 0);

}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
