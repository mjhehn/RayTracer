#ifndef PLANE_INCLUDED
#define PLANE_INCLUDED

#include <Point3D.h>
#include <iostream>
using std::ostream;

class Plane
{
public:
    inline Plane(Point3D a, Point3D b, Point3D c)
    {
        point1 = a;
        point2 = b;
        point3 = c;
    }
    ~Plane(){};

    Point3D point1;
    Point3D point2;
    Point3D point3;

    //inline ostream& operator<<(ostream& out, const Plane& pln){return out << "Plane: ("<<pln.point1<<"), ("<<pln.point2<<"), ("<<pln.point3<<")\n";}
};

#endif