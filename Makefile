CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW `sdl-config --libs` -lm -ldl
CFLAGS = -Wall -O3 `sdl-config --cflags` -fstrict-aliasing -fprofile-use
OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o sdlmanager.o particlemanager.o framebuffermanager.o texturemanager.o modelmanager.o vbomanager.o console.o viewportmanager.o entitymanager.o gamecodemanager.o hashtables.o renderqueue.o worldmanager.o mathlib.o lightmanager.o textmanager.o ubomanager.o glstates.o animmanager.o cvarmanager.o stringlib.o lighttile.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

enaengine: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	cd testgcdll && $(MAKE)

debug:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE) debug

valgrind:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -D DEBUGTIMESTEP
valgrind: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE) valgrind
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f enaengine
	@rm -f enaengine-debug
	@rm -f enaengine-valgrind
	@rm -f enaengine-profile
	@rm -f ./*.gcda
	
profile:	CFLAGS= -Wall -O3 -g `sdl-config --cflags`-fstrict-aliasing -fprofile-generate
profile: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	@echo
	@echo You must run make clean before trying to compile any other option than profile
