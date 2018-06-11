LFLAGS=-lglfw -lGL -lGLEW -lboost_system -lpthread -ldl -lm -lboost_thread
FLAGS=-g -o obj/

all : client

obj/misc.o : misc.cpp misc.hpp
	g++ $(FLAGS)misc.o --std=c++11 -c misc.cpp $(LFLAGS)

obj/vboindexer.o : vboindexer.cpp vboindexer.hpp
	g++ $(FLAGS)vboindexer.o --std=c++11 -c vboindexer.cpp $(LFLAGS)

obj/mesh.o : mesh.cpp mesh.hpp vboindexer.hpp
	g++ $(FLAGS)mesh.o --std=c++11 -c mesh.cpp $(LFLAGS)

obj/light.o : light.cpp light.hpp
	g++ $(FLAGS)light.o --std=c++11 -c light.cpp $(LFLAGS)

obj/texture.o : texture.cpp texture.hpp misc.hpp
	g++ $(FLAGS)texture.o --std=c++11 -c texture.cpp $(LFLAGS)

obj/shader.o : shader.cpp shader.hpp entity.hpp camera.hpp
	g++ $(FLAGS)shader.o --std=c++11 -c shader.cpp $(LFLAGS)

obj/camera.o : camera.cpp camera.hpp entity.hpp
	g++ $(FLAGS)camera.o --std=c++11 -c camera.cpp $(LFLAGS)

obj/entity.o : entity.cpp entity.hpp texture.hpp mesh.hpp vboindexer.hpp light.hpp
	g++ $(FLAGS)entity.o --std=c++11 -c entity.cpp $(LFLAGS)

obj/window.o : window.cpp window.hpp entity.hpp light.hpp mesh.hpp shader.hpp
	g++ $(FLAGS)window.o --std=c++11 -c window.cpp $(LFLAGS)

obj/renderer.o : renderer.cpp renderer.hpp light.hpp entity.hpp camera.hpp mesh.hpp texture.hpp 
	g++ $(FLAGS)renderer.o --std=c++11 -c renderer.cpp $(LFLAGS)

obj/main.o : main.cpp camera.hpp entity.hpp shader.hpp window.hpp texture.hpp vboindexer.hpp mesh.hpp  misc.hpp light.hpp renderer.hpp
	g++ $(FLAGS)main.o --std=c++11 -c main.cpp $(LFLAGS)

client : obj/main.o obj/vboindexer.o obj/mesh.o obj/texture.o obj/light.o obj/shader.o obj/camera.o obj/entity.o obj/window.o obj/misc.o  obj/renderer.o
	g++ $(FLAGS) --std=c++11 -o client obj/main.o obj/vboindexer.o obj/mesh.o obj/texture.o obj/light.o obj/shader.o obj/camera.o obj/entity.o obj/window.o obj/misc.o  obj/renderer.o $(LFLAGS)
