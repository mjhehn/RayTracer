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
    double maxLength;   //use this times sqrt of 2 in order to generate a sphere around it.

    void print();
    void printToFile(string folder);
    void buildFromFile(string filename);

    bool checkIntersection(Ray& ray);
    
    void updateMaxLength();

    static Matrix4d buildRotationMatrix(double wx, double wy, double wz, double theta);
    static Matrix4d buildTransformationMatrix(double tx, double ty, double tz);
    static Matrix4d buildScaleMatrix(double scale);

    Vector3d solve(int i, Ray ray);
private:
    Vector3d solve(Vector3d& a, Vector3d& b, Vector3d& c, Vector3d& d, Vector3d& l);
    



};

#endif