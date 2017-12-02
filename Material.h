#ifndef MATERIAL_INCLUDED
#define MATERIAL_INCLUDED

#include <fstream>
using std::ifstream;
#include <sstream>
using std::stringstream;

#include <string>
#include <vector>
#include <Eigen/Core>
using namespace Eigen;

class Material
{
public:
    std::string name;
    double phong;
    //all material designations are in r = 0, g = 1, b = 2 index order.
    Vector3d Ka;
    Vector3d Kd;
    Vector3d Ks;
    Vector3d Kr;
    Vector3d Ko;
    double etaIn;
    

    Material(std::string passedName, double phongIn, Vector3d fileKa, Vector3d fileKd, Vector3d fileKs, Vector3d fileKr, Vector3d fileKo) : name(passedName), phong(phongIn), Ka(fileKa), Kd(fileKd), Ks(fileKs), Kr(fileKr), Ko(fileKo){etaIn = 1.0;};

    Material(ifstream& fin, bool isSphere)
    {
        if(isSphere)
        {
                name = "";
                fin>>Ka[0]>>Ka[1]>>Ka[2];
                fin>>Kd[0]>>Kd[1]>>Kd[2];
                fin>>Ks[0]>>Ks[1]>>Ks[2];
                fin>>Kr[0]>>Kr[1]>>Kr[2];
                fin>>Ko[0]>>Ko[1]>>Ko[2];
                fin>>etaIn;
                phong = 16;
        }
    }

    Material(std::stringstream& ssin)
    {
            name = "";
            ssin>>Ka[0]>>Ka[1]>>Ka[2];
            ssin>>Kd[0]>>Kd[1]>>Kd[2];
            ssin>>Ks[0]>>Ks[1]>>Ks[2];
            Kr[0] = Kr[1] = Kr[2] = 1;
            Ko[0] = Ko[1] = Ko[2] = 1;
            phong = 16;
    }
    Material(){};
    ~Material(){};

};

inline bool operator==(const Material& lhs, const Material& other)
{
        return lhs.name == other.name;
}

inline bool operator==(const Material& lhs, const std::string& other)
{
        return lhs.name == other;
}

inline ostream& operator<<(ostream& out, const Material& mat)
{
    return out<<mat.name<<"\n";
}


#endif