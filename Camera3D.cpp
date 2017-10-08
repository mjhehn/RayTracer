#include <Camera3D.h>

Camera3D::Camera3D()
{
    eye = Vector3d::Zero();
    look=Vector3d::Zero();
    up=Vector3d::Zero();
    distance = 0;
    imageRect=Vector4d::Zero();
    resX = resY = 0;
}

Camera3D::Camera3D(ifstream& fin)
{
    string buffer = "";
    fin>>eye[0]>>eye[1]>>eye[2];
    fin>>buffer>>look[0]>>look[1]>>look[2];
    fin>>buffer>>up[0]>>up[1]>>up[2];
    fin>>buffer>>distance;
    fin>>buffer>>imageRect[0]>>imageRect[1]>>imageRect[2]>>imageRect[3];
    fin>>buffer>>resX>>resY;
    for(int i = 0; i < resY; i++)
    {
        for(int j = 0; j < resX; j++)
        {
            pixelPoint(j, i);
        }
    }
}

Camera3D::~Camera3D()
{
    ;
}


Camera3D::Camera3D(const Camera3D& cam)
{
    eye[0] = cam.eye[0];
    eye[1] = cam.eye[1];
    eye[2] = cam.eye[2];

    look[0] = cam.look[0];
    look[1] = cam.look[1];
    look[2] = cam.look[2];

    up[0] = cam.up[0];
    up[1] = cam.up[1];
    up[2] = cam.up[2];
    
    distance = cam.distance;

    imageRect[0] = cam.imageRect[0];
    imageRect[1] = cam.imageRect[1];
    imageRect[2] = cam.imageRect[2];
    imageRect[3] = cam.imageRect[3];

    resX = cam.resX;
    resY = cam.resY;
}

void Camera3D::print()
{
    cout<<"Eye: "<<eye<<endl;
    cout<<"Look: "<<look<<endl;
    cout<<"Up: "<<up<<endl;
    cout<<"Distance: "<<distance<<endl;
    cout<<"Rect: "<<imageRect<<endl;
    cout<<"Res: "<<resX<<" "<<resY<<endl;
}

Vector3d Camera3D::pixelPoint(double i, double j)
{
    double px = i/(resX-1)*(imageRect[2] - imageRect[0])+imageRect[0];
    double py = j/(resY-1)*(imageRect[3] - imageRect[1])+imageRect[1];
    Vector3d WV = (eye-look)*-1;
    WV.normalize();
    Vector3d UV = up.cross(WV);
    UV.normalize();
    Vector3d VV = WV.cross(UV);
    Vector3d pixPoint = eye + (distance*WV) + (px*UV) + (py*VV);
    return pixPoint;
}


