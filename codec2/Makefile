CXX = gcc
INCLUDE = /Users/keith/Documents/business/crypto/portaudio/include/
PORTAUDIOLIB = /Users/keith/Documents/business/crypto/portaudio/lib/.libs/ 
CODEC2LIB = /Users/keith/Documents/business/crypto/codec2-dev/src/.libs/ 
RM = rm

all: main

main:
	$(CXX) -o main main.c -I$(INCLUDE) -L$(PORTAUDIOLIB) -L$(CODEC2LIB) -lportaudio -lcryptopp -lcodec2

clean:
	rm ./main
