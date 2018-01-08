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

    inline Vector3d getCenter()
    {
        Vector3d center;
        center<<x,y,z;
        return center;
    }

    inline Vector3d refractRay(const Vector3d& incomingRay, const Vector3d& hitNormal, double eta1, double eta2)
    {
        double etaRatio = eta1/eta2;
        double rayDotNorm = incomingRay.dot(hitNormal);
        double radSquare = (etaRatio*etaRatio)*((rayDotNorm*rayDotNorm)-1) +1;
        Vector3d refractedRay = Vector3d::Zero(3);
        if(radSquare >=0)
        {
            double b = etaRatio*rayDotNorm - sqrt(radSquare);   //why b? I don't know. because it works, and it's what Ross used.
            refractedRay = (-etaRatio)*incomingRay + b*hitNormal;
        }
        else{;}
        return refractedRay;

    }

    inline Ray refractExit(const Vector3d& incomingRay, const Vector3d& hitPoint, const Vector3d& hitNormal, const double& etaMat, const double& etaScene)
    {
        Vector3d center = getCenter();
        Vector3d rayDir = refractRay(incomingRay, hitNormal, etaScene, etaMat);
        Ray refRay(Vector3d::Zero(3), std::numeric_limits<double>::max());
        if(rayDir.sum() != 0)
        {
            Vector3d exitPoint = (center-hitPoint);            
            exitPoint = hitPoint + 2*exitPoint.dot(rayDir)*rayDir;
            Vector3d hitNormalOut = (center-exitPoint);
            hitNormalOut.normalize();
            Vector3d rayDirOut = refractRay(-rayDir, hitNormalOut, etaMat, etaScene);
            //rayDirOut.normalize();
            refRay.startPoint = exitPoint;
            refRay.dirVector = rayDirOut;
        }
        else{;}
       
        return refRay;
    }
};

inline ostream& operator<<(ostream& out, const Sphere& sphere)
{
    return out<<"\nRadius: "<<sphere.radius<<" Pos: "<<sphere.x<<" "<<sphere.y<<" "<<sphere.z<<"\n";
}


#endif