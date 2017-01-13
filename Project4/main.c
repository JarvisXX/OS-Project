#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

/* the buffer */
int cnt;
buffer_item buffer[BUFFER_SIZE];
sem_t empty, full;
pthread_mutex_t mutex;

int insert_item(buffer_item item) {
	/* insert item into buffer
	 * return 0 if successful, otherwise
	 * return -1 indicating an error condition */
	if (cnt < BUFFER_SIZE) {
		buffer[cnt++] = item;
		return 0;
	}
	else
		return -1;
}

int remove_item(buffer_item *item) {
	/* remove an object from buffer
	 * placing it in item
	 * return 0 if successful, otherwise
	 * return -1 indicating an error condition */
	if (cnt > 0) {
		*item = buffer[--cnt];
		return 0;
	}
	else
		return -1;
}

void *producer(void *param) {
	buffer_item item;
	
	while (1) {
		/* sleep for a random period of time */
		sleep(rand() % 5);
		/* generate a random number */
		item = rand();
		
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		if (insert_item(item))
			fprintf(stderr, "report error condition\n");
		else
			printf("producer produced %d\n", item);
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
}

void *consumer(void *param) {
	buffer_item item;
	
	while (1) {
		/* sleep for a random period of time */
		sleep(rand() % 5);
		
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		if (remove_item(&item))
			fprintf(stderr, "report error condition\n");
		else
			printf("consumer consumed %d\n", item);
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
}

int main(int argc,  char *argv[]) {
	/* 1. Get command line arguments argv[1], argv[2], argv[3]
	 * 2. Initialize buffer
	 * 3. Create producer thread(s)
	 * 4. Create consumer thread(s)
	 * 5. Sleep
	 * 6. Exit */
	
	int argarray[3];
	if (argc != 4) {
		fprintf(stderr, "error1\n");
		return -1;
	}
	
	argarray[0] = atoi(argv[1]);
	argarray[1] = atoi(argv[2]);
	argarray[2] = atoi(argv[3]);
	
	if (argarray[1] < 1) {
		fprintf(stderr, "error2\n");
		return -1;
	}
	if (argarray[2] < 1) {
		fprintf(stderr, "error3\n");
		return -1;
	}
	pthread_t pid[argarray[1]];
	pthread_t cid[argarray[2]];
	
	srand(time(NULL));
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	pthread_mutex_init(&mutex, NULL);
	cnt = 0;
	int i;
	for (i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = 0;
	}
	
	for (i = 0; i < argarray[1]; ++i) {
		pthread_create(&pid[i], NULL, producer, NULL);
	}
	for (i = 0; i < argarray[2]; ++i) {
		pthread_create(&cid[i], NULL, consumer, NULL);
	}
	for (i = 0; i < argarray[1]; ++i) {
		pthread_join(pid[i], NULL);
	}
	for (i = 0; i < argarray[2]; ++i) {
		pthread_join(cid[i], NULL);
	}
	printf("join done\n");
	
	sleep(argarray[0]);
	return 0;
}
