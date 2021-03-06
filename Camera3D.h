#ifndef CAM3D_H_INCLUDED
#define CAM3D_H_INCLUDED


#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;
#include <vector>
#include <limits>


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
    Vector3d WV;
    Vector3d UV;
    Vector3d VV;
    double distance;
    Vector4d  imageRect;    //L, B, R, T
    int resX;
    int resY;
    vector<double> rayTVals;
    Matrix<Vector3d, Dynamic, Dynamic> imageParallel;
    Matrix<double, 3, Dynamic> image;
    int imagePosition;
    int recursionDepth;

    void print();
    void addToImage(Vector3d& pixel);
    void addToImage(const int& x, const int& y, const Vector3d& pixel);
    Vector3d pixelPoint(double i, double j) const;
};

#endif