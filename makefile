# Building Command: mingw32-make
# Command to run: app
# Key to hide console: -mwindows

INC = -I"E:\Libs\glu\include"

all: main.o
	g++ main.o -lopengl32 -lwinmm -lgdi32 -lglu32 \
	-static-libgcc -Wl,-Bstatic -lstdc++ \
	-lwinpthread -o app.exe

main.o: main.cpp
	g++ -c $(INC) main.cpp
