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
    MM = MM.inverse().eval();
    MM.transposeInPlace();
    normalsMatrix = MM*normalsMatrix;
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
        while(buffer[0] == '#' && !fin.fail()){
            comments.append("\n"+buffer);
            std::getline(fin, buffer);
        }
        bool materialSet = false;
        string currentMaterial = "";
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
            else if(ssbuffer == "vn")
            {
                double x = 0;
                double y = 0;
                double z = 0;
                ss>>x>>y>>z;
                normalsMatrix.conservativeResize(NoChange, normalsMatrix.cols()+1);
                normalsMatrix.col(normalsMatrix.cols()-1) << x, y, z, 1;
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
                else{buffer = std::regex_replace(buffer, std::regex(R"([/]{1})"), " / "); }//remove /'s from buffer lines, which are used for normal vectors for faces
                
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
                if(materialSet)
                {
                    faces.push_back(Face(textures, normals, tempPoints, currentMaterial));
                }
                else{
                    faces.push_back(Face(textures, normals, tempPoints));
                }
                
            }
            else if(ssbuffer == "usemtl")
            {
                ss>>currentMaterial;
                materialSet = true;
            }
            else if(ssbuffer == "mtllib")
            {
                generateMaterialsList(ss);
                currentMaterial = mat[0].name;
                materialSet = true;
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
    faces.clear();
}

void Object3D::print()
{
    std::cout<<objectName<<"\n"<<comments<<"\n";
    for (int i = 0; i < objectMatrix.cols(); ++i)   
    {
        std::cout<<"v "<<objectMatrix.col(i).row(0)<<" "<<objectMatrix.col(i).row(1)<<" "<<objectMatrix.col(i).row(2)<<"\n";
    }

    for(const auto j: faces)   //c++11 range based looping
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

    for(const auto j: faces)   //c++11 range based looping
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

Matrix4d Object3D::buildTransformationMatrix(const double tx, const double ty, const double tz)
{
	Matrix4d T = Matrix4d::Identity();
	T.col(3) << tx, ty, tz, 1;
	return T;
}

Matrix4d Object3D::buildScaleMatrix(const double scale)
{
	Matrix4d S = Matrix4d::Identity();
	S = S*scale;
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

    double xMid= (maxInModel[0] - minInModel[0])*0.5;
    double yMid = (maxInModel[1] - minInModel[1])*0.5;
    double zMid = (maxInModel[2] - minInModel[2])*0.5;

    double majorLength = std::max(std::max(xMid, yMid), zMid);
    double minorLength = std::min(std::max(xMid, yMid), std::max(zMid, yMid));
    sphereRadius = sqrt(majorLength*majorLength + minorLength*minorLength);

    center[0] =  xMid+ minInModel[0];
    center[1] =  yMid+ minInModel[1];
    center[2] =  zMid+ minInModel[2];
}

bool Object3D::checkSphere(const Ray& ray)
{
    Vector3d vVector = center-ray.startPoint;
    double v = vVector.dot(ray.dirVector);
    double csq = vVector.dot(vVector);
    double dsq = pow(sphereRadius, 2.0) - (csq - pow(v, 2.0));
    return !(dsq < 0.001);
}

bool Object3D::checkIntersection(const int& i, Ray& ray)
{
    if(checkSphere(ray))
    {
        Vector3d a = objectMatrix.col(faces[i].point1-1).head<3>();
        Vector3d b = objectMatrix.col(faces[i].point2-1).head<3>();
        Vector3d c = objectMatrix.col(faces[i].point3-1).head<3>();
        Matrix3d M;

        M.col(0) = a-b;     //cramer's rule begins
        M.col(1) = a-c;
        M.col(2) = ray.dirVector;

        Matrix3d Mi = M;
        Vector3d Y = a - ray.startPoint;
        Vector3d swap;
        
        float mDeterminant = M.determinant();
        if(mDeterminant != 0)
        {
            swap = Mi.col(0);   //opimization for swap rather than a full copy
            Mi.col(0) = Y;
            float beta = Mi.determinant()/mDeterminant;
            if(beta>=0)
            {
                Mi.col(0) = swap;
                swap = Mi.col(1);
                Mi.col(1) = Y;
                float gamma = Mi.determinant()/mDeterminant;
                if( (beta+gamma)<=1.001 && gamma>= 0)
                {
                    Mi.col(1) = swap;
                    Mi.col(2) = Y;
                    float t = Mi.determinant()/mDeterminant;
                    if( t > 0)
                    {
                        ray.t = t;
                        ray.beta = beta;
                        ray.gamma = gamma;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Object3D::generateMaterialsList(stringstream& ss)
{
    string filename;
    ss>>filename;
    ifstream fin;
    fin.open(filename);
    string temp = "";
    string matName = "";
    Vector3d Ka = Vector3d::Zero(3);
    Vector3d Kd = Vector3d::Zero(3);
    Vector3d Ks = Vector3d::Zero(3);
    Vector3d Kr;
    Kr[0] = Kr[1] = Kr[2] = 1.0; //update if needed. defaulted for A4;
    Vector3d Ko = Kr;
    double r;
    double g;
    double b;
    double phong = 16;
    while(!fin.fail())
    {
        fin>>temp;
        if(temp == "newmtl")
        {
            fin>>matName;
        }
        else if(temp == "Ka")
        {
            fin>>r; fin>>g; fin>>b;
            Ka<<r,g,b;
        }
        else if(temp == "Kd")
        {
            fin>>r; fin>>g; fin>>b;
            Kd<<r,g,b;
        }
        else if(temp == "Ks")
        {
            fin>>r; fin>>g; fin>>b;
            Ks<<r,g,b;
        }
        else if(temp == "Kr")
        {
            fin>>r; fin>>g; fin>>b;
            Kr<<r,g,b;
        }
        else if(temp == "illum")
        {
            mat.push_back(Material(matName, phong, Ka, Kd, Ks, Kr, Ko));
        }
        else if(temp == "Ns")
        {
            fin>>phong;
        }
    }
    fin.close();  
}
