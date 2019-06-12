
all: fractal

fractal: fractal.o bitmap.o
	gcc fractal.o bitmap.o -o fractal -lpthread -lm

fractal.o: fractal.c
	gcc --std=c99 -Wall -g -c fractal.c -o fractal.o

bitmap.o: bitmap.c
	gcc --std=c99 -Wall -g -c bitmap.c -o bitmap.o

clean:
	rm -f fractal.o bitmap.o fractal