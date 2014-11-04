CC = gcc
LDFLAGS = -shared
CFLAGS = -fPIC -Wall -O3 -fstrict-aliasing
OBJECTS = gc.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

gamecode.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	cp $@ ../
valgrind: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing -D DEBUGTIMESTEP
valgrind: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o gamecode.so $(LDFLAGS)
	cp gamecode.so ../
debug: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing
debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o gamecode.so $(LDFLAGS)
	cp gamecode.so ../
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
