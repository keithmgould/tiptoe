PORTAUDIO_INCLUDE = /Users/keith/source/portaudio/include/

vpath %.cpp ../utilities/src

main: main.o hamming.o unpacker.o convert.o untranscoder.o printer.o
	g++ -o main main.o hamming.o unpacker.o convert.o untranscoder.o printer.o -I$(EIGEN_HOME) -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_INCLUDE) -I$(EIGEN_HOME)

transmit.o: transmit.cpp
	g++ -c $<

unpacker.o: unpacker.cpp
	g++ -c $< -I$(EIGEN_HOME)

untranscoder.o: untranscoder.cpp
	g++ -c $<

hamming.o: hamming.cpp
	g++ -c $< -I$(EIGEN_HOME)

convert.o: convert.cpp
	g++ -c $<

printer.o: printer.cpp
	g++ -c $<

clean:
	rm ./main

