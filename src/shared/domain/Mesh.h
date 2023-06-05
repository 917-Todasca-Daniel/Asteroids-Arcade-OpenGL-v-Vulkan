#pragma once

#include "domain/Object3d.h"
#include "data/Quaternion.hpp"


namespace aa
{


    // loads and draws 3d objects
    // no rotation is applied by default and position is additive to vertex positions
    class Mesh : public Object3d
    {

    public:
        Mesh(
            LogicObject* parent,
            Vector3d     position
        );
        virtual ~Mesh();

        Quaternion& getRotation() {
            return this->rotation;
        }

        void setRotation(Quaternion rot) {
            this->rotation = rot;
        }

        Vector3d getCenter() const {
            return center;
        }
        
        // O(1) complexity, gets maximum value for vetices coordinates
        float getRadius() const {
            return radius;
        }


    protected:
        Quaternion              rotation;

        // contains vertex position, vertex normals, tex coord (for reflecting light)
        std::vector <float>     vertices;
        std::vector <uint32_t>  indices;

        float                   radius;

        // by default, the mean average of all points
        Vector3d                center;

        void loadFromFbx(const char* filepath, unsigned int pFlags, float scale);

    };
}
