#include <Scene3D.h>

Scene3D::Scene3D(string imageFileName, string driverFileName)
{
    imageName = imageFileName;
    ifstream fin;
    tmin = std::numeric_limits<double>::max();
    tmax = std::numeric_limits<double>::min();
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

void Scene3D::printImageNew()
{
    std::ofstream fout;
    fout.open(imageName);
    fout<<"P3"<<"\n";
    fout<<camera.resX<<" "<<camera.resY<<" "<<255<<"\n";
    int r = 0;
    int g = 0;
    int b = 0;
    for(int i = camera.resY-1; i>=0; --i){
        for(int j = camera.resX-1; j>=0; --j)
        {

            r = camera.imageParallel(j,i)[0]*255;
            g = camera.imageParallel(j,i)[1]*255;
            b = camera.imageParallel(j,i)[2]*255;
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));
            fout<<r<<" "<<g<<" "<<b<<" ";
        }
        fout<<"\n";
    }
    fout<<"\n";      
    
    fout.close();
}

Vector3d Scene3D::rayTrace(Ray& ray, Vector3d color, Vector3d attenuation, int objectHit, int sphereHit, int recursionLevel)
{
    double lowt = std::numeric_limits<double>::max();
    Vector3d hitNormal;
    Vector3d hitPoint;
    Material mat;
    bool intersected = false;
    bool somethingHit = false;
    for(unsigned int k = 0; k<objects.size(); ++k){
        //check for intersection with sphere defined by the object.
        for(unsigned int l = 0; l < objects[k].faces.size(); ++l){
            intersected = objects[k].checkIntersection(l, ray);   
            if(intersected && ray.t <= lowt && objectHit != k)
            {
                if(!somethingHit)
                    somethingHit = true;
                lowt = std::min(lowt, ray.t);
                tmin = std::min(tmin, ray.t);
                
                //average the normals of the corners of the face.
                objectHit = k;
                sphereHit = -1;
                if(objects[k].faces[l].normals)
                {
                    hitNormal = (-1.0)*(1.0/3.0)*(objects[k].normalsMatrix.col(objects[k].faces[l].point1Normal-1).head<3>() + objects[k].normalsMatrix.col(objects[k].faces[l].point2Normal-1).head<3>() + objects[k].normalsMatrix.col(objects[k].faces[l].point3Normal-1).head<3>());                        
                    //cout<<endl<<objects[k].normalsMatrix.col(objects[k].faces[l].point1Normal-1).head<3>()<<" + "<<objects[k].normalsMatrix.col(objects[k].faces[l].point2Normal-1).head<3>()<<" + "<<objects[k].normalsMatrix.col(objects[k].faces[l].point3Normal-1).head<3>()<<endl;
                    //cout<<hitNormal<<endl;
                }
                else{
                    ;
                }
                
                mat = *(std::find(objects[k].mat.begin(), objects[k].mat.end(), objects[k].faces[l].material));
            }
            else{;}
        }
    }

    //checkspheres
    for(unsigned int k = 0; k<spheres.size(); ++k){
        intersected = spheres[k].checkIntersection(ray);   
        if(intersected && sphereHit != k)
        {
            if(!somethingHit)
                somethingHit = true;
            lowt = std::min(lowt, ray.t);
            tmin = std::min(tmin, ray.t); 
            hitNormal = (ray.startPoint + ray.t*ray.dirVector) - spheres[k].center();
            mat = spheres[k].material;
            sphereHit = k;
            objectHit = -1;
        }
        else{;}
    }
    

    if(somethingHit)
    {
        ray.t = lowt;
        tmax = std::max(tmax, ray.t);
        hitNormal.normalize();
        color = colorize(ray, hitNormal, mat, color, attenuation, recursionLevel, sphereHit, objectHit); 
    }
    return color;
}

