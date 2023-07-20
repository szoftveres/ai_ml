#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

void
queue_init (queue_head_t* queue) {
    memset(queue, 0, sizeof(queue_head_t));
    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->cond), NULL);
}


/* Push front */
void
queue_push (queue_head_t* queue, queue_item_t* item) {
    pthread_mutex_lock(&(queue->mutex));
    item->prev = NULL;
    if (!queue->tail) {
        queue->tail = item;
    }
    item->next = queue->head;
    queue->head = item;
    if (item->next) {
        item->next->prev = item;
    }
    queue->size += 1;
    pthread_cond_signal(&(queue->cond));
    pthread_mutex_unlock(&(queue->mutex));

}


/* Pop back, blocking */
queue_item_t*
queue_pop (queue_head_t* queue) {
    queue_item_t* item;

    pthread_mutex_lock(&(queue->mutex));
    while (!queue->size) {
        pthread_cond_wait(&(queue->cond), &(queue->mutex));
    }
    assert(queue->head);
    assert(queue->tail);

    item = queue->tail;
    queue->tail = item->prev;
    if (queue->tail) {
        queue->tail->next = NULL;
    } else {
        queue->head = NULL;
    }
    queue->size -= 1;
    pthread_mutex_unlock(&(queue->mutex));
    return (item);
}


