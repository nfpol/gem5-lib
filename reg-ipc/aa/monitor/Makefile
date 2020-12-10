all:  clean monitor

monitor: monitor.c Makefile
	gcc monitor.c  arm_v8.c -O3 -o monitor -lrt -lm -ggdb

clean:
	rm -rf monitor out*
	touch output-reg.dat
