#include <Object3D.h>

Object3D::Object3D(string filename)
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
        std::stringstream ss;
        while(!fin.fail())
        {
            ss.str(buffer);
            ss>>buffer;
            if(buffer == "v")
            {
                double x = 0;
                double y = 0;
                double z = 0;
                ss>>x>>y>>z;
                Point3D newPoint = Point3D(x,y,z);
                points.push_back(newPoint);
            }
            else if(buffer == "f")
            {
                string buffer2;
                string buffer3;
                ss>>buffer;
                std::size_t result = buffer.find_first_not_of("0123456789");
                if(result != string::npos)
                    buffer.erase(result+1);
                else {break;}

                ss>>buffer2;
                result = buffer2.find_first_not_of("0123456789");
                if(result != string::npos)
                    buffer2.erase(result+1);
                else {break;}

                ss>>buffer3;
                result = buffer3.find_first_not_of("0123456789");
                if(result != string::npos)
                    buffer3.erase(result+1);
                else {break;}

                planes.push_back(Plane(points.at(stoi(buffer)-1), points.at(stoi(buffer2)-1), points.at(stoi(buffer3)-1)));               
            }
            else{;}
            getline(fin, buffer);
            ss.clear();
        } 
        ss.clear();      
    }
    else{};
    fin.close();
}
