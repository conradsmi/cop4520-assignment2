#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>

pthread_t *threads;
int *queue;
atomic_int queue_i, turn;
int num_threads, verbose, guests_spawned;

void *guest(void *t)
{
    int id = t;
    queue[queue_i++] = id;

    // wait in queue until it's our turn
    while (id != queue[turn]);

    // stare at the vase and leave
    if (verbose)
        printf("Guest %d has looked at the vase.\n", id);
    turn++;

    return NULL;
}

int main(int argc, char **argv)
{
    num_threads = atoi(argv[1]);
    verbose = atoi(argv[2]);
    threads = calloc(num_threads, sizeof(pthread_t));
    queue = calloc(num_threads, sizeof(int));
    queue_i = 0;
    turn = 0;

    // create threads
    printf("Spawning guests...\n");
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, guest, (void *)i);
    }

    // join threads
    printf("The guests have begun.\n");
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("All of the guests have looked at the vase at least once.\n");

    free(threads);
    free(queue);

    return 0;
}