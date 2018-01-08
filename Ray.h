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
    double beta;
    double gamma;
    int objectHit;
    int sphereHit;
    Vector3d dirVector; //direction going. startPoint-Eye normalized.

    Ray(Vector3d pos, double ct, Vector3d eye): startPoint(pos), t(ct), dirVector(startPoint-eye) {dirVector.normalize(); sphereHit = -1; objectHit = -1; beta = gamma = 0;};
    Ray(Vector3d pos, double ct): startPoint(pos), t(ct){sphereHit = -1; objectHit = -1; beta = gamma = 0;};
    ~Ray(){startPoint = Vector3d::Zero(3); dirVector = Vector3d::Zero(3); t = gamma = beta = objectHit = sphereHit = -1;};

};

inline ostream& operator<<(ostream& out, const Ray& r)
{
    return out<<"\nT: "<<r.t<<" Pos: "<<(RowVector3d)r.startPoint<<"\n Vector: "<<(RowVector3d)r.dirVector<<"\n";
}

inline bool operator<(const Ray& lhs, const Ray& rhs)
{
    return lhs.t<rhs.t;
}


#endif