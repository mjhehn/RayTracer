

all: raytracer

raytracer: Eigen/ main.cpp Plane.h Object3D.cpp Object3D.h
	g++ -Wall -g -std=c++11 -I. *.cpp -o raytracer 	
clean:
	rm raytracer
	make
tar:
	tar -cvf pa2.tar *.cpp *.h Makefile readme.md Eigen/
