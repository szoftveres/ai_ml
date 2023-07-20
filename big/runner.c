#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>


#include "runner.h"
#include "aigame.h"


pthread_mutex_t     instance_mem_mutex;


static int sock;

static pthread_t job_dispatch_thrd_info;
static pthread_t job_collect_thrd_info;
static queue_head_t    job_tx_queue;
static queue_head_t    job_collect_queue;
static queue_head_t    job_rx_queue;


typedef struct thrdarg_s {
    run_mode_t runmode;
    int nconn;
    int cconn[8];
    int sconn;
} thrdarg_t;


static thrdarg_t thrd_args;


int
think_and_decide (gamefield_t *game, instance_t *instance) {
    int a;
    int h;
    int i;
    int max = 0;

    int hidden1[HIDDEN1];
    int hidden2[HIDDEN2];
    int action[ACTIONS];

    uint8_t *field = malloc(instance->n_input * sizeof(uint8_t));
    if (!field) {
        exit(1);
    }
    lookahead(game, instance->n_input, field);

    /* Evaluating */
    for (h = 0; h != HIDDEN1 / 2; h++) {
        int weight = 0;
        int f;
        for (f = 0; f != FIELDS; f++) {
            weight += (field[f] ? 1 : 0) * ((int)instance->nn_1[f*(h+1)]);
        }
        hidden1[h] = weight / FIELDS;
    }
    for (h = HIDDEN1 / 2; h != HIDDEN1 ; h++) {
        int weight = 0;
        int f;
        for (f = 0; f != FIELDS; f++) {
            weight += (field[f] ? 0 : 1) * ((int)instance->nn_1[f*(h+1)]);
        }
        hidden1[h] = weight / FIELDS;
    }

    free(field);

    for (h = 0; h != HIDDEN2; h++) {
        int weight = 0;
        int f;
        for (f = 0; f != HIDDEN1; f++) {
            weight += (hidden1[f] ? 1 : 0) * ((int)instance->nn_h[f*(h+1)]);
        }
        hidden2[h] = weight / HIDDEN1;
    }
    for (a = 0; a != ACTIONS; a++) {
        int weight = 0;
        for (h = 0; h != HIDDEN2; h++) {
            weight += hidden2[h] * ((int)instance->nn_2[h*(a+1)]);
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

    return i;
}


void*
play_game (void* args) {

    int g;
    int i;

    instance_t* instance = (instance_t*)args;

    instance->score = 0;

    for (g = 0; g != instance->rounds; g++) {
        gamefield_t *game = init_game(instance->difficulty);

        for (i = 0; i != 36000; i++) {
            if (instance->graphics) {
                draw_field(game);
                usleep(50000u);
                fflush(0);
            }
            if (shift(game)) {
                /* The fitness function (with extra rewards) */
                instance->score += i;
                break;
            }
            switch (think_and_decide(game, instance)) {
              case 0: up(game); break;
              case 2: down(game); break;
            }
        }
        free(game);
    }
    instance->score /= instance->rounds;
    queue_push(&job_collect_queue, (queue_item_t*)instance);
    return NULL;
}


int
setup_client (char* ip) {
    int conn;
    struct sockaddr_in addr;

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = inet_addr(ip);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("socket error %s\n", ip);
        exit(1);
    }

    conn = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (conn < 0) {
        printf("Can't connect %s\n", ip);
        exit(conn);
    }
    printf("Connected to server %s (conn:%d)\n", ip, sock);
    return sock;
}


int
setup_server (void) {
    struct sockaddr_in addr;
    pid_t pid = 0;
    int client;

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("socket error\n");
        exit(1);
    }
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    listen(sock, 1);

    do {
        if (pid) {
            printf("Client connected (conn:%d, pid:%d) \n", client, pid);
            waitpid(pid, NULL, 0);
        }
        client = accept(sock, NULL, NULL);
        pid = fork();
    } while (pid);
    return (client);
}


instance_t *
receive_data (int conn) {
    instance_t* instance;
    int n = sizeof(instance_t);
    int r;
    char* buf;

    instance = create_instance();
    buf = (char*)instance;

    while (n) {
        r = recv(conn, buf, n, MSG_WAITALL);
        if (r < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                printf("%s\n", strerror(errno));
                exit(1);
            }
        } else if (!r) {
            printf("%s\n", strerror(errno));
            exit(0);
        } else {
            n -= r;
            buf += r;
        }
    }
    return instance;
}


