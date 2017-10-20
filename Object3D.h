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

#include <Face.h>
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
    std::vector<Face> faces;
    std::vector<Material> mat;
    Matrix<double, 4, Dynamic> normalsMatrix;
    Matrix<double, 4, Dynamic> objectMatrix;
    Vector3d center;
    double sphereRadius;

    void print();
    void printToFile(string folder);
    void buildFromFile(string filename);
    
    void updateExtents();
    bool checkIntersection(const int& i, Ray& ray);

    static Matrix4d buildRotationMatrix(double wx, double wy, double wz, double theta);
    static Matrix4d buildTransformationMatrix(const double tx, const double ty, const double tz);
    static Matrix4d buildScaleMatrix(double scale);
    void generateMaterialsList(stringstream& ss);

private:
    bool checkSphere(const Ray& ray);
};

#endif