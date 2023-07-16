#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include "aigame.h"

#define POP_SIZE (96)
#define REPR_FRACT (8)
#define REPR_NUM (POP_SIZE / REPR_FRACT)

#define FIELDS (48)
#define ACTIONS (3)

static uint8_t field[FIELDS];
static int action[ACTIONS];

typedef struct instance_s {
    int score;
    uint8_t nn_p[FIELDS * ACTIONS];
} instance_t;


static instance_t population[POP_SIZE];
static instance_t winners[REPR_NUM];


void
think (instance_t *instance) {
    int f;
    int a;
    for (a = 0; a != ACTIONS; a++) {
        int weight = 0;
        for (f = 0; f != FIELDS; f++) {
            weight += ((int)field[f]) * ((int)instance->nn_p[f*a]);
        }
        action[a] = weight;
    }
}

void
act (void) {
    int i;
    int max = 0;
    int a = 0;
    for (i = 0; i != ACTIONS; i++) {
        if (action[i] > max) {
            max = action[i];
            a = i;
        }
    }
    switch (a) {
        case 0: up(); break;
        case 2: down(); break;
    }
}


void
init_population (void) {
    int i;
    int n;
    for (i = 0; i != POP_SIZE; i++) {
        population[i].score = 0;
        for (n = 0; n != (FIELDS * ACTIONS); n++) {
            population[i].nn_p[n] = (rand() % 256);
        }
    }
}

int
next_gen (void) {
    int i;
    int p;
    int rc;
    /* Find the highest performers */
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

    /* Reproduce */
    for (p = 0; p != POP_SIZE; p++) {
        memcpy(&population[p], &winners[p % REPR_NUM], sizeof(instance_t));
    }

    /* Mutate */
    for (p = 0; p != (POP_SIZE / 2); p++) {
        population[p].nn_p[rand() % (FIELDS * ACTIONS)] = rand() % 256;
    }

    return rc;
}


int
main (int argc, char** argv) {
    time_t t;
    int gen = 0;
    int p;
    int i;
    int max = 0;

    srand((unsigned) time(&t));

    init_population();

    while (1) {
        for (p = 0; p != POP_SIZE; p++) {
            init_game();
            for (i = 0; i != 36000; i++) {
                shift();
                if (((!(gen % 500)) && (p < 3))) {
                    draw_field();
                    usleep(50000u);
                    fflush(0);
                }
                lookahead(field);

                if (evaluate()) {
                    population[p].score = i;
                    break;
                }
                think(&population[p]);
                act();
                if (i > 35000) {
                    printf("WINNER\n");
                    return 0;
                }
            }
        }
        max = next_gen();
        printf("gen: %d, hi-score: %d\n", gen, max);
        gen++;
    }

    return 0;

}




