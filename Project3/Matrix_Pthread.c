#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define M 2000
#define K 2000
#define N 2000

typedef int matrixA[M][K];
typedef int matrixB[K][N];
typedef int matrixC[M][N];
matrixA A;
matrixB B;
matrixC C;
pthread_t peer[M];

void peer_multi(int *arg){
	int row = *arg;
	int column, position;
	for (column = 0; column < N; ++column) {
		C[row][column] = 0;
		for (position = 0; position < K; ++position) {
			C[row][column] = C[row][column] + 
			(A[row][position] * B[position][column]);
		}
	}
}

int main(){
	int row, column, id, i;
	time_t start, finish;
	double duration;
	
	srand(time(0));
	for (row = 0; row < M; ++row){
		for (column = 0; column < K; ++column){
			A[row][column] = rand() % 100;
		}
	}
	for (row = 0; row < K; ++row){
		for (column = 0; column < N; ++column){
			B[row][column] = rand() % 100;
		}
	}
	
	start = time(NULL);
	for (row = 0; row < M; ++row){
		id = row;
		int *arg = (int *)malloc(sizeof(int));
		*arg = row;
		pthread_create(&(peer[id]), NULL, (void *)peer_multi, arg);
	}
	for (i = 0; i < M; ++i){
		pthread_join(peer[i], NULL);
	}
	finish = time(NULL);
	duration = (double)(finish - start);
	/*
	printf("The resulting matrix C is:\n");
	for (row = 0; row < M; ++row){
		for (column = 0; column < N; ++column){
			printf("%d ",C[row][column]);
		}
		printf("\n");
	}
	*/
	printf("Multiplication by multiple threads...\n");
	printf("Duration: %.1f seconds\n", duration);
	return 0;
}
