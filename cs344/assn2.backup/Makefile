all: boalm.adventure

boalm.adventure: boalm.adventure.o
	gcc -Wall -o boalm.adventure boalm.adventure.o

boalm.adventure.o: boalm.adventure.c
	gcc -c boalm.adventure.c

clean:
	rm boalm.adventure.o boalm.adventure
