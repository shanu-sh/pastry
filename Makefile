CC=g++
CLI=-lcrypto -lboost_serialization

test: Node.o utils.o
	$(CC) test.cpp Node.o utils.o $(CLI) -o test.o

utils.o: utils.h

Node.o: Node.h md5.h
	$(CC) -c Node.cpp -o Node.o
clean:
	rm *.o

