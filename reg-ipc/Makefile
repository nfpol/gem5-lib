FILE1 = random.c

all: random

random: ${FILE1} Makefile
	gcc -static -O3 ${FILE1} monitor.c  arm_v8.c -o random -lm

clean:
	rm -rf random
