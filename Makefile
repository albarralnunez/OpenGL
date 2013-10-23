all: 

sessio3: sessio3.cc
	g++ -o sessio3 sessio3.cc model.o -lGLU -lglut -lGL 
sphere: sphere_pres.cc
	g++ -o sphere sphere_pres.cc model.o -lGLU -lglut -lGL 
ejem: ejem.cc
	g++ -o ejem ejem.cc model.o -lGLU -lglut -lGL 
def: def.cc
	g++ -o def def.cc model.o -lGLU -lglut -lGL 
main: main.cpp
	g++ -o main main.cpp model.o -lGLU -lglut -lGL 
clean:
	rm -f test2 sessio3
