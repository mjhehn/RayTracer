#ifndef OBJECT3D_H_INCLUDED
#define OBJECT3D_H_INCLUDED

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

#include <Plane.h>
#include <Ray.h>

#define PI 3.14159265	//pi definition

class Object3D
{
public:
    Object3D();
    Object3D(ifstream& fin);
    ~Object3D();
    string objectName;
    string comments;
    std::vector<Plane> planes;
    Matrix<double, 4, Dynamic> objectMatrix;
    Vector3d center;
    double sphereRadius;

    void print();
    void printToFile(string folder);
    void buildFromFile(string filename);
    
    void updateExtents();
    bool checkIntersection(Plane& plane, Ray& ray);
    bool checkIntersection2(Plane& plane, Ray& ray);

    static Matrix4d buildRotationMatrix(double wx, double wy, double wz, double theta);
    static Matrix4d buildTransformationMatrix(double tx, double ty, double tz);
    static Matrix4d buildScaleMatrix(double scale);

private:
    bool checkSphere(Ray& ray);
};

#endif