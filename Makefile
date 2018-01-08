

all: raytracer

raytracer: Eigen/ main.cpp Face.h Ray.h Sphere.h Camera3D.cpp Camera3D.h Object3D.cpp Object3D.h Scene3D.cpp Scene3D.h 
	g++ -Wall -g -std=c++11 -I. *.cpp -O3 -o raytracer	
clean:
	rm raytracer
	make
tar:
	tar -cvf pa5.tar *.cpp *.h Makefile readme.md Eigen/ driver02.txt driver02.ppm driver01.txt driver01.ppm mnt.obj mnt.mtl tree.obj tree.mtl present.obj present.mtl
