CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW `sdl-config --libs`  -lm
CFLAGS = -Wall -O3 `sdl-config --cflags`
OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o sdlmanager.o particlemanager.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

enaengine: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
	@rm -f enaengine
