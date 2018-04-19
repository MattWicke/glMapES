CC = g++
CFLAGS = -g `pkg-config --cflags opencv` 
#CFLAGS = -O3 `pkg-config --cflags opencv` 
#LDFLAGS = -lGL -lGLU -lglut -lIL -lILU -lGLEW -L/usr/lib/nvidia-331 -std=c++11 
LDFLAGS = -lGL -lGLU -lglut  -lGLEW -L/usr/lib/nvidia-331 -std=c++11 -lmosquitto
LDFLAGS += `pkg-config --libs opencv`


#OBJS specifies which files to compile as part of the proect
OBJS = main.cpp util.cpp surface.cpp ogl.cpp mqtt.cpp

#OBJ_NAME specifies th ename of the executable
OBJ_NAME = map

#this is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $(OBJ_NAME)
