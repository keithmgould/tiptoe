PORTAUDIO_INCLUDE = /Users/keith/source/portaudio/include/

vpath %.cpp ../utilities/src

main: main.o printer.o delta_finder.o 
	g++ -o main main.o printer.o delta_finder.o -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_INCLUDE)

printer.o: printer.cpp
	g++ -c $<

delta_finder.o: delta_finder.cpp
	g++ -c $<

clean:
	rm ./main

