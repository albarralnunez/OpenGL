all: 

sessio3: sessio3.cc
	g++ -o sessio3.exe sessio3.cc model.o -lGLU -lglut -lGL 
sphere: sphere_pres.cc
	g++ -o sphere.exe sphere_pres.cc model.o -lGLU -lglut -lGL 
ejem: ejem.cc
	g++ -o ejem.exe ejem.cc model.o -lGLU -lglut -lGL 
def: def.cc
	g++ -o def.exe def.cc model.o -lGLU -lglut -lGL 
main: main.cpp
	g++ -o main.exe main.cpp model.o -lGLU -lglut -lGL 
clean: