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
    int j = 1;
    for(int i = camera.rayTVals.size()-1; i>=0; --i)
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
        if((j)%camera.resX == 0)
        {
            fout<<"\n";
        }
        ++j;
    }
    fout<<"\n";      
    
    fout.close();
}

void Scene3D::castRays()
{
    bool tsNotSet = true;
    bool intersected = false;
    for(int j = 0; j < camera.resY; ++j){
        for(int i = 0; i < camera.resX; ++i){
            Ray ray(camera.pixelPoint(i, j), 0, camera.eye);
            cout<<ray<<endl;
            double lowt = 0;
            for(unsigned int k = 0; k<objects.size(); ++k){
                //check for intersection with sphere defined by the object.
                for(unsigned int l = 0; l < objects[k].planes.size(); ++l){
                    intersected = objects[k].checkIntersection(objects[k].planes[l], ray);   
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
            for(unsigned int k = 0; k<spheres.size(); ++k){
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
    //cout<<tmin<<" "<<tmax<<endl;
}