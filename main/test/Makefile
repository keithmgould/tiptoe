all: suite

suite: Main.cpp ./UnitTest++/libUnitTest++.a
	g++ Main.cpp ./UnitTest++/libUnitTest++.a -o suite -I ./UnitTest++/src/

clean:
	rm -f ./suite
