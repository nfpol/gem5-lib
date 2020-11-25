all:  dijkstra_small-disable-no dijkstra_small

dijkstra_small-disable-no: dijkstra_small-disable-no.c Makefile
	gcc -static dijkstra_small-disable-no.c  arm_v8.c -O3 -o dijkstra_small-disable-no

dijkstra_small: dijkstra_small.c Makefile
	gcc -static dijkstra_small.c  arm_v8.c -O3 -o dijkstra_small 

clean:
	rm -rf dijkstra_small-disable-no dijkstra_small output*
