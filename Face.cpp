#include <Face.h>

Face::Face(int a, int b, int c)
{
    point1 = a;
    point2 = b;
    point3 = c;
    point3Normal = point2Normal = point1Normal = -2;
    textures = false;
    normals = false;
    material = "";
} 

Face::Face(bool texturesPresent, bool normalsPresent, std::vector<int> points)
{
    normals = normalsPresent;
    textures = texturesPresent;
    point3Normal = point2Normal = point1Normal = -2;
    point3Texture = point2Texture = point1Texture = -2;

    if(normals && textures)
    {
        point1 = points[0];
        point1Texture = points[1];
        point1Normal = points[2];
        point2 = points[3];
        point2Texture = points[4];
        point2Normal = points[5];
        point3 = points[6];
        point3Texture = points[7];
        point3Normal = points[8];
        
    }
    else if(normals)
    {
        point1 = points[0];
        point1Normal = points[1];
        point2 = points[2];
        point2Normal = points[3];
        point3 = points[4];
        point3Normal = points[5];
        
    }
    else if(textures)
    {
        point1 = points[0];
        point1Texture = points[1];
        point2 = points[2];
        point2Texture= points[3];
        point3 = points[4];
        point3Texture = points[5];
    }
    else
    {
        point1 = points[0];
        point2 = points[1];
        point3 = points[2];
    }
    material = "";
}

Face::Face(bool texturesPresent, bool normalsPresent, std::vector<int> points, std::string mat)
{
    normals = normalsPresent;
    textures = texturesPresent;
    point3Normal = point2Normal = point1Normal = -2;
    point3Texture = point2Texture = point1Texture = -2;

    if(normals && textures)
    {
        point1 = points[0];
        point1Texture = points[1];
        point1Normal = points[2];
        point2 = points[3];
        point2Texture = points[4];
        point2Normal = points[5];
        point3 = points[6];
        point3Texture = points[7];
        point3Normal = points[8];
        
    }
    else if(normals)
    {
        point1 = points[0];
        point1Normal = points[1];
        point2 = points[2];
        point2Normal = points[3];
        point3 = points[4];
        point3Normal = points[5];
        
    }
    else if(textures)
    {
        point1 = points[0];
        point1Texture = points[1];
        point2 = points[2];
        point2Texture= points[3];
        point3 = points[4];
        point3Texture = points[5];
    }
    else
    {
        point1 = points[0];
        point2 = points[1];
        point3 = points[2];
    }
    material = mat;
    
}

Face::~Face()
{
    ;
}