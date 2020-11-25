FILE1 = basicmath_large-v2.c rad2deg.c cubic.c isqrt.c arm_v8.c
FILE2 = basicmath_small-v2.c   rad2deg.c  cubic.c   isqrt.c arm_v8.c

all:  basicmath_large-v2 basicmath_small-v2

basicmath_small-v2: ${FILE2} Makefile
	gcc -static -O3 ${FILE2} -o basicmath_small-v2 -lm
basicmath_large-v2: ${FILE1} Makefile
	        gcc -static -O3 ${FILE1} -o basicmath_large-v2 -lm

clean:
	rm -rf basicmath_large-v2 basicmath_small-v2 output*
