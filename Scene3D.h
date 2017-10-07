#ifndef SCENE3D_H_INCLUDED
#define SCENE3D_H_INCLUDED

#include <iostream>
using std::cout;
using std::endl;
#include <sstream>
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <regex>
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <Camera3D.h>
#include <Object3D.h>
#include <Sphere.h>

class Scene3D
{
public:
    Scene3D();
    Scene3D(string imageName, string driverFileName);
    ~Scene3D();
    string imageName;
    string dirName;
    double tmin;
    double tmax;
    std::vector<Object3D> objects;
    std::vector<Sphere> spheres;
    Camera3D camera;
    std::vector<double> tVals;

    


    void print();
    void printObjectsToFile();
    void printImage();
    void getTVals();
};

#endif