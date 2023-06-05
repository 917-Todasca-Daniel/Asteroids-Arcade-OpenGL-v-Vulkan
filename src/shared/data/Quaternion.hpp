#pragma once

#include <ostream>
#include <vector>


namespace aa
{
    // implements main operations and algorithms on quaternions
    class Quaternion
    {

    public:
        double w;
        double x;
        double y;
        double z;

        // scalar multiplication
        Quaternion operator * (float scalar) const {
            return Quaternion(w, x * scalar, y * scalar, z * scalar);
        }

        Vector3d rotatePoint(const Vector3d& point) {
            Quaternion pointQ(0.0, point.x, point.y, point.z);
            Quaternion conjugate = Quaternion(w, -x, -y, -z);
            Quaternion resultQ = *this * pointQ * conjugate;

            return Vector3d((float)resultQ.x, (float)resultQ.y, (float)resultQ.z);
        }

        static Quaternion fromYawPitchRoll(double yaw, double pitch, double roll) {
            double cy = cos(yaw * 0.5);
            double sy = sin(yaw * 0.5);
            double cp = cos(pitch * 0.5);
            double sp = sin(pitch * 0.5);
            double cr = cos(roll * 0.5);
            double sr = sin(roll * 0.5);

            Quaternion qYaw(cos(yaw * 0.5), 0.0, 0.0, sin(yaw * 0.5));
            Quaternion qPitch(cos(pitch * 0.5), sin(pitch * 0.5), 0.0, 0.0);
            Quaternion qRoll(cos(roll * 0.5), 0.0, sin(roll * 0.5), 0.0);
            Quaternion output = qRoll * qPitch * qYaw;
            output.normalize();

            return output;
        }

        Quaternion(double w, double x, double y, double z)
            : w(w), x(x), y(y), z(z) { }

        Quaternion& operator = (const Quaternion& other) {
            this->w = other.w;
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
            return *this;
        }

        // useful operations
        void normalize() {
            double mag = norm();
            if (mag > 0.0) {
                double invMag = 1.0 / mag;
                w *= invMag;
                x *= invMag;
                y *= invMag;
                z *= invMag;
            }
        }

        //  math operations
        //  multiplication
        Quaternion operator * (const Quaternion& other) const {
            return Quaternion(
                w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w);
        }

        // conjugation
        Quaternion operator ! () const {
            return Quaternion(w, -x, -y, -z);
        }

        Quaternion(Quaternion&& other) : w(other.w), x(other.x), y(other.y), z(other.z) { }
        Quaternion(Quaternion& other) : w(other.w), x(other.x), y(other.y), z(other.z) { }

        //  delete other implicit constructors 
        Quaternion()              = delete;


    private:
        double norm() const {
            return sqrt(w * w + x * x + y * y + z * z);
        }

    };

}
