CC=g++
CLI=-lcrypto -lboost_serialization

test: Node.o 
	$(CC) test.cpp Node.o $(CLI) -o test.o
Node.o: Node.h md5.h 
	$(CC) -c Node.cpp -o Node.o
clean:
	rm *.o

