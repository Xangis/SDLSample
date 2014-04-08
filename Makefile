# Main executable file
PROGRAM = SDLSample

# Directory containing include files.
INCLUDEDIR = /usr/include/SDL2/

# Object files
OBJECTS = Main.o

# Library directory.
LIBDIR = /usr/lib/

CXX = g++

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c -I$(INCLUDEDIR) -o $@ $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $(PROGRAM) -L$(LIBDIR) $(OBJECTS) -lSDL2 -lSDLmain -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lrt

clean:
	rm -f *.o $(PROGRAM)
