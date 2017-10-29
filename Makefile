

all: raytracer

raytracer: Eigen/ main.cpp Face.h Ray.h Sphere.h Camera3D.cpp Camera3D.h Object3D.cpp Object3D.h Scene3D.cpp Scene3D.h 
	g++ -Wall -g -std=c++11 -I. *.cpp -o raytracer	
clean:
	rm raytracer
	make
tar:
	tar -cvf pa2.tar *.cpp *.h Makefile readme.md Eigen/
