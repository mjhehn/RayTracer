#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <iostream>
using std::ostream;
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <vector>
#include <Ray.h>
#include <Material.h>

class Sphere
{
public:
    double radius;
    double x;
    double y;
    double z;
    Material material;
    Sphere(double cx, double cy, double cz, double rad, Material passedMat): radius(rad), x(cx), y(cy), z(cz), material(passedMat) {};
    ~Sphere(){};

    inline bool checkIntersection(Ray& ray)
    {
        Vector3d center;
        center<<x,y,z;
        Vector3d cVector = center-ray.startPoint;
        double v = cVector.dot(ray.dirVector);
        double csquared = cVector.dot(cVector);
        double dsquared = radius*radius - (csquared - v*v);
        if(dsquared < 0)
        {
            return false;
        }

        double d = sqrt(dsquared);
        ray.t = (v-d);
        return true;
    }

    inline Vector3d center()
    {
        Vector3d center;
        center<<x,y,z;
        return center;
    }
};

inline ostream& operator<<(ostream& out, const Sphere& sphere)
{
    return out<<"\nRadius: "<<sphere.radius<<" Pos: "<<sphere.x<<" "<<sphere.y<<" "<<sphere.z<<"\n";
}


#endif