#ifndef __RUNNER_H__
#define __RUNNER_H__

#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>
#include "queue.h"


#define FIELDS (3 + 5 + 7 + 9 + 11 + 13 + 15 + 17 + 19)
#define HIDDEN1 (27)
#define ACTIONS (3)

//static int hidden_layers[] = {18, 18};

#define POP_SIZE (((FIELDS * HIDDEN1) + (HIDDEN1 * ACTIONS)) * 2)
#define REPR_NUM  (POP_SIZE / 12)


typedef enum {
    NORMAL,
    CLIENT,
    SERVER,
} run_mode_t;



typedef struct instance_s {
    QUEUE_HEADER
    int score;
    pthread_t thrd_info;
    int difficulty;
    int graphics;
    int p;
    int rounds;

    int n_input;
    int n_output;

    uint8_t nn_1[FIELDS * HIDDEN1];
    uint8_t nn_2[HIDDEN1 * ACTIONS];
} instance_t;

void job_dispatcher_init (run_mode_t mode, char* ips[], int nconn);
void enqueue_job (instance_t *instance);
instance_t * get_back_results (void);

instance_t *create_instance (void);
void destroy_instance (instance_t *instance);

#endif
