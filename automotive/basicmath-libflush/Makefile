FILE1 = basicmath_large.c rad2deg.c cubic.c isqrt.c arm_v8.c
FILE2 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c arm_v8.c

all:  basicmath_large basicmath_small

basicmath_small: ${FILE2} Makefile
	gcc -static -O3 ${FILE2} -o basicmath_small -lm
basicmath_large: ${FILE1} Makefile
	        gcc -static -O3 ${FILE1} -o basicmath_large -lm

clean:
	rm -rf basicmath_large basicmath_small output*
