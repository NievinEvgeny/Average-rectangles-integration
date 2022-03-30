all: Integration.c
		gcc -fopenmp -Wall -o bin $^ -lm
clean:
		rm bin