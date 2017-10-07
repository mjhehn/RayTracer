#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include <iostream>
using std::ostream;
#include <vector>

class Sphere
{
public:
    double radius;
    double x;
    double y;
    double z;

    Sphere(double cx, double cy, double cz, double rad): radius(rad), x(cx), y(cy), z(cz) {};
    ~Sphere(){};

    inline bool checkIntersection()
    {
        return 0;
    }
};

inline ostream& operator<<(ostream& out, const Sphere& sphere)
{
    return out<<"\nRadius: "<<sphere.radius<<" Pos: "<<sphere.x<<" "<<sphere.y<<" "<<sphere.z<<"\n";
}


#endif