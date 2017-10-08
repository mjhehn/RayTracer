#include <Scene3D.h>

Scene3D::Scene3D(string imageFileName, string driverFileName)
{
    imageName = imageFileName;
    ifstream fin;
    tmin = 0;
    tmax = 0;
    fin.open(driverFileName);
    
    dirName = driverFileName;		//get the directory name by chopping the file extension and directory off of the driver text file
	size_t lastindex = dirName.find_last_of("."); 
	dirName = dirName.substr(0, lastindex); 

    string buffer = "";
	fin>>buffer;

    while(!fin.fail())
	{
        if(buffer == "model")
        {
            Object3D newObject(fin);
            objects.push_back(newObject);
        }
        else if(buffer == "sphere")
        {
            double x;
            double y;
            double z;
            double radius;
            fin>>x>>y>>z>>radius;
            Sphere newSphere(x,y,z,radius);
            spheres.push_back(newSphere);
        }
        else if(buffer == "eye")
        {
            Camera3D tempCam(fin);
            camera = tempCam;
        }
		fin>>buffer;
    }
	fin.close();
}
Scene3D::~Scene3D()
{
    ;
}

void Scene3D::printObjectsToFile()
{
    for(auto j: objects)   //c++11 range based looping
    {
        j.printToFile(dirName);
    }
    cout<<"\n\n Camera:\n";
    camera.print();
}

void Scene3D::print()
{
    cout<<imageName<<endl;
    cout<<"\n\n Camera:\n";
    camera.print();
    for(auto j: objects)   //c++11 range based looping
    {
        j.print();
    }
    for(auto j: spheres)   //c++11 range based looping
    {
        cout<<j<<endl;
    }
}

 //ray is pixelPoint-eye normalized
void Scene3D::printImage()
{
    std::ofstream fout;
    fout.open(imageName);
    fout<<"P3"<<"\n";
    fout<<camera.resX<<" "<<camera.resY<<" "<<255<<"\n";
    double ratio = 0;
    int r = 0;
    int g = 0;
    int b = 0;

    for(int i = camera.rayTVals.size()-1; i>=0; i--)
    {
        if(camera.rayTVals[i] == 0)
        {
            r = g = b = 239;
        }
        else
        {
            ratio = 2*(camera.rayTVals[i]-tmin)/(tmax-tmin);
            r = std::max(0.0, 255*(1-ratio));
            b = std::max(0.0, 255*(ratio-1));
            g = 255-b-r;
        }
        
        fout<<r<<" "<<g<<" "<<b<<" ";
        if((i+1)%camera.resX == 0)
        {
            fout<<"\n";
        }
    }
        
    
    fout.close();
}

void Scene3D::castRays()
{
    bool tsNotSet = true;
    bool intersected = false;
    for(int j = 0; j < camera.resY; j++){
        for(int i = 0; i < camera.resX; i++){
            Ray ray(camera.pixelPoint(i, j), 0, camera.eye);
            double lowt = 0;
            for(unsigned int k = 0; k<objects.size(); k++){
                //check for intersection with sphere defined by the object.
                for(auto l: objects[k].planes){
                    intersected = objects[k].checkIntersection(l, ray);   
                    if(intersected){
                        if(lowt == 0){lowt = ray.t;}
                        else{;}

                        if(tsNotSet){
                            tmin = tmax = ray.t;
                            tsNotSet = false;
                        }
                        else{
                            lowt = std::min(lowt, ray.t);
                            tmin = std::min(tmin, ray.t);
                            
                        }
                    }
                    else{;}
                }
                
            }

            //checkspheres
            for(unsigned int k = 0; k<spheres.size(); k++){
                intersected = spheres[k].checkIntersection(ray);   
                if(intersected){
                    if(lowt == 0){lowt = ray.t;}
                    else{;}

                    if(tsNotSet){
                        tmin = tmax = ray.t;
                        tsNotSet = false;
                    }
                    else{
                        lowt = std::min(lowt, ray.t);
                        tmin = std::min(tmin, ray.t);
                        
                    }
                }
                else{;}
            }
            ray.t = lowt;
            tmax = std::max(tmax, ray.t);
            
            camera.rayTVals.push_back(ray.t);
        }
    }
    cout<<tmin<<" "<<tmax<<endl;
}

bool Scene3D::checkIntersection(int i, Plane& plane, Ray& ray)
{
    Vector3d a = objects[i].objectMatrix.col(plane.point1-1).head<3>();
    Vector3d b = objects[i].objectMatrix.col(plane.point2-1).head<3>();
    Vector3d c = objects[i].objectMatrix.col(plane.point3-1).head<3>();
    Vector3d d = ray.dirVector;
    Vector3d l = ray.startPoint;
    Matrix3d M;
    M<<a[0]-b[0], a[0]-c[0], d[0],
    a[1]-b[1], a[1]-c[1], d[1],
    a[2]-b[2], a[2]-c[2], d[2];

    Vector3d Y;
    Y<<a[0]-l[0], a[1]-l[1], a[2]-l[2];
    Vector3d X = M.householderQr().solve(Y);

    

    if(X[0] >= 0 && X[1] >= 0)
    {
        if((X[0]+X[1])<=1 && X[2] > 0)
        {
            ray.t = X[2];
            tmin = std::min(tmin, ray.t);
            tmax = std::max(tmax, ray.t);
            return true;
        }
    }
    return false;
}