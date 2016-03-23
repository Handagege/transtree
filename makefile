ifeq ($(ARCH),i686)
    CC=gcc -m32
    CFLAGS=-Wall -shared -fPIC -g -O3 
else
    CC=gcc
    CFLAGS=-Wall -shared -fPIC -g -O3 
endif

EROS_LIB_ROOT_PATH = ../eros/lib/

INC_PATH=-I../depend/xbson/include -I../depend/feature_pool_bin/include/ -I../depend/uthash/ -I../depend/mapdb/include/ -I../depend/woo/include -I/usr/local/include/json -I/usr/local/include/hiredis/ -I../include/ -I../include/db/ -I../include/utility -I../include/work -I../include/algorithm
INC_PATH += -I$(EROS_LIB_ROOT_PATH)/jsoncpp-0.6.0/include

LIB_LINK=-L../depend/xbson/lib -L../depend/feature_pool_bin/lib/ -L../depend/mapdb/lib/ -L../depend/woo/lib -L../depend/Json/lib -L/usr/local/lib -lxbson -lmemcached -lapifp -lmapdb -lcurl -lssl -ljson -lwoo -lstdc++ -lz -lhiredis -lpthread -ljsoncpp

#LIB += -lxbson -ljson -lwoo -lstdc++ -lz -lpthread

SRC_PATH = ./src/
OBJS = $(SRC_PATH)mtreeWorkInterface.o
OBJS += $(SRC_PATH)MultiWayTree.o
OBJS += ../src/ini_file.o

ALL_BIN = ../lib/mtreeWorkInterface.so


.PHONY: all     

all: $(ALL_BIN)

../lib/mtreeWorkInterface.so : $(OBJS)
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_LINK)


%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -I$(SRC_PATH) -pg

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg


.PHONY: clean

clean: 
	rm -rf $(ALL_BIN)
	rm -rf ./src/*.o

