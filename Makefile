
all: fractal

fractal: fractal.o bitmap.o
	gcc fractal.o bitmap.o -o fractal -lpthread -lm

fractal.o: fractal.c
	gcc --std=c99 -Wall -g -c fractal.c -o fractal.o

bitmap.o: bitmap.c
	gcc --std=c99 -Wall -g -c bitmap.c -o bitmap.o

clean:
	rm -f fractal.o bitmap.o fractal

all: fractal-seq

fractal-seq: fractal-seq.o bitmap.o
	gcc fractal-seq.o bitmap.o -o fractal-seq -lpthread -lm

fractal-seq.o: fractal-seq.c
	gcc --std=c99 -Wall -g -c fractal-seq.c -o fractal-seq.o

bitmap.o: bitmap.c
	gcc --std=c99 -Wall -g -c bitmap.c -o bitmap.o

clean:
	rm -f fractal-seq.o bitmap.o fractal-seq