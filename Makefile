#hello : event.cpp global.cpp server.cpp handle.cpp
#	g++ -o hello event.cpp global.cpp server.cpp handle.cpp
#clean:
#	rm -f *.o hello

hello: main.cc
	g++ -o hello main.cc
clean:
	rm -f *.o hello