Vector3d Scene3D::colorize(Ray& ray, const Vector3d& hitNormal, const Material& mat, Vector3d accumulatedColor, Vector3d attenuation, int recursionLevel, int sphereHit, int objectHit)
{
    Vector3d hitPoint =(ray.startPoint + ray.t*ray.dirVector);
    Vector3d color;

    color<<  ambient[0]*mat.Ka[0], ambient[1]*mat.Ka[1], ambient[2]*mat.Ka[2];
    for(unsigned int i = 0; i < lights.size(); i++)
    {
        Vector3d vectorToLight;
        vectorToLight << lights[i].location[0] - hitPoint[0], lights[i].location[1] - hitPoint[1], lights[i].location[2] - hitPoint[2];
        vectorToLight.normalize();
        Ray VtoL(hitPoint, 0, hitPoint+vectorToLight);
        if(checkForLights(VtoL, sphereHit, objectHit))
        {
            if(hitNormal.dot(vectorToLight) > 0.0)
            {
                double normalDottoLight = hitNormal.dot(vectorToLight);
                color[0] += (mat.Kd[0]*lights[i].rgb[0])*normalDottoLight;
                color[1] += (mat.Kd[1]*lights[i].rgb[1])*normalDottoLight;
                color[2] += (mat.Kd[2]*lights[i].rgb[2])*normalDottoLight;

                Vector3d vectorToCamera = ray.startPoint - hitPoint;
                vectorToCamera.normalize();
                Vector3d spR = (2*(normalDottoLight)*hitNormal)-vectorToLight;

                if((vectorToCamera.dot(spR)) > 0.0)
                {
                    double spectralFalloff = pow((vectorToCamera.dot(spR)),mat.phong);
                    color[0] += mat.Ks[0] * lights[i].rgb[0]* spectralFalloff;
                    color[1] += mat.Ks[1] * lights[i].rgb[1]* spectralFalloff;
                    color[2] += mat.Ks[2] * lights[i].rgb[2]* spectralFalloff;
                }
            }
        }   
    }
    //accumulatedColor = color;
    accumulatedColor += attenuation.cwiseProduct(color); //(1.0-mat.reflectivity);
    //cout<<"start: "<<accumulatedColor.transpose()<<"Rec level"<<recursionLevel<<endl;
    if(recursionLevel > 0)
    {
        Vector3d Rv = (ray.dirVector)* (-1);
        Rv = 2*(Rv.dot(hitNormal))*hitNormal - Rv;
        Ray reflectionRay(hitPoint, std::numeric_limits<double>::max());
        reflectionRay.dirVector = Rv;
        attenuation = mat.Kr.cwiseProduct(attenuation);//mat.reflectivity;
        accumulatedColor = rayTrace(reflectionRay, accumulatedColor, attenuation, objectHit, sphereHit, (recursionLevel-1));
    }
    //cout<<accumulatedColor.transpose()<<"Rec level"<<recursionLevel<<endl;

    return accumulatedColor;
}

bool Scene3D::checkForLights(Ray& ray, int sphereHit, int objectHit)
{
    for(unsigned int k = 0; k<spheres.size(); ++k){
        if(signed(k) == sphereHit)
            continue;
        if(spheres[k].checkIntersection(ray))
            return false;
    }
    for(unsigned int k = 0; k<objects.size(); ++k){
        for(unsigned int l = 0; l < objects[k].faces.size(); ++l){
            if(signed(k) == objectHit)
                continue;
            if(objects[k].checkIntersection(l, ray))
                return true;
        }
    }
    
    return true;
}

void Scene3D::castRays()
{
    for(int i = 0; i<camera.resX; ++i){
        for(int j = 0; j < camera.resY; ++j){
            Ray ray(camera.pixelPoint(i, j), std::numeric_limits<double>::max(), camera.eye);
            //if(i<10){cout<<(RowVector3d)ray.dirVector<<endl;}
            Vector3d color;
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
              
            Vector3d attenuation;
            attenuation<<1,1,1;

            color = rayTrace(ray, color, attenuation, -1, -1, camera.recursionDepth);
            if( ray.t == std::numeric_limits<double>::max() || ray.t == std::numeric_limits<double>::min())
                ray.t = 0;
            
            //swap to inserting rgb color into camera.image here
            camera.rayTVals.push_back(ray.t);
            camera.addToImage(color);
            camera.addToImage(i, j, color);
        }
    }
    //cout<<tmin<<" "<<tmax<<endl;
}