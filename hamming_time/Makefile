vpath %.cpp ../utilities/src

main: main.o hamming.o
	g++ -o main main.o hamming.o -I$(EIGEN_HOME)
	rm *.o

main.o: main.cpp
	g++ -c main.cpp -I$(EIGEN_HOME)

hamming.o: hamming.cpp
	g++ -c $< -I$(EIGEN_HOME)

clean:
	rm ./main
