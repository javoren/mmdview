
#LDFLAGS =  -L/usr/X11R6/lib -lglut -lGLU -lGLEW -lGL -lXi -lXext -lX11 -lpthread -lm
LDFLAGS =  -L/usr/X11R6/lib -lglut -lGLU -lGL -lXext -lX11 -lpthread -lm

OBJS = Test.o Mesh.o
CC = g++

all: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $<

