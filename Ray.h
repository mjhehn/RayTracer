#ifndef RAY_INCLUDED
#define RAY_INCLUDED

#include <iostream>
using std::ostream;
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

class Ray
{
public:
    Vector3d l;
    double t;
    Vector3d d;
    Vector3d tRay;    

    Ray(Vector3d pos, double ct, Vector3d dir): l(pos), t(ct), d(dir) {};
    ~Ray(){};
};

inline ostream& operator<<(ostream& out, const Ray& r)
{
    return out<<"\nT: "<<r.t<<" Vector: "<<r.l<<"\n"<<r.d<<"\n";
}


#endif