#OBJS specifies which files to compile as part of the project
OBJS = platform.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I..\SDL2-2.0.14\i686-w64-mingw32\include\SDL2 -I..\SDL2_image-2.0.5\i686-w64-mingw32\include\SDL2 -I..\SDL2_ttf-2.0.15\i686-w64-mingw32\include\SDL2 -I..\SDL2_mixer-2.0.4\i686-w64-mingw32\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L..\SDL2-2.0.14\i686-w64-mingw32\lib -L..\SDL2_image-2.0.5\i686-w64-mingw32\lib -L..\SDL2_ttf-2.0.15\i686-w64-mingw32\lib -L..\SDL2_mixer-2.0.4\i686-w64-mingw32\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
#COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = platform.exe

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)