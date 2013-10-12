PORTAUDIO_INCLUDE = /Users/keith/source/portaudio/include/

vpath %.cpp ../utilities/src

main: main.o transmit.o hamming.o transcode.o convert.o
	g++ -o main main.o transmit.o hamming.o transcode.o convert.o -I$(EIGEN_HOME) -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_INCLUDE) -I$(EIGEN_HOME)

transmit.o: transmit.cpp
	g++ -c $<

transcode.o: transcode.cpp
	g++ -c $< -I$(EIGEN_HOME)

hamming.o: hamming.cpp
	g++ -c $< -I$(EIGEN_HOME)

convert.o: convert.cpp
	g++ -c $<

clean:
	rm ./main
