CXX = g++
INCLUDE = /Users/keith/Documents/business/crypto/portaudio/include/
LIB = /Users/keith/Documents/business/crypto/portaudio/lib/.libs/
RM = rm

all: tiptoe

tiptoe: tiptoe.cpp
	$(CXX) -o tiptoe tiptoe.cpp -I$(INCLUDE) -L$(LIB) -lportaudio -lcryptopp

clean:
	rm ./tiptoe
