#OBJS specifies which files to compile as part of the project
OBJS = breakout.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -Wall -ansi -g -D_GNU_SOURCE=1 -D_REENTRANT -std=c90 -pedantic

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = breakout

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
