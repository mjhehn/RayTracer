#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <iostream>
using std::ostream;
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <vector>
#include <Ray.h>

class Sphere
{
public:
    double radius;
    double x;
    double y;
    double z;

    Sphere(double cx, double cy, double cz, double rad): radius(rad), x(cx), y(cy), z(cz) {};
    ~Sphere(){};

    inline bool checkIntersection(Ray& ray)
    {
        Vector3d center;
        center<<x,y,z;
        Vector3d vVector = center-ray.startPoint;
        double v = vVector.dot(ray.dirVector);
        double csq = vVector.dot(vVector);
        double dsq = radius*radius - (csq - v*v);
        if(dsq <= 0)
        {
            return false;
        }

        double d = sqrt(dsq);
        ray.t = (v-d);
        return true;
    }
};

inline ostream& operator<<(ostream& out, const Sphere& sphere)
{
    return out<<"\nRadius: "<<sphere.radius<<" Pos: "<<sphere.x<<" "<<sphere.y<<" "<<sphere.z<<"\n";
}


#endif