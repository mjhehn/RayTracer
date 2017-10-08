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
    Vector3d startPoint;    //point starts from
    double t;
    Vector3d dirVector; //direction going. startPoint-Eye normalized.

    Ray(Vector3d pos, double ct, Vector3d eye): startPoint(pos), t(ct), dirVector(startPoint-eye) {dirVector.normalize();};
    Ray(Vector3d pos, double ct): startPoint(pos), t(ct){};
    ~Ray(){};

    inline void setDirVector(Vector3d& eye)
    {
        dirVector = startPoint-eye;
        dirVector.normalize();
    }
};

inline ostream& operator<<(ostream& out, const Ray& r)
{
    return out<<"\nT: "<<r.t<<" Pos: "<<r.startPoint<<"\n Vector: "<<r.dirVector<<"\n";
}

inline bool operator<(const Ray& lhs, const Ray& rhs)
{
    return lhs.t<rhs.t;
}


#endif