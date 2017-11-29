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

    inline Vector3d refractRay(const Ray& incomingRay, const Vector3d& hitPoint, const Vector3d& hitNormal, etaOut, etaIn)
    {
        double etaRatio = etaOut/etaIn;
        Vector3d toOrigin = incomingRay.dirVector*(-1.0);
        double rayDotNorm = toOrigin.dot(hitNormal);
        double radSquare = (etaRatio*etaRatio)*((rayDotNorm*rayDotNorm)-1) +1;
        Vector3d refractedRay = Vector3d::Zeros();
        if(radSquare > 0.0001)
        {
            double b = etaRatio*rayDotNorm - sqrt(radSquare);   //why b? I don't know. because it works, and it's what Ross used.
            refractedRay = (-etar)*toOrigin + b*hitNormal;
        }
        else{;}
        return refractedRay;

    }

    inline Ray refractExit(const Ray& incomingRay, const Vector3d& hitPoint, const Vector3d& hitNormal, etaOut, etaIn)
    {
        Vector3d rayIntoSphere = refractRay(incomingRay, hitPoint, (hitPoint-center).normalized(), etaOut, etaIn);
        Ray ray();
        if(rayIntoSphere[0]+rayIntoSphere[1]+rayIntoSphere[2] != 0)
        {
            Vector3d exitPoint = hitPoint + 2*((center-hitPoint).dot(rayIntoSphere))*rayIntoSphere;
            Vector3d hitNormalOut = (center-exitPoint).normalized();
            Vector3d rayOutOfSphere = refractRay((-1*rayIntoSphere), exitPoint, hitNormalOut, etaIn, etaOut);
            ray.startPoint = exitPoint;
            ray.dirVector = rayOutOfSphere;
        }
        else{;}
        return ray;
    }
};

inline ostream& operator<<(ostream& out, const Sphere& sphere)
{
    return out<<"\nRadius: "<<sphere.radius<<" Pos: "<<sphere.x<<" "<<sphere.y<<" "<<sphere.z<<"\n";
}


#endif