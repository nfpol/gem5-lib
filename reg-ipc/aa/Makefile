FILE1 = random.c

all: clean random

random: ${FILE1} Makefile
	gcc -static -O3 ${FILE1} -g  -o random -lm -lrt

clean:
	rm -rf random
