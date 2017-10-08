#ifndef CAM3D_H_INCLUDED
#define CAM3D_H_INCLUDED


#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;
#include <vector>

#include <Ray.h>
#include <Object3D.h>

class Camera3D
{
public:
    Camera3D();
    Camera3D(ifstream& fin);
    ~Camera3D();

    Camera3D(const Camera3D& cam);

    Vector3d eye;
    Vector3d look;
    Vector3d  up;
    double distance;
    Vector4d  imageRect;    //L, B, R, T
    int resX;
    int resY;
    vector<double> rayTVals;

    void print();
    Vector3d pixelPoint(double i, double j);

};

#endif