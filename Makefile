all: model bloc4

model: model.cpp
	g++ -c model.cpp
bloc4: bloc4.cc
	g++ -o bloc4.exe bloc4.cc model.o -lGLU -lglut -lGL 
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
	rm *.exe 