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

typedef struct instance_s {
    int score;
    uint8_t nn[FIELDS * ACTIONS];
} instance_t;

static instance_t population[POP_SIZE];

int
look_think_act (instance_t *instance) {
    int a;
    int i;
    int max = 0;
    int rc = 0;
    uint8_t field[FIELDS];
    int action[ACTIONS];

    /* Information gathering */
    lookahead(field);

    /* Evaluating */
    for (a = 0; a != ACTIONS; a++) {
        int weight = 0;
        int f;
        for (f = 0; f != FIELDS; f++) {
            weight += ((int)field[f]) * ((int)instance->nn[f*(a+1)]);
        }
        action[a] = weight;
    }

    /* Decision making */
    for (a = 0; a != ACTIONS; a++) {
        if (action[a] > max) {
            max = action[a];
            i = a;
        }
    }

    /* Acting */
    switch (i) {
        case 0: up(); rc = 1; break;
        case 2: down(); rc = 1; break;
    }
    return rc;
}

void
init_population (void) {
    int i;
    for (i = 0; i != POP_SIZE; i++) {
        int n;
        population[i].score = 0;
        for (n = 0; n != (FIELDS * ACTIONS); n++) {
            population[i].nn[n] = 0;
        }
        for (n = 0; n != (FIELDS); n++) {
            /* Initializing for don't move */
            population[i].nn[n*2] = 255;
        }
    }
}

int
next_gen (void) {
    int i;
    int p;
    int rc;
    instance_t winners[REPR_NUM];

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
        population[p].nn[rand() % (FIELDS * ACTIONS)] = rand() % 256;
    }

    return rc;
}

int
main (void) {
    time_t t;
    int gen = 0;
    int difficulty = 1;

    srand((unsigned) time(&t));

    init_population();

    while (1) {
        int max;
        int p;
        for (p = 0; p != POP_SIZE; p++) {
            int i;
            init_game(difficulty);
            int moves = 0;
            for (i = 0; i != 36000; i++) {
                if (shift()) {
                    /* The fitness function (with extra rewards) */
                    population[p].score = i;
                    population[p].score += ((i - moves) / 4);
                    break;
                }
                if (((!(gen % 100)) && (p >= (POP_SIZE / 2)) && p < (POP_SIZE / 2) + 3)) {
                    draw_field();
                    usleep(50000u);
                    fflush(0);
                }

                moves += look_think_act(&population[p]);
            }
        }
        max = next_gen();
        printf("generation: %d, level:%d, hi-score: %d\n", gen, difficulty, max);
        if (max > 1500) {
            printf("Increasing difficulty level!\n"); sleep(2);
            difficulty++;
        }
        gen++;
    }
    return 0;
}

