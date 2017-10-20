#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED

#include <fstream>
using std::ifstream;

class Light3D
{
public:
    double location[3];
    double w;
    double rgb[3];

    Light3D(double loc[3], double w, double passedrgb[3]) : w(w)
    {
        location[0] = loc[0]; 
        location[1] = loc[1]; 
        location[2] = loc[3];
        rgb[0] = passedrgb[0]; 
        rgb[1] = passedrgb[1]; 
        rgb[2] = passedrgb[3]; 
    }
    Light3D(ifstream& fin)
    {
        fin>>location[0]>>location[1]>>location[2];
        fin>>w;
        fin>>rgb[0]>>rgb[1]>>rgb[2];
    }
    ~Light3D(){};

};



#endif