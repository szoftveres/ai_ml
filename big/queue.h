#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>


#define QUEUE_HEADER                    \
    struct queue_item_s     *next;      \
    struct queue_item_s     *prev;      \


typedef struct queue_item_s {
    QUEUE_HEADER
} queue_item_t;


typedef struct queue_head_s {
    queue_item_t            *head;
    queue_item_t            *tail;
    int                     size;
    pthread_mutex_t         mutex;
    pthread_cond_t          cond;
} queue_head_t;


void
queue_init (queue_head_t* queue);
void
queue_push (queue_head_t* queue, queue_item_t* item);
queue_item_t*
queue_pop (queue_head_t* queue);

#endif


