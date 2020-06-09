########################################################################
# eg. make MODE=STATIC/DYNAMIC
# STATIC---.a   DYNAMIC---.so
########################################################################
CC = g++
INC = -I ./ -I include -I 3rd/libuv/include
LIB = 
FLAG_SUFFIX = -std=c++0x -Wall -O2 -Wno-deprecated
MODE = STATIC

ifeq ($(MODE),DYNAMIC)
	TARGET = build/libnetbase.so
	AR_ = g++ -fPIC -shared -o
	FLAG = $(FLAG_SUFFIX) -fPIC
else
	TARGET = build/libnetbase.a
	AR_ = ar rcs
	FLAG = $(FLAG_SUFFIX)
endif

SOURCES=$(wildcard ./src/*.cpp)
DIR=$(notdir $(SOURCES))
OBJECTS=$(patsubst %.cpp, build/objs/%.o, $(DIR) )

all:$(TARGET)
#OBJECTS = $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/objs%.o : src/%.cpp src/%.h
	$(CC) -c $(FLAG) $< -o $@ $(INC)

$(TARGET) : $(OBJECTS)
	$(AR_) $(TARGET) $(OBJECTS)

.PHONY : clean

clean:
	rm $(OBJECTS) -f
	rm $(TARGET) -f 
