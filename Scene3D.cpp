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
            Sphere newSphere(x,y,z,radius, Material(fin, true));
            spheres.push_back(newSphere);
        }
        else if(buffer == "eye")
        {
            Camera3D tempCam(fin);
            camera = tempCam;
        }
        else if(buffer == "ambient")
        {
            fin>>ambient[0]>>ambient[1]>>ambient[2];
        }
        else if(buffer == "light")
        {
            lights.push_back(Light3D(fin));
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
void Scene3D::printTImage()
{
    std::ofstream fout;
    fout.open("T"+imageName);
    fout<<"P3"<<"\n";
    fout<<camera.resX<<" "<<camera.resY<<" "<<255<<"\n";
    double ratio = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    int j = 1;
    for(unsigned int i = 0; i < camera.image.cols(); ++i)
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

void Scene3D::printImage()
{
    std::ofstream fout;
    fout.open(imageName);
    fout<<"P3"<<"\n";
    fout<<camera.resX<<" "<<camera.resY<<" "<<255<<"\n";
    int j = 1;
    int r = 0;
    int g = 0;
    int b = 0;
    for(unsigned int i = 0; i < camera.image.cols(); ++i)
    {   
        r = camera.image.col(i)[0]*255;
        g = camera.image.col(i)[1]*255;
        b = camera.image.col(i)[2]*255;
        r = std::max(0, std::min(255, r));
        g = std::max(0, std::min(255, g));
        b = std::max(0, std::min(255, b));
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

void Scene3D::rayTrace(Ray& ray, Vector3d& color, bool& tsNotSet)
{
    double lowt = 0;
    Vector3d hitNormal;
    Vector3d hitPoint;
    Material mat;
    bool intersected = false;
    for(unsigned int k = 0; k<objects.size(); ++k){
        //check for intersection with sphere defined by the object.
        for(unsigned int l = 0; l < objects[k].faces.size(); ++l){
            intersected = objects[k].checkIntersection(l, ray);   
            if(intersected){
                if(lowt == 0){lowt = ray.t;}
                else{;}

                if(tsNotSet){
                    tmin = tmax = ray.t;
                    tsNotSet = false;
                }
                else if (ray.t <= lowt)
                {
                    lowt = std::min(lowt, ray.t);
                    tmin = std::min(tmin, ray.t);
                    
                    //average the normals of the corners of the face.
                    Vector3d A = objects[k].objectMatrix.col(objects[k].faces[l].point1-1).head<3>();
                    Vector3d B = objects[k].objectMatrix.col(objects[k].faces[l].point2-1).head<3>();
                    Vector3d C = objects[k].objectMatrix.col(objects[k].faces[l].point3-1).head<3>();
                    hitNormal = (B-A).cross(C-A)*-1;
                    mat = *(std::find(objects[k].mat.begin(), objects[k].mat.end(), objects[k].faces[l].material));
                    hitNormal.normalize();
                    color = colorize(ray, hitNormal, mat);
                }
                else{;}
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
            else //if(ray.t == std::min(lowt, ray.t))
            {
                lowt = std::min(lowt, ray.t);
                tmin = std::min(tmin, ray.t); 
                hitNormal = (ray.startPoint + ray.t*ray.dirVector) - spheres[k].center();
                mat = spheres[k].material;
                hitNormal.normalize();
                color = colorize(ray, hitNormal, mat);
            }
        }
        else{;}
    }
    ray.t = lowt;
    tmax = std::max(tmax, ray.t);
    return; //return color here
}

Vector3d Scene3D::colorize(Ray& ray, const Vector3d& hitNormal, const Material& mat )
{
    Vector3d hitPoint =(ray.startPoint + ray.t*ray.dirVector);
    Vector3d color;

    color<<  ambient[0]*mat.Ka[0], ambient[1]*mat.Ka[1], ambient[2]*mat.Ka[2];
    for(unsigned int i = 0; i < lights.size(); i++)
    {
        Vector3d vectorToLight;
        vectorToLight << lights[i].location[0] - hitPoint[0], lights[i].location[1] - hitPoint[1], lights[i].location[2] - hitPoint[2];
        vectorToLight.normalize();
        if(hitNormal.dot(vectorToLight) > 0.0)
        {
            double normalDottoLight = hitNormal.dot(vectorToLight);
            color[0] += (mat.Kd[0]*lights[i].rgb[0])*normalDottoLight;
            color[1] += (mat.Kd[1]*lights[i].rgb[1])*normalDottoLight;
            color[2] += (mat.Kd[2]*lights[i].rgb[2])*normalDottoLight;

            Vector3d vectorToCamera = ray.startPoint - hitPoint;
            vectorToCamera.normalize();
            Vector3d spR = (2*(normalDottoLight)*hitNormal)-vectorToLight;

            double spectralFalloff = pow((vectorToCamera.dot(spR)),mat.phong);
            color[0] += mat.Ks[0] * lights[i].rgb[0]* spectralFalloff;
            color[1] += mat.Ks[1] * lights[i].rgb[1]* spectralFalloff;
            color[2] += mat.Ks[2] * lights[i].rgb[2]* spectralFalloff;
            
        }
    }

    return color;
}

void Scene3D::castRays()
{
    bool tsNotSet = true;
    for(int j =  camera.resY-1; j >=0; --j){
        for(int i =  camera.resX-1; i >=0; --i){
            Ray ray(camera.pixelPoint(i, j), 0, camera.eye);
            //if(i<10){cout<<(RowVector3d)ray.dirVector<<endl;}
            Vector3d color;
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
                //background color;

            rayTrace(ray, color, tsNotSet);
            
            //swap to inserting rgb color into camera.image here
            camera.rayTVals.push_back(ray.t);
            camera.addToImage(color);
        }
    }
    //cout<<tmin<<" "<<tmax<<endl;
}