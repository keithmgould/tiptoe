vpath %.cpp ../utilities/src

main: main.o transmitter.o hamming.o packer.o converter.o transcoder.o printer.o
	g++ -o main main.o transmitter.o hamming.o packer.o converter.o transcoder.o printer.o -I$(EIGEN_HOME) -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_HOME)/include/ -I$(EIGEN_HOME)

transmitter.o: transmitter.cpp
	g++ -c $<

packer.o: packer.cpp
	g++ -c $< -I$(EIGEN_HOME)

transcoder.o: transcoder.cpp
	g++ -c $<

hamming.o: hamming.cpp
	g++ -c $< -I$(EIGEN_HOME)

converter.o: converter.cpp
	g++ -c $<

printer.o: printer.cpp
	g++ -c $<

clean:
	rm ./main
