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

private:
    std::vector<Object3D*> data;
    int capacity;
};

#endif
	
