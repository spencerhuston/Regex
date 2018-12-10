COMP = g++ -std=c++11
CFLAG = -c -g -Wall 

all: regex

regex: main.cpp Regex.o
	$(COMP) -o regex -g main.cpp Regex.o

Regex.o: Regex.h Regex.cpp Node.h Edge.h
	$(COMP) $(CFLAG) Regex.cpp Node.h Edge.h

clean:
	rm -rf regex *.o
