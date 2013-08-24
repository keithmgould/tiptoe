CXX = g++
INCLUDE = /Users/keith/Documents/business/crypto/portaudio/include/
LIB = /Users/keith/Documents/business/crypto/portaudio/lib/.libs/
RM = rm

all: main

main: main.cpp
	$(CXX) -o main main.cpp -I$(INCLUDE) -L$(LIB) -lportaudio -lcryptopp

clean:
	rm ./main
