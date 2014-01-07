CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW `sdl-config --libs` -lm -lSDL_image
CFLAGS = -Wall -O3 `sdl-config --cflags`
OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o sdlmanager.o particlemanager.o framebuffermanager.o texturemanager.o modelmanager.o vbomanager.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

enaengine: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
debug: $(OBJECTS)
	CFLAGS = -Wall -O0 -g `sdl-config --cflags`
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
	@rm -f enaengine
	@rm -f enaengine-debug
