#include <Object3D.h>

Object3D::Object3D()
{
    objectName = "None";
    comments = "Nada";
}

Object3D::Object3D(ifstream& fin)
{
    double wx = 0;
    double wy = 0;
    double wz = 0;
    double theta = 0;

    double scale = 0;

    double tx = 0;
    double ty = 0;
    double tz = 0;

	string tempFileName = "";
	fin>>wx>>wy>>wz>>theta>>scale>>tx>>ty>>tz>>tempFileName;
	buildFromFile(tempFileName);
	Matrix4d R = buildRotationMatrix(wx, wy, wz, theta);
	Matrix4d T = buildTransformationMatrix(tx, ty, tz);
	Matrix4d S = buildScaleMatrix(scale);
	Matrix4d MM = T*S*R;
    objectMatrix = MM*objectMatrix;
    updateExtents();
}

void Object3D::buildFromFile(string filename)
{
    objectName = filename;
    ifstream fin;
    fin.open(filename);
    if(!fin.fail())
    {
        string buffer = "";
        std::getline(fin, buffer);
        while(!fin.fail() && buffer[0] == '#'){
            comments.append("\n"+buffer);
            std::getline(fin, buffer);
        }
        comments.append("\n #modified/translated version of "+filename);
        while(!fin.fail())
        {
            std::stringstream ss;
            string ssbuffer = "";
            ss.str(buffer);
            ss>>ssbuffer;
            if(ssbuffer == "v")
            {
                double x = 0;
                double y = 0;
                double z = 0;
                ss>>x>>y>>z;
                objectMatrix.conservativeResize(NoChange, objectMatrix.cols()+1);
                objectMatrix.col(objectMatrix.cols()-1) << x, y, z, 1;
            }
            else if(ssbuffer == "f")
            {
                std::vector<int> tempPoints;
                bool textures = false;
                bool normals = false;
                
                //R"() notation to convert to string literal and avoid escaping everything.
                if(std::regex_search(buffer, std::regex(R"(\d+(\/)\d+(\/)\d+)")))
                {
                    textures = normals = true;
                }
                else
                {
                    textures = std::regex_search(buffer, std::regex(R"(\d+(\/)\d+)"));
                    normals = std::regex_search(buffer, std::regex(R"(\d+(\/\/)\d+)"));
                }
                
                if(normals) {buffer = std::regex_replace(buffer, std::regex(R"([/]{2})"), " // ");}
                else{buffer = std::regex_replace(buffer, std::regex(R"([/]{1})"), " / "); }//remove /'s from buffer lines, which are used for normal vectors for planes
                
                ss.str(buffer);
                string temp;
                ss>>temp;
                while(!ss.fail())
                {
                    if(isdigit(temp[0]))
                    {
                        if(temp =="-1")
                        {
                            tempPoints.push_back(tempPoints.back());
                        }
                        else{
                            tempPoints.push_back(stof(temp));
                        }  
                    }
                    else{;}
                    ss>>temp;
                }
                planes.push_back(Plane(textures, normals, tempPoints));
            }
            else{;}
            ss.flush();
            std::getline(fin, buffer);
        }   
    }
    else{};
    fin.close();
}

Object3D::~Object3D()
{
    objectName="";
    planes.clear();
}

void Object3D::print()
{
    std::cout<<objectName<<"\n"<<comments<<"\n";
    for (int i = 0; i < objectMatrix.cols(); i++)   
    {
        std::cout<<"v "<<objectMatrix.col(i).row(0)<<" "<<objectMatrix.col(i).row(1)<<" "<<objectMatrix.col(i).row(2)<<"\n";
    }

    for(const auto j: planes)   //c++11 range based looping
    {
        std::cout<<"f "<<j<<std::endl;
    }
}

void Object3D::printToFile(string folder)
{
    string cmdFolder = "mkdir -p "+folder;
    ofstream fout;
    
    //build the folder
    const int dir_err = system(cmdFolder.c_str());
    if (-1 == dir_err)
    {
        std::cerr<<"Error creating directory!\n";
    }

    //get locaiton of . to later pop the .obj off the objectname
    size_t lastindex = objectName.find_last_of("."); 
	

    //check if the file name exists, increment if it doesnt.
    int suffix = 0;
    string stringSuffix ="";
    bool fileNotFound = true;
    string fileName = objectName.substr(0, lastindex)+"_mw";
    while(fileNotFound)
    {
        if(suffix<10){stringSuffix="0";}
        else{stringSuffix="";}
        stringSuffix += std::to_string(suffix);
        ifstream ifile((folder+"/"+fileName+stringSuffix+".obj").c_str());
        if(ifile)
        {
            suffix++;
        }
        else{fileNotFound = false;}
        ifile.close();
    }
    
   
    fileName+= stringSuffix;
    fout.open((folder+"/"+fileName+".obj").c_str());
    fout<<comments<<"\n";
    for (int i = 0; i < objectMatrix.cols(); i++)   
    {
        fout<<"v "<<objectMatrix.col(i).row(0)<<" "<<objectMatrix.col(i).row(1)<<" "<<objectMatrix.col(i).row(2)<<"\n";
    }

    for(const auto j: planes)   //c++11 range based looping
    {
        fout<<"f "<<j<<"\n";
    }
    fout<<"\n";
    fout.close();
}

