#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define EAST 0
#define WEST 1

int east_count = 0;
int west_count = 0;
pthread_mutex_t bridge_mutex;
pthread_cond_t bridge_cond;

void *car(void *direction_ptr)
{
    int direction = (int)direction_ptr;
    pthread_mutex_lock(&bridge_mutex);
    if (direction == EAST)
    {
        east_count++;
        while (west_count > 0 || (east_count > 1))
        {
            pthread_cond_wait(&bridge_cond, &bridge_mutex);
        }
    }
    else
    {
        west_count++;
        while (east_count > 0 || (west_count > 1))
        {
            pthread_cond_wait(&bridge_cond, &bridge_mutex);
        }
    }
    printf("Carro cruzando de %s\n", direction == EAST ? "Este a Oeste" : "Oeste a Este");
    sleep(1);
    if (direction == EAST)
    {
        east_count--;
    }
    else
    {
        west_count--;
    }
    pthread_cond_broadcast(&bridge_cond);
    pthread_mutex_unlock(&bridge_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <número de autos de Este> <número de autos de Oeste>\n", argv[0]);
        return -1;
    }

    int east_cars = atoi(argv[1]);
    int west_cars = atoi(argv[2]);

    pthread_t threads[east_cars + west_cars];
    int directions[east_cars + west_cars];

    pthread_mutex_init(&bridge_mutex, NULL);
    pthread_cond_init(&bridge_cond, NULL);

    for (int i = 0; i < east_cars; i++)
    {
        directions[i] = EAST;
        pthread_create(&threads[i], NULL, car, &directions[i]);
    }

    for (int i = 0; i < west_cars; i++)
    {
        directions[east_cars + i] = WEST;
        pthread_create(&threads[east_cars + i], NULL, car, &directions[east_cars + i]);
    }

    for (int i = 0; i < east_cars + west_cars; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&bridge_mutex);
    pthread_cond_destroy(&bridge_cond);

    return 0;
}