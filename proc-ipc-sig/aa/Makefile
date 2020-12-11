FILE1 = random.c

all: clean random monitor join
	
random: ${FILE1} Makefile
	gcc -O3 ${FILE1} -g  -o random  -lrt -lm
	
monitor: monitor.c Makefile
	gcc monitor.c  arm_v8.c -O3 -o monitor -lrt -lm -ggdb

join:${FILE1} Makefile
	gcc -O3 ${FILE1} monitor.c  arm_v8.c -g  -o exp  -lrt -lm -ggdb

clean:
	rm -rf random  output* monitor
