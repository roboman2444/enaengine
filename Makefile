CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW `sdl-config --libs` -lm -lSDL_image
CFLAGS = -Wall -O3 `sdl-config --cflags`
OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o sdlmanager.o particlemanager.o framebuffermanager.o texturemanager.o modelmanager.o vbomanager.o console.o viewportmanager.o entitymanager.o gamecodemanager.o hashtables.o renderqueue.o worldmanager.o mathlib.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

enaengine: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
debug:	CFLAGS= -Wall -O0 -g `sdl-config --cflags`
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
	@rm -f enaengine
	@rm -f enaengine-debug
