#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <memory>
// #include "object3d.hpp"
#include "triangle.hpp"
#include "group.hpp"

#define eps 1e-5

// previously used for all Object3Ds (including groups)
// now changed to only for Triangle (mesh faces)
class bvh_node {
public:
    bvh_node() {}

    // bvh_node(const Group& list, float time0, float time1)
    //     : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}

    // bvh_node(const std::vector<std::shared_ptr<Object3D>> &src_objects,
    bvh_node(const std::vector<Triangle *> &src_objects,
             size_t start, size_t end, float time0, float time1) {
        
        
        auto objects = src_objects; // Create a modifiable array of the source scene objects

        // randomly choose an axis
        int axis = std::rand() % 3; // Returns a random integer in [0, 2].
        
        size_t object_span = end - start;
        
        /*
        if (object_span == 1) {
            face = objects[start];
            objects[start]->bounding_box(0, 0, box);
            left = nullptr;
            right = nullptr;
            return;
        }
        else if (object_span == 2) {
            left = make_shared<bvh_node>();
            right = make_shared<bvh_node>();
            left->face = objects[start];
            objects[start]->bounding_box(0, 0, left->box);
            right->face = objects[start + 1];
            objects[start + 1]->bounding_box(0, 0, right->box);
            left->left = nullptr;
            left->right = nullptr;
            right->left = nullptr;
            right->right = nullptr;
            return;
        }
        */

        // bug...
        /*
        auto comparator = (axis == 0) ? box_x_compare 
                        : (axis == 1) ? box_y_compare
                                    : box_z_compare;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                left = objects[start + 1];
                right = objects[start];
            }
        } else {
            // sort the primitives (using std::sort)
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            // put half in each subtree
            auto mid = start + object_span / 2;
            // left = make_shared<bvh_node>(objects, start, mid, time0, time1);
            // right = make_shared<bvh_node>(objects, mid, end, time0, time1);
            left = new bvh_node(objects, start, mid, time0, time1); // when to delete?
            right = new bvh_node(objects, mid, end, time0, time1);
        }
        */
        if (axis == 0) {
            // std::cout << "axis " << axis << std::endl;
            // std::cout << "object_span " << object_span << std::endl;
            if (object_span == 1) {
                // if(!left) std::cout << "nullptr" << std::endl;
                left = make_shared<bvh_node>();
                right = make_shared<bvh_node>();
                left->face = objects[start];
                right->face = objects[start];
                objects[start]->bounding_box(0, 0, left->box);
                objects[start]->bounding_box(0, 0, right->box);
            } else if (object_span == 2) {
                if (box_x_compare(objects[start], objects[start + 1])) {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start];
                    right->face = objects[start + 1];
                    objects[start]->bounding_box(0, 0, left->box);
                    objects[start + 1]->bounding_box(0, 0, right->box);
                } else {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start + 1];
                    right->face = objects[start];
                    objects[start + 1]->bounding_box(0, 0, left->box);
                    objects[start]->bounding_box(0, 0, right->box);
                }
            } else {
                // sort the primitives (using std::sort)
                std::sort(objects.begin() + start, objects.begin() + end, box_x_compare);

                // put half in each subtree
                auto mid = start + object_span / 2;
                left = make_shared<bvh_node>(objects, start, mid, time0, time1);
                right = make_shared<bvh_node>(objects, mid, end, time0, time1);
                // left = bvh_node(objects, start, mid, time0, time1); // when to delete? how to save face?
                // right = bvh_node(objects, mid, end, time0, time1);
            }
        } else if (axis == 1) {
            // std::cout << "axis " << axis << std::endl;
            // std::cout << "object_span " << object_span << std::endl;
            if (object_span == 1) {
                // if(!left) std::cout << "nullptr" << std::endl;
                left = make_shared<bvh_node>();
                right = make_shared<bvh_node>();
                left->face = objects[start];
                right->face = objects[start];
                objects[start]->bounding_box(0, 0, left->box);
                objects[start]->bounding_box(0, 0, right->box);
            } else if (object_span == 2) {
                if (box_y_compare(objects[start], objects[start + 1])) {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start];
                    right->face = objects[start + 1];
                    objects[start]->bounding_box(0, 0, left->box);
                    objects[start + 1]->bounding_box(0, 0, right->box);
                } else {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start + 1];
                    right->face = objects[start];
                    objects[start + 1]->bounding_box(0, 0, left->box);
                    objects[start]->bounding_box(0, 0, right->box);
                }
            } else {
                // sort the primitives (using std::sort)
                std::sort(objects.begin() + start, objects.begin() + end, box_y_compare);

                // put half in each subtree
                auto mid = start + object_span / 2;
                left = make_shared<bvh_node>(objects, start, mid, time0, time1);
                right = make_shared<bvh_node>(objects, mid, end, time0, time1);
                // left = bvh_node(objects, start, mid, time0, time1); // when to delete? how to save face?
                // right = bvh_node(objects, mid, end, time0, time1);
            }
        } else {
            // std::cout << "axis " << axis << std::endl;
            // std::cout << "object_span " << object_span << std::endl;
            if (object_span == 1) {
                // if(!left) std::cout << "nullptr" << std::endl;
                left = make_shared<bvh_node>();
                right = make_shared<bvh_node>();
                left->face = objects[start];
                right->face = objects[start];
                objects[start]->bounding_box(0, 0, left->box);
                objects[start]->bounding_box(0, 0, right->box);
            } else if (object_span == 2) {
                if (box_z_compare(objects[start], objects[start + 1])) {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start];
                    right->face = objects[start + 1];
                    objects[start]->bounding_box(0, 0, left->box);
                    objects[start + 1]->bounding_box(0, 0, right->box);
                } else {
                    left = make_shared<bvh_node>();
                    right = make_shared<bvh_node>();
                    left->face = objects[start + 1];
                    right->face = objects[start];
                    objects[start + 1]->bounding_box(0, 0, left->box);
                    objects[start]->bounding_box(0, 0, right->box);
                }
            } else {
                // sort the primitives (using std::sort)
                std::sort(objects.begin() + start, objects.begin() + end, box_z_compare);

                // put half in each subtree
                auto mid = start + object_span / 2;
                left = make_shared<bvh_node>(objects, start, mid, time0, time1);
                right = make_shared<bvh_node>(objects, mid, end, time0, time1);
                // left = bvh_node(objects, start, mid, time0, time1); // when to delete? how to save face?
                // right = bvh_node(objects, mid, end, time0, time1);
            }
        }

        // std::cout << "checkpoint" << std::endl;

        aabb box_left, box_right;

        if (!left->bounding_box(time0, time1, box_left)
         || !right->bounding_box(time0, time1, box_right))
            std::cerr << "No bounding box in bvh_node constructor.\n";

        box = aabb::surrounding_box(box_left, box_right);
    }


    virtual bool intersect(const Ray& r, Hit& rec, float tmin) const {
        // if(!left && !right) return true;
        
        if(!left && !right) { // 到底了，和真实的物体做交
            // std::cout << "leaf" << std::endl;
            // std::cout << face->material->getDiffuseColor() << std::endl;
            bool hitobject = face->intersect(r, rec, eps); // !!!!eps
            // if(hitobject) std::cout << "hit object" << std::endl;
            return hitobject;
        }

        float t;
        // 暂时注释掉
        bool boxhit = box.intersect(r, t, tmin);
        // if (boxhit) std::cout << "box hit once in tree" << std::endl;
        if (!boxhit) return false;


        bool hit_left = false, hit_right = false;
        if(left) hit_left = left->intersect(r, rec, tmin);
        if(right) hit_right = right->intersect(r, rec, tmin);
        
        bool treehit = hit_left || hit_right;
        // if(treehit) std::cout << "treehit yes" << std::endl;
        // else std::cout << "treehit no" << std::endl;
        return treehit;
    }

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const {
        output_box = box;
        return true;
    }

    // inline bool box_compare(const shared_ptr<Object3D> a, const shared_ptr<Object3D> b, int axis) {
    static inline bool box_compare(const Triangle *a, const Triangle *b, int axis) {
        aabb box_a;
        aabb box_b;

        if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
            std::cerr << "No bounding box in bvh_node constructor.\n";

        return box_a.min()[axis] < box_b.min()[axis];
    }


    // bool box_x_compare (const shared_ptr<Object3D> a, const shared_ptr<Object3D> b) {
    static bool box_x_compare (const Triangle *a, const Triangle *b) {
        return box_compare(a, b, 0);
    }

    // bool box_y_compare (const shared_ptr<Object3D> a, const shared_ptr<Object3D> b) {
    static bool box_y_compare (const Triangle *a, const Triangle *b) {
        return box_compare(a, b, 1);
    }

    // bool box_z_compare (const shared_ptr<Object3D> a, const shared_ptr<Object3D> b) {
    static bool box_z_compare (const Triangle *a, const Triangle *b) {
        return box_compare(a, b, 2);
    }

// protected:
    // shared_ptr<Object3D> left;
    // shared_ptr<Object3D> right;
    // bvh_node *left, *right; // 令人头大，因为教材里bvh_node也继承了Object3D...
    shared_ptr<bvh_node> left, right; 
    Triangle *face; // 存储真正的"Object3D"...
    // vector<Triangle*> face; // ?
    aabb box;
};

#endif