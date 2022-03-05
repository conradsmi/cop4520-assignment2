#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_t *threads;
int num_threads, eaten_count, summoned_count, guest_summoned;
char done, cupcake_uneaten, guest_inlabyrinth;

void *guest(void *t)
{
    int i = t;
    int i_have_eaten = 0;

    while (!done) {
        // wait to be summoned
        usleep(1);
        if (guest_summoned == i) {

            // we are the leader
            if (i == 1) {
                if (!cupcake_uneaten) {
                    // note that cupcake was eaten
                    eaten_count++;
                    // fetch another cupcake from servant
                    cupcake_uneaten = 1;
                }
                if (eaten_count >= num_threads - 1) {
                    // it's the leaders turn to eat since they are the last one
                    eaten_count++;
                    done = 1;
                }
            }
            // we are a regular guest, check if we've eaten yet
            else if (cupcake_uneaten && !i_have_eaten) {
                cupcake_uneaten = 0;
                i_have_eaten = 1;
            }
            // cupcake must be eaten and/or we've had it already,
            // let leader increment his count when he reenters

            // leave labyrinth and let another guest in
            guest_summoned = 0;
            guest_inlabyrinth = 0;
        }
    }

    return NULL;
}

void *minotaur(void *t)
{
    while (!done) {
        // wait until the guest in the labyrinth is out
        usleep(1);
        if (!guest_inlabyrinth) {
            // summon another guest
            guest_inlabyrinth = 1;
            guest_summoned = rand() % num_threads + 1;
            summoned_count++;
        }
    }

    // it's technically fine if someone ate multiple cupcakes, though this shouldn't happen
    if (eaten_count >= num_threads) {
        // quotes because The Minotaur is speaking in first person. this is a very important distinction to note.
        printf("\"The Minotaur is satisfied with your answer.\"\n");
    // this is also impossible afaik
    } else {
        printf("\"The Minotaur is not satisfied with your answer. I am disappointed.\"\n");
    }

    return NULL;
}

int main(int argc, char **argv)
{
    num_threads = atoi(argv[1]);
    threads = calloc(num_threads + 1, sizeof(pthread_t));
    cupcake_uneaten = 1;
    eaten_count = 0;
    done = 0;
    guest_summoned = 0;
    guest_inlabyrinth = 0;
    summoned_count = 0;

    // create threads
    printf("Spawning minotaur and guests...\n");
    pthread_create(&threads[0], NULL, minotaur, NULL);
    for (int i = 1; i <= num_threads; i++) {
        pthread_create(&threads[i], NULL, guest, (void *)i);
    }

    // join threads
    printf("The guests have begun.\n");
    for (int i = 0; i <= num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);

    return 0;
}