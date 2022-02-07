#ifndef DEQUE_H
#define DEQUE_H

#include <stdint.h>

#include "linkable.h"

typedef struct deque {
	struct linkable *current;
	struct linkable *tail;
} deque;

deque *deque_new(void);

linkable *deque_peek_front(deque *queue);
linkable *deque_peek_back(deque *queue);

linkable *deque_next(deque *queue);
linkable *deque_prev(deque *queue);

linkable *deque_pop_front(deque *queue);

void deque_push_front(deque *queue, linkable *node);
void deque_push_back(deque *queue, linkable *node);

void deque_clear(deque *queue, bool also_free);
void deque_free(deque *queue, bool free_nodes);

#endif

