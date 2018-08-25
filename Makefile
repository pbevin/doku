CFLAGS=-g -O2 -Wall

doku: picosat.o sudoku.o
	$(CC) $^ -o $@

.PHONY: clean
clean:
	rm *.o doku
