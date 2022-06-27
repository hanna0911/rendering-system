#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {
        this->capacity = 0;
    }

    explicit Group (int num_objects) {
        this->capacity = num_objects;
    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool True = false;
        for(int i = 0; i < capacity; i++){
            if(data[i]->intersect(r, h, tmin)){
                True = true;
                if(!h.getMaterial()) std::cout << "null in group" << std::endl;
                // break;
            }
        }
        if(True) return true;
        return false;
    }

    void addObject(int index, Object3D *obj) {
        data.push_back(obj);
    }

    int getGroupSize() {
        return capacity;
    }

    // Creating Bounding Boxes of Lists of Objects
    /*
    bool bounding_box(float time0, float time1, aabb& output_box) const {
        if (data.empty()) return false;

        aabb temp_box;
        bool first_box = true;

        for (const auto& object : data) {
            if (!object->bounding_box(time0, time1, temp_box)) return false;
            output_box = first_box ? temp_box : aabb::surrounding_box(output_box, temp_box);
            first_box = false;
        }

        return true;
    }
    */

private:
    std::vector<Object3D*> data;
    int capacity;
};

#endif
	
