/*
Matt Hehn
12/7/2017
Colorado State - CS410 Computer Graphics

Programming Assignment #5
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
#include <Scene3D.h>

int main(int argc, const char** argv)
{
    int returnCode = 0;
    Scene3D scene(argv[2], argv[1]);
    scene.castRays();
    scene.printImageNew();
    //scene.printTImage();
    return returnCode;
}

