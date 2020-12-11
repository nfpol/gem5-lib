FILE1 = random.c

all: clean random
	+$(MAKE) -C monitor
	
random: ${FILE1} Makefile
	gcc -O3 ${FILE1} -g  -o random  -lrt -lm

clean:
	rm -rf random  output* ./monitor/out* ./monitor/monitor
