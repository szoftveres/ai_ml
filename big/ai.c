#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "runner.h"

static instance_t population[POP_SIZE];

void
init_population (void) {
    int i;
    for (i = 0; i != POP_SIZE; i++) {
        int n;
        population[i].score = 0;
        for (n = 0; n != (FIELDS * HIDDEN1); n++) {
            population[i].nn_1[n] = rand() % 256;
        }
        for (n = 0; n != (HIDDEN1 * ACTIONS); n++) {
            population[i].nn_2[n] = rand() % 256;
        }
    }
}

int
next_gen (void) {
    int i;
    int it;
    int p;
    int rc;
    instance_t winners[REPR_NUM];

    /* Select the highest performers */
    for (i = 0; i != REPR_NUM; i++) {
        int max_score = 0;
        int fit;
        for (p = 0; p != POP_SIZE; p++) {
            if (population[p].score > max_score) {
                max_score = population[p].score;
                fit = p;
            }
        }
        if (!i) {
            rc = max_score;
        }
        population[fit].score = 0;

        /* Save the winners to a temporary storage */
        memcpy(&winners[i], &population[fit], sizeof(instance_t));
    }

    for (p = 0; p != POP_SIZE; p++) {
        memcpy(&population[p], &winners[p % REPR_NUM], sizeof(instance_t));
    }

    /* Mutate */
    it = 0;
    for (i = 0; i != 2; i++ ) {
        for (p = 0; p != (FIELDS * HIDDEN1); p++) {
            population[it++].nn_1[p] = rand() % 256;
        }
        for (p = 0; p != (HIDDEN1 * ACTIONS); p++) {
            population[it++].nn_2[p] = rand() % 256;
        }
    }
    return rc;
}



int
main (int argc, char **argv) {
    time_t t;
    int gen = 0;
    int difficulty = 3;
    int running;
    int c;


    char*   ips[8];
    int     nconn = 0;

    run_mode_t mode = NORMAL;

    while((c = getopt(argc, argv, "sc:")) != -1) {
        switch (c) {
          case 'c':
            mode = CLIENT;
            ips[nconn] = optarg;
            printf("client mode (%s)\n", optarg);
            nconn++;
            break;
          case 's':
            mode = SERVER;
            printf("server mode\n");
            break;
          default:
            printf("No IPs were given\n");
            exit(1);
        }
    }

    srand((unsigned) time(&t));

    job_dispatcher_init(mode, ips, nconn);

    init_population();

    while (1) {
        int max;
        int p;
        running = 0;
        for (p = 0; p != POP_SIZE; p++) {
            instance_t* instance = create_instance();
            memcpy(instance, &(population[p]), sizeof(instance_t));
            instance->p = p;
            instance->rounds = 4;
            instance->difficulty = difficulty;
            instance->graphics = 0; //(!(gen % 50)) && (p == POP_SIZE-1);
            instance->n_input = FIELDS;
            instance->n_output = ACTIONS;
            enqueue_job(instance);
            running++;
        }
        while (running) {
            instance_t* instance = get_back_results();
            population[instance->p].score = instance->score;
            destroy_instance(instance);
            running--;
        }
        max = next_gen();
        printf("generation: %d (psize:%d), level:%d, hi-score: %d\n", gen, POP_SIZE, difficulty, max);
        usleep(50000u);
        if (max > 2500) {
            printf("Increasing difficulty level!\n"); sleep(2);
            difficulty++;
        }
        gen++;
    }
    return 0;
}

