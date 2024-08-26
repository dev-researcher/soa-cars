#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define EAST 1
#define WEST 2
#define MAX_CARS 10

int east_count = 0;
int west_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define DEBUG 1

void *car(void *direction_ptr) {
    int direction = (intptr_t)direction_ptr;

    if (DEBUG) {
        printf("Carro llegó desde %s\n", direction == EAST ? "Este" : "Oeste");
    }

    pthread_mutex_lock(&mutex);
    if (direction == EAST) {
        east_count++;
        while (west_count > 0 || east_count > 1) {
            if (DEBUG) {
                printf("Carro esperando para cruzar de Este a Oeste\n");
            }
            pthread_cond_wait(&cond, &mutex);
        }
    } else {
        west_count++;
        while (east_count > 0 || west_count > 1) {
            if (DEBUG) {
                printf("Carro esperando para cruzar de Oeste a Este\n");
            }
            pthread_cond_wait(&cond, &mutex);
        }
    }

    if (DEBUG) {
        printf("Carro cruzó de %s\n", direction == EAST ? "Este a Oeste" : "Oeste a Este");
    }

    if (direction == EAST) {
        east_count--;
    } else {
        west_count--;
    }

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <carros_este> <carros_oeste>\n", argv[0]);
        return 1;
    }

    int cars_east = atoi(argv[1]);
    int cars_west = atoi(argv[2]);

    pthread_t cars[MAX_CARS];

    for (int i = 0; i < cars_east; i++) {
        pthread_create(&cars[i], NULL, car, (void *)(intptr_t)EAST);
    }

    for (int i = cars_east; i < cars_east + cars_west; i++) {
        pthread_create(&cars[i], NULL, car, (void *)(intptr_t)WEST);
    }

    for (int i = 0; i < cars_east + cars_west; i++) {
        pthread_join(cars[i], NULL);
    }

    return 0;
}