########################################################################
# eg. make MODE=STATIC/DYNAMIC
# STATIC---.a   DYNAMIC---.so
########################################################################
CC = g++
INC = -I ./ -I include -I 3rd/libuv/include
LIB = 
FLAG_SUFFIX = -std=c++0x -Wall -O2 -Wno-deprecated
MODE = SYNAMIC

ifeq ($(MODE),DYNAMIC)
	TARGET = libnetbase.so
	AR_ = g++ -fPIC -shared -o
	FLAG = $(FLAG_SUFFIX) -fPIC
else
	TARGET = libnetbase.a
	AR_ = ar rcs
	FLAG = $(FLAG_SUFFIX)
endif

SOURCES=$(wildcard ./src/*.cpp)
DIR=$(notdir $(SOURCES))
OBJECTS=$(patsubst %.cpp,%.o,$(DIR) )

all:$(TARGET)
#OBJECTS = $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

%.o : src/%.cpp src/%.h
	$(CC) -c $(FLAG) $< -o $@ $(INC)

$(TARGET) : $(OBJECTS)
	$(AR_) $(TARGET) $(OBJECTS)

.PHONY : clean

clean:
	rm $(OBJECTS)
	rm $(TARGET)
