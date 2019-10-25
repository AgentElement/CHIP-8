CC=gcc -g

chip8.o: src/chip8.c src/chip8.h
	$(CC) -c src/chip8.c -o chip8.o

chip8: chip8.o
	$(CC) chip8.o -o chip8 

c8test.o: src/tests.c src/chip8.h
	$(CC) -c src/tests.c -o c8test.o 

c8test: chip8.o c8test.o
	$(CC) chip8.o c8test.o -o c8test

clean:
	rm -f *.o c8test chip8
