
#Use GNU compiler
cc = gcc -g
CC = g++ -g

all: consumer producer

shell: consumer.cpp producer.cpp
	$(CC) -c consumer.cpp -o consumer
	$(CC) -c producer.cpp -o producer
clean:
	rm -f producer consumer
