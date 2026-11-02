
.PHONY: clean Procedural OOP

all: Procedural OOP


Procedural:	Procedural/main.exe
	./Procedural/main.exe

Procedural/main.exe: Procedural/main.c Procedural/solution.c Procedural/solution.h
	gcc -std=c17 -Wall -Werror -pedantic Procedural/main.c Procedural/solution.c -o Procedural/main.exe

OOP: OOP/main.exe
	./OOP/main.exe

OOP/main.exe: OOP/main.cpp
	g++ -std=c++20 -Wall -Werror -pedantic OOP/main.cpp -o OOP/main.exe

clean:
	rm Procedural/main.exe OOP/main.exe --Force