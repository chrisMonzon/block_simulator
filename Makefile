CC = g++
ifeq ($(shell uname 2>/dev/null), Darwin)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -DOSX -Wno-deprecated-register -Wno-deprecated-declarations -Wno-shift-op-parentheses
INCFLAGS = -I./glm-0.9.7.1 -I/opt/local/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL -L/opt/homebrew/lib \
  -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
  -lGL -lGLU -lm -lstdc++ -lfreeimage
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES
INCFLAGS = -I./glm-0.9.7.1 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L./lib/nix/ \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lGLEW -lglut -lGLU -lGL -lX11 -lfreeimage
endif

RM = /bin/rm -f
all: startsim
startsim: main.o shaders.o Transform.o inithelpers.o display.o variables.h inithelpers.h shaders.h Transform.h \
	   Geometry.o
	$(CC) $(CFLAGS) -o startsim shaders.o main.o Transform.o Geometry.o inithelpers.o display.o $(INCFLAGS) $(LDFLAGS)
main.o: main.cpp shaders.h Transform.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
shaders.o: shaders.cpp shaders.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c shaders.cpp
inithelpers.o: inithelpers.cpp inithelpers.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c inithelpers.cpp
display.o: display.cpp variables.h Geometry.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c display.cpp
Transform.o: Transform.cpp Transform.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp
Geometry.o: Geometry.cpp Geometry.h Transform.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Geometry.cpp
clean:
	$(RM) *.o startsim *.png


 
