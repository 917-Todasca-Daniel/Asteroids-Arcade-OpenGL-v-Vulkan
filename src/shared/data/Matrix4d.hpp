#pragma once

#include <ostream>
#include <vector>

#include "Quaternion.hpp"
#include "Vector3d.h"

#include "constants/window_constants.hpp"
#include <iostream>


namespace aa
{

    // implements main operations and algorithms on 4x4 matrices
    class Matrix4d final
    {

    public:
        // static builders (named constructors)
        static Matrix4d IdentityMatrix4d() {
            Matrix4d output = Matrix4d();
            for (size_t i = 0; i < 4; i++) {
                output.elements[5*i] = 1;
            }
            return output;
        }

        static Matrix4d TranslationMatrix(const Vector3d &position) {
            Matrix4d output = IdentityMatrix4d();
            output.elements[3] = position.x;
            output.elements[7] = position.y;
            output.elements[11] = position.z;
            return output;
        }

        static Matrix4d RotateAround(const Quaternion &q, const Vector3d &point) {
            Matrix4d output = TranslationMatrix(point);
            output *= Matrix4d(q);
            output *= TranslationMatrix(-point);
            return output;
        }

        static Matrix4d ViewportMatrix() {
            Matrix4d output = IdentityMatrix4d();
            for (size_t i = 0; i < 3; i++) {
#ifdef IMPL_OPENGL
                output.elements[5*i] = 0.005f;
#endif
#ifdef IMPL_VULKAN
                output.elements[5 * i] = 0.005f;
#endif
            }
            return output;
        }


        // constructors

        Matrix4d() {
            elements.resize(16);
        }

        Matrix4d(const Matrix4d& other) : elements(other.elements) { }

        Matrix4d(const Quaternion& other) {
            double xx = other.x * other.x;
            double xy = other.x * other.y;
            double xz = other.x * other.z;
            double xw = other.x * other.w;

            double yy = other.y * other.y;
            double yz = other.y * other.z;
            double yw = other.y * other.w;

            double zz = other.z * other.z;
            double zw = other.z * other.w;

            elements.insert(
                elements.end(),
                {
                    1.0f - 2.0f * float(yy + zz), 2.0f * float(xy - zw), 
                    2.0f * float(xz + yw), 0.0f,
                    2.0f * float(xy + zw), 1.0f - 2.0f * float(xx + zz), 
                    2.0f * float(yz - xw), 0.0f,
                    2.0f * float(xz - yw), 2.0f * float(yz + xw), 
                    1.0f - 2.0f * float(xx + yy), 0.0f,
                    0.0f, 0.0f, 
                    0.0f, 1.0f
                }
            );
        }


        // maths operators

        Matrix4d& operator = (const Matrix4d& other) {
            elements = other.elements;
            return *this;
        }

        Matrix4d& operator *= (const Matrix4d& other) {
            Matrix4d result;
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    for (size_t k = 0; k < 4; k++) {
                        result.elements[i*4 + j] +=
                            elements[i*4 + k] * other.elements[k*4 + j];
                    }
                }
            }
            elements = result.elements;
            return *this;
        }

        Matrix4d& operator *= (float num) {
            for (size_t i = 0; i < 16; i++) {
                elements[i] *= num;
            }
            return *this;
        }


        // getters

        float* data() {
            return elements.data();
        }


        // delete all implicit constructors and unneeded operators

        Matrix4d& operator = (Matrix4d&&) = delete;


    private:
        std::vector<float> elements;

    };

}
