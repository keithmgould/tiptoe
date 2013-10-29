PORTAUDIO_INCLUDE = /Users/keith/source/portaudio/include/

vpath %.cpp ../utilities/src

main: main.o hamming.o unpacker.o converter.o untranscoder.o printer.o demodulator.o delta_finder.o extractor.o
	g++ -o main main.o hamming.o unpacker.o converter.o untranscoder.o printer.o demodulator.o delta_finder.o extractor.o -I$(EIGEN_HOME) -lportaudio
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(PORTAUDIO_INCLUDE) -I$(EIGEN_HOME)

unpacker.o: unpacker.cpp
	g++ -c $< -I$(EIGEN_HOME)

untranscoder.o: untranscoder.cpp
	g++ -c $<

hamming.o: hamming.cpp
	g++ -c $< -I$(EIGEN_HOME)

converter.o: converter.cpp
	g++ -c $<

printer.o: printer.cpp
	g++ -c $<

demodulator.o: demodulator.cpp
	g++ -c $<

delta_finder.o: delta_finder.cpp
	g++ -c $<

extractor.o: extractor.cpp
	g++ -c $<

clean:
	rm ./main

