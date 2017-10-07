#include <Scene3D.h>

Scene3D::Scene3D(string imageFileName, string driverFileName)
{
    imageName = imageFileName;
    ifstream fin;
    tmin = 1;
    tmax = -1;
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

void Scene3D::printImage()
{
    std::ofstream fout;
    fout.open(imageName);
    fout<<"P3"<<"\n";
    fout<<camera.resX<<" "<<camera.resY<<" "<<255<<"\n";

    getTVals();

    int numCols = camera.pixelPoints.cols();

    double ratio = 0;
    double r = 0;
    double g = 0;
    double b = 0;
    for(int j = 0; j < numCols; j++)   //c++11 range based looping
    {
        ratio = 2*(tVals[j]-tmin)/(tmax-tmin);
        r = std::max(0.0, 255*(1-ratio));
        b = std::max(0.0, 255*(ratio-1));
        g = 255-b-r;
        fout<<r<<" "<<g<<" "<<b<<"\n";
    }
    fout.close();
    cout<<tmin<<" "<<tmax<<endl;
}

void Scene3D::getTVals()
{
    int numCols = camera.pixelPoints.cols();
    Vector3d lookVector = -(camera.eye-camera.look);
    lookVector.normalize();
    for(int j = 0; j < numCols; j++) 
    {
        //cout<<j<<endl;
        Ray ray(camera.pixelPoints.col(j), 0, lookVector);
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            bool intersected = false;
            intersected = objects[i].checkIntersection(ray);
            if(intersected)
            {
                cout<<" yay ";
            }
            if(ray.t > tmax)
            {
                tmax = ray.t;
            }
            else if (ray.t<tmin)
            {
                tmin = ray.t;
            }
            tVals.push_back(ray.t);
        }
    }
}