#ifndef PLANE_INCLUDED
#define PLANE_INCLUDED

#include <iostream>
using std::ostream;
#include <vector>

class Plane
{
public:
    

        //default to -2 if non-existent.
    int point1;
    int point1Texture;
    int point1Normal;
    
    int point2;
    int point2Texture;
    int point2Normal;
    
    int point3;
    int point3Texture;
    int point3Normal;
    
    bool normals;
    bool textures;

    Plane(int a, int b, int c);
    Plane(bool texturesPresent, bool normalsPresent, std::vector<int> points);
    ~Plane(){};

    
};

inline ostream& operator<<(ostream& out, const Plane& pln)
{
    if(pln.textures && pln.normals){return out << pln.point1<<"/"<<pln.point1Texture<<"/"<<pln.point1Normal<<" "<<pln.point2<<"/"<<pln.point2Texture<<"/"<<pln.point2Normal<<" "<<pln.point3<<"/"<<pln.point3Texture<<"/"<<pln.point3Normal;}
    else if(pln.textures){return out << pln.point1<<"/"<<pln.point1Texture<<" "<<pln.point2<<"/"<<pln.point2Texture<<" "<<pln.point3<<"/"<<pln.point3Texture;}
    else if(pln.normals){return out << pln.point1<<"//"<<pln.point1Normal<<" "<<pln.point2<<"//"<<pln.point2Normal<<" "<<pln.point3<<"//"<<pln.point3Normal;}
    else{return out << pln.point1<<" "<<pln.point2<<" "<<pln.point3;}
}


#endif