

all: raytracer

raytracer: Eigen/ main.cpp Face.h Ray.h Sphere.h Camera3D.cpp Camera3D.h Object3D.cpp Object3D.h Scene3D.cpp Scene3D.h 
	g++ -Wall -g -std=c++11 -I. *.cpp -O3 -o raytracer	
clean:
	rm raytracer
	make
tar:
	tar -cvf pa3.tar *.cpp *.h Makefile readme.md Eigen/
