/*
Matt Hehn
09/09/2017
Colorado State - CS410 Computer Graphics

Programming Assignment #1
*/

#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <Object3D.h>

Matrix4f buildRotationMatrix(double wx, double wy, double wz, double theta);
Matrix4f buildTransformationMatrix(double tx, double ty, double tz);
Matrix4f buildScaleMatrix(double scale);


int main(int argc, const char** argv)
{
    double wx = 0;
    double wy = 0;
    double wz = 0;
    double theta = 0;

    double scale = 0;

    double tx = 0;
    double ty = 0;
    double tz = 0;

    string fileName = "";

    int returnCode = 0;
    ifstream fin;
    fin.open(argv[1]);
    if(!fin.fail())
    {	//TODO: add a loop
        string buffer = "";
        fin>>buffer;
        if(buffer == "model")
        {
            fin>>wx>>wy>>wz>>theta>>scale>>tx>>ty>>tz>>fileName;
            Object3D newObject = Object3D(fileName);
			Matrix4f R = buildRotationMatrix(wx, wy, wz, theta);
			Matrix4f T = buildTransformationMatrix(tx, ty, tz);
			Matrix4f S = buildScaleMatrix(scale);
			Matrix4f MM = T*S*R;
        }
    }
    else{
        std::cerr<<"File not found or not opening"<<std::endl;
        returnCode = -1;
    }
    fin.close();
    return returnCode;
}

Matrix4f buildRotationMatrix(double wx, double wy, double wz, double theta)
{
	Vector3f axisOfRotationW;       //build vector W, axis of rotation.
	axisOfRotationW << wx, wy, wz;
	axisOfRotationW.normalize();

	//begin building the other axis vectors to match W.
	Vector3f tempvector;
	//pick smallest term in w, set to 1, normalize again.
	if(wx <= wy && wx <= wz){wx=1;}
	else if(wy <= wz){wy=1;}
	else{wz=1;}
	tempvector << wx, wy, wz;
	//cross above with w to get U
	Vector3f axisOfRotationU = axisOfRotationW.cross(tempvector);
	Vector3f axisOfRotationV = axisOfRotationU.cross(axisOfRotationW);
	//cross u and w to get v
	//throw in 4x4 matrix, u v w top to bottom as homogeneous
	Matrix4f Rw; //matrix of rotation.
	Rw.row(0) << axisOfRotationU(0), axisOfRotationU(1), axisOfRotationU(2), 0;
	Rw.row(1) << axisOfRotationV(0), axisOfRotationV(1), axisOfRotationV(2), 0;
	Rw.row(2) << axisOfRotationW(0), axisOfRotationW(1), axisOfRotationW(2), 0;
	Rw.row(3) << 0, 0, 0, 1;

	//get the inverse/transpose for inverting the rotation afterward afterward.
	Matrix4f RwInverse = Rw.transpose();
	
	Matrix4f Rtheta;	//build the actual rotation matrix.
	Rtheta <<   cos(theta), -sin(theta), 0, 0,
				sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
	
	//Rw^-1 Rz Rw
	
	return RwInverse*Rtheta*Rw;
}

Matrix4f buildTransformationMatrix(double tx, double ty, double tz)
{
	Matrix4f T = Matrix4f::Identity();
	T.col(3) << tx, ty, tz, 1;
	return T;
}

Matrix4f buildScaleMatrix(double scale)
{
	Matrix4f S = Matrix4f::Identity();
	S *= scale;
	S.col(3).tail(1) << 1;
	return S;
}