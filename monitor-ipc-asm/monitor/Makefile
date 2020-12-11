all:  clean monitor

monitor: monitor.c Makefile
	gcc -static monitor.c -O3 -o monitor 

clean:
	rm -rf monitor out*
	touch output-reg.dat