void
send_data (int conn, instance_t *instance) {
    int n = sizeof(instance_t);
    int r;
    char* buf;

    buf = (char*)instance;

    while (n) {
        r = send(conn, buf, n, MSG_WAITALL);
        if (r < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                printf("%s\n", strerror(errno));
                exit(1);
            }
        } else if (!r) {
            printf("%s\n", strerror(errno));
            exit(0);
        } else {
            n -= r;
            buf += r;
        }
    }
    destroy_instance(instance);
}



void*
job_dispatch_thrd(void* arg) {
    thrdarg_t *thrdarg;
    instance_t* instance;
    int con_idx = 0;

    thrdarg = (thrdarg_t*)arg;
    while (1) {
        if (thrdarg->runmode == SERVER) {
            instance = receive_data(thrdarg->sconn);
            pthread_create(&(instance->thrd_info), NULL, play_game, instance);
        } else {
            instance = (instance_t*)queue_pop(&job_tx_queue);
            if (thrdarg->runmode == CLIENT) {
                send_data(thrdarg->cconn[con_idx], instance);
            } else {
                pthread_create(&(instance->thrd_info), NULL, play_game, instance);
            }
        }
        con_idx = (con_idx+1 == thrdarg->nconn) ? 0 : con_idx+1;
    }
    return NULL;
}


void*
job_collect_thrd(void* arg) {
    thrdarg_t *thrdarg;
    instance_t* instance;
    int con_idx = 0;

    thrdarg = (thrdarg_t*)arg;
     while (1) {
        if (thrdarg->runmode == CLIENT) {
            instance = receive_data(thrdarg->cconn[con_idx]);
        } else {
            instance = (instance_t*)queue_pop(&job_collect_queue);
            pthread_join(instance->thrd_info, NULL);
        }
        if (thrdarg->runmode == SERVER) {
            send_data(thrdarg->sconn, instance);
        } else {
            queue_push(&job_rx_queue, (queue_item_t*)instance);
        }
        con_idx = (con_idx+1 == thrdarg->nconn) ? 0 : con_idx+1;
    }
    return NULL;
}

void
job_dispatcher_init (run_mode_t mode, char* ips[], int nconn) {
    thrd_args.runmode = mode;
    thrd_args.nconn = nconn;
    int i;

    pthread_mutex_init(&instance_mem_mutex, NULL);

    queue_init(&job_tx_queue);
    queue_init(&job_collect_queue);
    queue_init(&job_rx_queue);

    switch(mode) {
      case NORMAL:
        thrd_args.nconn = 1;
        break;
      case CLIENT: {
            int i;
            if (!nconn) {
                printf("No IPs given\n");
                exit(1);
            }
            for (i = 0; i != thrd_args.nconn; i++) {
                thrd_args.cconn[i] = setup_client(ips[i]);
            }
        }
        break;
      case SERVER:
        thrd_args.nconn = 1;
        thrd_args.sconn = setup_server();
        break;
    }

    for (i = 0; i != thrd_args.nconn; i++) {
        thrdarg_t* arg = (thrdarg_t*)malloc(sizeof(thrdarg_t));
        memcpy(arg, &thrd_args, sizeof(thrdarg_t));
        arg->cconn[0] = arg->cconn[i];
        arg->nconn = 1;
        pthread_create(&job_collect_thrd_info, NULL, job_collect_thrd, arg);
    }
    pthread_create(&job_dispatch_thrd_info, NULL, job_dispatch_thrd, &thrd_args);
}

void
enqueue_job (instance_t *instance) {
    queue_push(&job_tx_queue, (queue_item_t*)instance);
}


instance_t *
get_back_results (void) {
    return (instance_t*)queue_pop(&job_rx_queue);
}


instance_t *
create_instance (void) {
    instance_t *instance;

    pthread_mutex_lock(&instance_mem_mutex);
    instance = (instance_t*)malloc(sizeof(instance_t));
    pthread_mutex_unlock(&instance_mem_mutex);
    if (!instance) {
        printf("Malloc error create_instance\n");
        exit(1);
    }
    return instance;
}

void
destroy_instance (instance_t *instance) {
    pthread_mutex_lock(&instance_mem_mutex);
    free(instance);
    pthread_mutex_unlock(&instance_mem_mutex);
}

