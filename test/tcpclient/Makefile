LIB3RD = ../../3rd

CC = g++
INC = -I../../include
STDLIB = -ldl -lz -lcrypto -lrt -ldl -lpthread
LIB = ../../build/libnetbase.a $(LIB3RD)/libuv/lib/libuv.a 

BUILD_TIME = `date "+%Y-%m-%d_%H:%M:%S"`

FLAG_RELEASE = -std=c++0x -Wall -O2 -Wno-deprecated -DBUILD_TIME=\"$(BUILD_TIME)\"
FLAG_DEBUG = -std=c++0x -Wall -g -Wno-deprecated -DBUILD_TIME=\"$(BUILD_TIME)\"
FLAG = $(FLAG_DEBUG)

BIN_DIR = ./
PROCESS = tcpclient

all: $(PROCESS)


$(PROCESS) : $(OBJECT) $(LIB) main.cpp
	$(CC) $(FLAG) -o $@ main.cpp $(INC) $(LIB) $(STDLIB)

.PHONY: deploy
.PHONY: clean

deploy:
	make
	cp $(PROCESS) $(BIN_DIR)

clean :
	rm $(OBJECT)
	rm $(PROCESS)
