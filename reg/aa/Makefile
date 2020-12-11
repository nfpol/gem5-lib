FILE1 = random.c
all: 	clean 	random
	+$(MAKE) -C monitor

random: ${FILE1} Makefile
	gcc -static -O3 ${FILE1}  -g  -o random -lm

clean:
	rm -rf random out*  ./monitor/out* ./monitor/monitor
	touch output.dat
	touch ./monitor/output-reg.dat
