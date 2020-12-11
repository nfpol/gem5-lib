FILE1 = random.c
all: clean random monitor

random: ${FILE1} Makefile
	gcc -static -O3 ${FILE1}  -g  -o random -lm

monitor: ./monitor/monitor.c Makefile
	gcc -static ./monitor/monitor.c  ./monitor/arm_v8.c -O3 -o ./monitor/monitor 

clean:
	rm -rf random out*  ./monitor/out*
	touch output.dat
	touch ./monitor/output-reg.dat
