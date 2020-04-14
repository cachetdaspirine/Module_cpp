CC =g++ #compilateur
VERSION = -std=c++1y #derniere version de c++
OPT = -O3 #optimisation de ouf
SharedLib = lib.so

SRC=$(wildcard *.cpp)

OBJ=$(SRC:.cpp=.o) #convertie la liste de .cpp en une liste de .o

HEAD=$(filter-out main.h System_c.h , $(SRC:.cpp=.h)) #converti tout les .cpp en .h sauf main.cpp et certains autres...

all : $(SharedLib)

$(SharedLib) : $(OBJ)
	     $(CC) -shared -Wl,-soname,$(SharedLib) -o $(SharedLib) $(OBJ)

.cpp.o:
	$(CC) $(Version) -fPIC $(OPT) -c $< -o $@

.PHONY: clean

clean:
	rm -rf *.o *~ $(SharedLib)
