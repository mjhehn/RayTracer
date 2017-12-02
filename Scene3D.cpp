#include <Scene3D.h>

Scene3D::Scene3D(string imageFileName, string driverFileName)
{
    eta = 1.0;
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
        else if(buffer == "eta")
        {
            fin>>eta;
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

void Scene3D::rayTrace(Ray& ray, Vector3d& color, Vector3d& attenuation, int sphereOrigin, int objectOrigin, int recursionLevel)
{
    double lowt = std::numeric_limits<double>::max();
    Vector3d hitNormal;
    Vector3d hitPoint;
    Material mat;
    bool intersected = false;
    bool somethingHit = false;
    for(unsigned int k = 0; k<objects.size(); ++k){
        if(signed(k) == objectOrigin){continue;}//make sure to not hit self on way out of reflection
        //check for intersection with sphere defined by the object.
        for(unsigned int l = 0; l < objects[k].faces.size(); ++l){
            intersected = objects[k].checkIntersection(l, ray);   
            if(intersected && ray.t <= lowt && ray.t > 0)
            {
                if(!somethingHit)
                    somethingHit = true;
                lowt = std::min(lowt, ray.t);
                tmin = std::min(tmin, ray.t);
                ray.objectHit = k;
                ray.sphereHit = -1;
                //average the normals of the corners of the face.
                if(objects[k].faces[l].normals)
                {
                    hitNormal = (-1.0)*(1.0/3.0)*((1-(ray.beta+ray.gamma))*objects[k].normalsMatrix.col(objects[k].faces[l].point1Normal-1).head<3>() + (1-ray.beta)*objects[k].normalsMatrix.col(objects[k].faces[l].point2Normal-1).head<3>() + (1-ray.gamma)*objects[k].normalsMatrix.col(objects[k].faces[l].point3Normal-1).head<3>());                        
                    //hitNormal = (-1.0)*(1.0/3.0)*(objects[k].normalsMatrix.col(objects[k].faces[l].point1Normal-1).head<3>() + objects[k].normalsMatrix.col(objects[k].faces[l].point2Normal-1).head<3>() + objects[k].normalsMatrix.col(objects[k].faces[l].point3Normal-1).head<3>());                        
                    //cout<<endl<<objects[k].normalsMatrix.col(objects[k].faces[l].point1Normal-1).head<3>()<<" + "<<objects[k].normalsMatrix.col(objects[k].faces[l].point2Normal-1).head<3>()<<" + "<<objects[k].normalsMatrix.col(objects[k].faces[l].point3Normal-1).head<3>()<<endl;
                    //cout<<hitNormal<<endl;
                }
                else{
                    cout<<"you shouldn't be here. this really doesn't work"<<endl;
                    hitNormal =  (objects[k].objectMatrix.col(objects[k].faces[l].point1-1).head<3>() - objects[k].objectMatrix.col(objects[k].faces[l].point2-1).head<3>()).cross( objects[k].objectMatrix.col(objects[k].faces[l].point3-1).head<3>() - objects[k].objectMatrix.col(objects[k].faces[l].point1-1).head<3>());
                }
                
                mat = *(std::find(objects[k].mat.begin(), objects[k].mat.end(), objects[k].faces[l].material));
            }
            else{;}
        }
    }

    //checkspheres
    for(unsigned int k = 0; k<spheres.size(); ++k){
        if(signed(k) == sphereOrigin){continue;}
        intersected = spheres[k].checkIntersection(ray);   
        if(intersected && ray.t <= lowt && ray.t > 0)
        {
            if(!somethingHit)
                somethingHit = true;
            lowt = std::min(lowt, ray.t);
            tmin = std::min(tmin, ray.t); 
            hitNormal = (ray.startPoint + ray.t*ray.dirVector) - spheres[k].getCenter();
            mat = spheres[k].material;
            ray.objectHit = -1;
            ray.sphereHit = k;
        }
        else{;}
    }
    

    if(somethingHit)
    {
        ray.t = lowt;
        //cout<<lowt<<endl;
        tmax = std::max(tmax, ray.t);
        hitNormal.normalize();
        Vector3d reflection = ray.dirVector*(-1);   //should be normal already
        reflection.normalize(); //but i don't trust it
        Vector3d hitPoint =(ray.startPoint + ray.t*ray.dirVector);
        colorize(reflection, hitPoint, hitNormal, mat, color, attenuation, ray.sphereHit, ray.objectHit);
        attenuation = attenuation.cwiseProduct(mat.Kr);
        if(recursionLevel > 0)
        {
            Vector3d flec;
            flec[0] = flec[1] = flec[2] = 0;
            Ray reflectionRay(hitPoint, std::numeric_limits<double>::max());
            reflectionRay.dirVector = (2)*(hitNormal.dot(reflection))*hitNormal - reflection;
            reflectionRay.dirVector.normalize();
            
            rayTrace(reflectionRay, color, attenuation, ray.sphereHit, ray.objectHit, (--recursionLevel));
            color += attenuation.cwiseProduct(flec).cwiseProduct(mat.Ko);
        }
        if( recursionLevel > 0 && mat.Ko.sum() < 3.0 )
        {
            Vector3d thru;
            thru[0] = thru[1] = thru[2] = 0;
            Ray fraR = spheres[ray.sphereHit].refractExit(-ray.dirVector, hitPoint, hitNormal, mat.etaIn, eta);
            if (fraR.t != -1)
            {
                rayTrace(fraR, thru, attenuation, ray.sphereHit, ray.objectHit, (--recursionLevel));
                Vector3d one;
                one<<1,1,1;
                color += attenuation.cwiseProduct(thru).cwiseProduct(one-mat.Ko);
            }
        }
    }
   // return color;
   return;
}

void Scene3D::colorize(const Vector3d& toCamera, const Vector3d& hitPoint, Vector3d& hitNormal, const Material& mat, Vector3d& accumulatedColor, const Vector3d& attenuation, int sphereHit, int objectHit)
{
    Vector3d color;
    color[0] = ambient[0]*mat.Ka[0];
    color[1] = ambient[1]*mat.Ka[1];
    color[2] = ambient[2]*mat.Ka[2];
    for(unsigned int i = 0; i < lights.size(); i++)
    {
        if(hitNormal.dot(toCamera) < 0)
        {
            hitNormal = hitNormal*-1;
        }

        Vector3d L;
        L << lights[i].location[0], lights[i].location[1], lights[i].location[2];
        Vector3d vectorToLight = L - hitPoint;
        vectorToLight.normalize();
        float normalDotLight = hitNormal.dot(vectorToLight);
        
        if(normalDotLight >= 0.0 && notShadowed(hitPoint, L, sphereHit, objectHit))
        {
            
            color[0] += (mat.Kd[0]*lights[i].rgb[0])*normalDotLight;
            color[1] += (mat.Kd[1]*lights[i].rgb[1])*normalDotLight;
            color[2] += (mat.Kd[2]*lights[i].rgb[2])*normalDotLight;

            Vector3d spR = (2*(normalDotLight)*hitNormal)-vectorToLight;
            spR.normalize();
            float CdR = toCamera.dot(spR);

            if(CdR >= 0.0)
            {
                double spectralFalloff = pow(CdR, mat.phong);
                color[0] += mat.Ks[0] * lights[i].rgb[0]* spectralFalloff;
                color[1] += mat.Ks[1] * lights[i].rgb[1]* spectralFalloff;
                color[2] += mat.Ks[2] * lights[i].rgb[2]* spectralFalloff;
            }
        }   
    }
    
    accumulatedColor += attenuation.cwiseProduct(mat.Ko).cwiseProduct(color);
}

bool Scene3D::notShadowed(const Vector3d& hitPoint, Vector3d& L, const int& sphereHit, const int& objectHit)
{
    Vector3d VtoL = L - hitPoint;
    Ray ray(hitPoint, std::numeric_limits<double>::max());
    ray.dirVector = VtoL.normalized();
    double distToLight = VtoL.dot(ray.dirVector);
    
    for(unsigned int k = 0; k<spheres.size(); ++k){
        if(sphereHit != signed(k) && spheres[k].checkIntersection(ray) && ray.t < distToLight)
            return false;
    }
    for(unsigned int k = 0; k<objects.size(); ++k){
        for(unsigned int l = 0; l < objects[k].faces.size(); ++l){
            if(objectHit != signed(k) && objects[k].checkIntersection(l, ray)  && ray.t < distToLight)
                return false;
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

            rayTrace(ray, color, attenuation, -1, -1, camera.recursionDepth);
            if( ray.t == std::numeric_limits<double>::max() || ray.t == std::numeric_limits<double>::min())
                ray.t = 0;
            
            //swap to inserting rgb color into camera.image here
            //camera.addToImage(color);
            camera.addToImage(i, j, color);
        }
    }
    //cout<<tmin<<" "<<tmax<<endl;
}