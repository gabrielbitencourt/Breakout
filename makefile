#SOURCE specifies which files to compile as part of the project
SOURCE = breakout.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -Wall -ansi -g -D_GNU_SOURCE=1 -D_REENTRANT -std=c90 -pedantic

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

#BINARIES specifies the name of our exectuable
BINARIES = breakout

#This is the target that compiles our executable
all: $(BINARIES)

breakout: $(SOURCE)
	$(CC) $(SOURCE) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BINARIES)

clean:
	rm -rf *.o *.exe *.bak *.c~ $(SOURCE) core a.out
