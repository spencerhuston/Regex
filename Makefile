COMP = g++ -std=c++11
CFLAG = -c -g -Wall 

all: regex

regex: main.cpp Regex.o
	$(COMP) -o regex -g main.cpp Regex.o

Regex.o: Regex.h Regex.cpp
	$(COMP) $(CFLAG) Regex.cpp

clean:
	rm -rf regex *.o
