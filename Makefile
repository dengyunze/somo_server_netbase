########################################################################
# eg. make MODE=STATIC/DYNAMIC
# STATIC---.a   DYNAMIC---.so
########################################################################
CC = g++
INC = -I ./ -I../../lib3rd/libuv/include -I ../base/include -I ../protocol -I ../syscomm -I ../../lib3rd/jsoncpp/include -I ../../lib3rd/libevent/include
LIB = ../../lib3rd/jsoncpp/lib/libjsoncpp.a
FLAG_SUFFIX = -std=c++0x -Wall -O2 -Wno-deprecated

ifeq ($(MODE),DYNAMIC)
	TARGET = libnetbase.so
	AR_ = g++ -fPIC -shared -o
	FLAG = $(FLAG_SUFFIX) -fPIC
else
	TARGET = libnetbase.a
	AR_ = ar rcs
	FLAG = $(FLAG_SUFFIX)
endif

all:$(TARGET)
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

%.o : %.cpp %.h
	$(CC) -c $(FLAG) $< -o $@ $(INC)

$(TARGET) : $(OBJECTS)
	$(AR_) $(TARGET) $(OBJECTS)

.PHONY : clean

clean:
	rm $(OBJECTS)
	rm $(TARGET)
