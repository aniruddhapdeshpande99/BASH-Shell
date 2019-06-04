
output: shell.o
	g++ shell.o -o output

shell.o: shell.c
	gcc -c shell.c

clean:
	rm *.o output