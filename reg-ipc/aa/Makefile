FILE1 = random.c
FILE2 = file.c
all: clean server random

random: ${FILE1} Makefile
	gcc -O3 ${FILE1} -g  -o random  -lrt -lm

server: ${FILE2} Makefile
	gcc file.c -o server -lrt


clean:
	rm -rf random server
