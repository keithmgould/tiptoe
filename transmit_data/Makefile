CXX = g++
INCLUDE = /Users/keith/source/portaudio/include/
LIB = /Users/keith/source/portaudio/lib/.libs/
all: main

vpath %.cpp ../utilities/src

main: main.cpp transmit.cpp transcode.cpp convert.cpp
	$(CXX) -o main $< -I$(INCLUDE) -L$(LIB) -lportaudio

clean:
	rm ./main