Matrix4d Object3D::buildRotationMatrix(double wx, double wy, double wz, double theta)
{
	Vector3d axisOfRotationW;       //build vector W, axis of rotation.
	axisOfRotationW << wx, wy, wz;
	axisOfRotationW.normalize();

	//begin building the other axis vectors to match W.
	Vector3d tempvector;
	//pick smallest term in w, set to 1, normalize again.
	if(abs(wx) <= abs(wy) && abs(wx) <= abs(wz)){wx=1;}
	else if(abs(wy) <= abs(wz)){wy=1;}
	else{wz=1;}
	tempvector << wx, wy, wz;
	tempvector.normalize();
	//cross above with w to get U
	Vector3d axisOfRotationU = axisOfRotationW.cross(tempvector);
	Vector3d axisOfRotationV = axisOfRotationU.cross(axisOfRotationW);
	axisOfRotationU.normalize();
	axisOfRotationV.normalize();	//eigen doesn't seem to promise that a unit crossed with a unit will give a unit vector/normalized vector.
	//cross u and w to get v
	//throw in 4x4 matrix, u v w top to bottom as homogeneous
	Matrix4d Rw; //matrix of rotation.
	Rw.row(1) << axisOfRotationU(0), axisOfRotationU(1), axisOfRotationU(2), 0;
	Rw.row(0) << axisOfRotationV(0), axisOfRotationV(1), axisOfRotationV(2), 0;
	Rw.row(2) << axisOfRotationW(0), axisOfRotationW(1), axisOfRotationW(2), 0;
	Rw.row(3) << 0, 0, 0, 1;
	//get the inverse/transpose for inverting the rotation afterward afterward.
	Matrix4d RwInverse = Rw.transpose();
	
	Matrix4d Rtheta;	//build the actual rotation matrix.
	theta = theta*PI/180;
	Rtheta <<   cos(theta), -sin(theta), 0, 0,
				sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
	
	//Rw^-1 Rz Rw
	
	return RwInverse*Rtheta*Rw;
}

Matrix4d Object3D::buildTransformationMatrix(double tx, double ty, double tz)
{
	Matrix4d T = Matrix4d::Identity();
	T.col(3) << tx, ty, tz, 1;
	return T;
}

Matrix4d Object3D::buildScaleMatrix(double scale)
{
	Matrix4d S = Matrix4d::Identity();
	S *= scale;
	S.col(3).tail(1) << 1;
	return S;
}

void Object3D::updateExtents()
{
    Vector3d maxInModel;
    Vector3d minInModel;
    maxInModel[0] = objectMatrix.row(0).maxCoeff();
    maxInModel[1] = objectMatrix.row(1).maxCoeff();
    maxInModel[2] = objectMatrix.row(2).maxCoeff();
    minInModel[0] = objectMatrix.row(0).minCoeff();
    minInModel[1] = objectMatrix.row(1).minCoeff();
    minInModel[2] = objectMatrix.row(2).minCoeff();

    double diameter = maxInModel[0] - minInModel[0];
    double swap = maxInModel[1] - minInModel[1];
    diameter = std::max(diameter, swap);
    swap = maxInModel[2] - minInModel[2];
    diameter = std::max(diameter, swap);

    center[0] = (maxInModel[0] - minInModel[0])*0.5 + minInModel[0];
    center[1] = (maxInModel[1] - minInModel[1])*0.5 + minInModel[1];
    center[2] = (maxInModel[2] - minInModel[2])*0.5 + minInModel[2];
    sphereRadius = diameter/2;
}

bool Object3D::checkSphere(Ray& ray)
{
    Vector3d vVector = center-ray.startPoint;
    double v = vVector.dot(ray.dirVector);
    double csq = vVector.dot(vVector);
    double dsq = sphereRadius*sphereRadius - (csq - v*v);
    if(dsq <= 0)
    {
        return false;
    }

    double d = sqrt(dsq);
    ray.t = (v-d);
    return true;
}

bool Object3D::checkIntersection(Plane& plane, Ray& ray)
{
    Vector3d a = objectMatrix.col(plane.point1-1).head<3>();
    Vector3d b = objectMatrix.col(plane.point2-1).head<3>();
    Vector3d c = objectMatrix.col(plane.point3-1).head<3>();
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
            return true;
        }
    }
    return false;
}

