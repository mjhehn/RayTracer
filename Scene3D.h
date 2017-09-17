#ifndef OBJECT3D_H_INCLUDED
#define OBJECT3D_H_INCLUDED

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <Object3D.h>

class Scene3D
{
public: 
    Scene3D(){};
    ~Scene3D(){};

    int numberOfObjects;
    string sceneName;
    //vector<Object3D> sceneObjects;
};

#endif