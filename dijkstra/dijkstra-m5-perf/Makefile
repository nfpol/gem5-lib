all:  dijkstra_large dijkstra_small dijkstra_small1

dijkstra_large: dijkstra_large.c Makefile
	gcc -static dijkstra_large.c  -O3 -o dijkstra_large

dijkstra_small: dijkstra_small.c Makefile
	gcc -static dijkstra_small.c  -O3 -o dijkstra_small 

dijkstra_small1: dijkstra_small1.c Makefile
	gcc -static dijkstra_small1.c  -O3 -o dijkstra_small1

clean:
	rm -rf dijkstra_large dijkstra_small dijkstra_small1  output*
