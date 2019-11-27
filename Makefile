CC=g++
CLI=-lcrypto -lboost_serialization

test: Node.o PastryNode.o utils.o
	$(CC) test.cpp Node.o PastryNode.o utils.o $(CLI) -o test.o

utils.o:utils.h

PastryNode.o:Node.o PastryNode.h

Node.o:Node.h md5.h
	$(CC) -c Node.cpp -o Node.o

clean:
	rm *.o

