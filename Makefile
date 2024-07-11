server: main.o
	g++ -o server main.o

main.o: src/main.cpp
	g++ -c src/main.cpp

clean:
	rm *.o
	rm server