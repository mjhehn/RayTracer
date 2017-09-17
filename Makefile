

all: modeltoworld

modeltoworld: Eigen/ main.cpp Point3D.h Plane.h Object3D.cpp Object3D.h Scene3D.cpp Scene3D.h  
	g++ -Wall -g -std=c++11 -I. *.cpp -o2 modeltoworld 	
clean:
	rm modeltoworld
tar:
	tar -cvf pa1.tar *.cpp *.h makefile
