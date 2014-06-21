
#LDFLAGS =  -L/usr/X11R6/lib -lglut -lGLU -lGLEW -lGL -lXi -lXext -lX11 -lpthread -lm
LDFLAGS =  -L/usr/X11R6/lib -lglut -lGLU -lGL -lXext -lX11 -lpthread -lm

all:
	g++ Test.cpp $(LDFLAGS)
