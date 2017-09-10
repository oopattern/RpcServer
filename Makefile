#makefile bin
BIN_NAME=hello
BIN_DIR=bin
INC_DIR = server
SRC_DIR = server
OBJ_PATH = obj
CCREACTOR_LIB_PATH = component
CCREACTOR_LIB = ccreactor


#NET_PROTOCOL := -D__USE_QEV1_PROTOCOL__ 
LIB_MYSQL :=  OFF
LIB_MEMCACHE := OFF

#GAME_DEFINES :=  -D__XXFLAG__

###############
include ./makefile.base
# g++ -o bin/hello -Icomponent/jsoncpp/include  server/main.cc -Lcomponent/lib -lccreactor
	
	
	
