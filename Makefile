COMP = g++ -std=c++11
CFLAG = -c -g -Wall 

all: regex

regex: main.cpp Regex.o Format.o
	$(COMP) -o regex -g main.cpp Regex.o Format.o

Regex.o: Regex.h Regex.cpp State.h Edge.h Fragment.h
	$(COMP) $(CFLAG) Regex.cpp State.h Edge.h Fragment.h

Format.o: Format.h Format.cpp
	$(COMP) $(CFLAG) Format.cpp

clean:
	rm -rf regex *.o
