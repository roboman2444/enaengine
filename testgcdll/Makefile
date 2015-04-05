CC = gcc
LDFLAGS = -shared
CFLAGS = -fPIC -Wall -O3 -fstrict-aliasing -march=native
OBJECTS = gc.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

gamecode.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../$@ $(LDFLAGS)
valgrind: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing -D DEBUGTIMESTEP
valgrind: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../gamecode.so $(LDFLAGS)
debug: CFLAGS = -fPIC -Wall -O0 -g -fstrict-aliasing
debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o ../gamecode.so $(LDFLAGS)
clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
