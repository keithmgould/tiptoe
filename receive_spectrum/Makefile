CXX = g++
INCLUDE = /Users/keith/Documents/business/crypto/portaudio/include/
PORTAUDIOLIB = /Users/keith/Documents/business/crypto/portaudio/lib/.libs/ 
CODEC2LIB = /Users/keith/Documents/business/crypto/codec2-dev/src/.libs/ 
RM = rm

all: main

main:
	$(CXX) -o main main.cpp -I$(INCLUDE) -L$(PORTAUDIOLIB) -L$(CODEC2LIB) -lfftw3 -lportaudio -lcryptopp -lcodec2

clean:
	rm ./main

