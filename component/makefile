TARGET_PATH=lib
TARGET=libccreactor.a
OBJ_DIR=obj



ifeq ($(MAKECMDGOALS), )
$(error you must select lib name! cc or net (e.g. make cc))
endif

.PHONY : cc net clean a 

cc:
	rm -rf lib/*
	rm -rf  obj/*
	make -f makefile.json   all
	make -f makefile.core   all
	make -f makefile.ar     all

net:
	rm -rf lib/*
	rm -rf  obj/*
	make -f makefile.json   all
	make -f makefile.core   all
	make -f makefile.ar     all
	
clean:
	rm -rf lib/*
	rm -rf  obj/*

	
a:
	echo   $(LIB_OBJ)   $(OBJ_FILE_PATH)
