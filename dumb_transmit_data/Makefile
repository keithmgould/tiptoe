vpath %.cpp ../utilities/src

main: main.o dumb_transmitter.o converter.o printer.o
	g++ -o main main.o dumb_transmitter.o converter.o printer.o -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_HOME)/include/

dumb_transmitter.o: dumb_transmitter.cpp
	g++ -c $<

converter.o: converter.cpp
	g++ -c $<

printer.o: printer.cpp
	g++ -c $<

clean:
	rm ./main
