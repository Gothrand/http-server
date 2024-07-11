server: main.o
	g++ -o server main.o

main.o: src/main.cpp src/utils.h src/server.h
	g++ -c src/main.cpp

clean:
	rm *.o
	rm server