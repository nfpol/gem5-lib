all:  monitor

monitor: monitor.c Makefile
	gcc -static monitor.c  arm_v8.c -O3 -o monitor 

clean:
	rm -rf monitor out*
	touch output-reg.dat
