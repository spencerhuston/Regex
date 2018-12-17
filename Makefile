COMP = g++ -std=c++11
CFLAG = -c -g -Wall 

all: regex

regex: main.cpp Regex.o Format.o Analyzer.o
	$(COMP) -o regex -g main.cpp Regex.o Format.o Analyzer.o

Regex.o: Regex.h Regex.cpp State.h Edge.h Fragment.h
	$(COMP) $(CFLAG) Regex.cpp State.h Edge.h Fragment.h

Format.o: Format.h Format.cpp
	$(COMP) $(CFLAG) Format.cpp

Analyzer.o: Analyzer.h Analyzer.cpp
	$(COMP) $(CFLAG) Analyzer.cpp 

clean:
	rm -rf regex *.o *.gch tests/*.out

