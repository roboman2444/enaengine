CC = gcc
LDFLAGS = -lGL -lGLU -lGLEW -lglfw -lm -ldl
##LDFLAGS = -lGL -lGLU -lGLEW -lglfw -lm -ldl -lode -lvulkan
CFLAGS = -Wall -Ofast -fstrict-aliasing -fprofile-use -march=native
##OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o sdlmanager.o particlemanager.o framebuffermanager.o texturemanager.o modelmanager.o vbomanager.o console.o viewportmanager.o entitymanager.o gamecodemanager.o hashtables.o renderqueue.o worldmanager.o mathlib.o lightmanager.o textmanager.o ubomanager.o glstates.o animmanager.o cvarmanager.o stringlib.o lighttile.o rendermodel.o forwardlights.o physics.o drawbb.o glfwmanager.o
OBJECTS = enaengine.o matrixlib.o shadermanager.o filemanager.o glmanager.o particlemanager.o framebuffermanager.o texturemanager.o modelmanager.o vbomanager.o console.o viewportmanager.o entitymanager.o gamecodemanager.o hashtables.o renderqueue.o worldmanager.o mathlib.o lightmanager.o textmanager.o ubomanager.o glstates.o animmanager.o cvarmanager.o stringlib.o lighttile.o rendermodel.o forwardlights.o physics.o drawbb.o glfwmanager.o vkmanager.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

enaengine: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	cd testgcdll && $(MAKE)

debug:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -march=native
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE) debug

valgrind:	CFLAGS= -Wall -O0 -g `sdl-config --cflags` -fstrict-aliasing -march=native -D DEBUGTIMESTEP 
valgrind: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE) valgrind



vulkan:	LDFLAGS= -lGL -lGLU -lGLEW -lglfw -lm -ldl -lvulkan
vulkan:	CFLAGS = -Wall -Ofast `sdl-config --cflags` -fstrict-aliasing -fprofile-use -march=native -D VULKAN_COMPILE
vulkan: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE)

ode:	LDFLAGS= -lGL -lGLU -lGLEW -lglfw -lm -ldl -lode
ode:	CFLAGS = -Wall -Ofast `sdl-config --cflags` -fstrict-aliasing -fprofile-use -march=native -D ODE_COMPILE
ode: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE)

vkode:	LDFLAGS= -lGL -lGLU -lGLEW -lglfw -lm -ldl -lode -lvulkan
vkode:	CFLAGS = -Wall -Ofast `sdl-config --cflags` -fstrict-aliasing -fprofile-use -march=native -D ODE_COMPILE -D VULKAN_COMPILE
vkode: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	cd testgcdll && $(MAKE)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
	cd testgcdll && $(MAKE) clean
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f enaengine
	@rm -f enaengine-debug
	@rm -f enaengine-valgrind
	@rm -f enaengine-profile
	@rm -f enaengine-vulkan
	@rm -f enaengine-ode
	@rm -f enaengine-vkode
	@rm -f ./*.gcda
	
profile:	CFLAGS= -Wall -Ofast -g `sdl-config --cflags` -fstrict-aliasing -fprofile-generate -march=native
profile: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o enaengine-$@ $(LDFLAGS)
	@echo
	@echo You must run make clean before trying to compile any other option than profile
