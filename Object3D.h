#ifndef OBJECT3D_H_INCLUDED
#define OBJECT3D_H_INCLUDED

#include <iostream>
using std::cout;
#include <sstream>
#include <fstream>
using std::ifstream;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <Plane.h>
#include <Point3D.h>

class Object3D
{
public:
    Object3D(){};
    Object3D(string filename);
    ~Object3D(){};
    string objectName;
    string comments;
    vector<Point3D> points;
    vector<Plane> planes;

    inline void print(){
        std::cout<<objectName<<"\n"<<comments<<"\n"<<points.at(0)<<"\n";
    }
};

#endif