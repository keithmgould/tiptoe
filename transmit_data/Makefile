INCLUDE = /Users/keith/source/portaudio/include/

vpath %.cpp ../utilities/src

main: main.o transmit.o transcode.o convert.o
	g++ -o main main.o transmit.o transcode.o convert.o -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(INCLUDE)

transmit.o: transmit.cpp
	g++ -c $<

transcode.o: transcode.cpp
	g++ -c $<

convert.o: convert.cpp
	g++ -c $<

clean:
	rm ./main
