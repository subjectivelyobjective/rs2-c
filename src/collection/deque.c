/*
 * Copyright (C) 2021-2022 subjectivelyobjective
 *
 * This file is part of rs2client317.
 *
 * rs2client317 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * rs2client317 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "collection/deque.h"
#include "collection/linkable.h"

deque *deque_new(void)
{
	deque *queue = malloc(sizeof(deque));

	linkable *_tail = linkable_new(NULL);
	_tail->id = -2;

	if (queue != NULL) {
		queue->tail = _tail;
		queue->tail->next = _tail;
		queue->tail->prev = _tail;
	}

	return queue;
}

linkable *deque_peek_front(deque *queue)
{
	linkable *node = queue->tail->next;

	if (node == queue->tail) {
		queue->current = NULL;
		return NULL;
	}

	queue->current = node->next;
	return node;
}

linkable *deque_peek_back(deque *queue)
{
	linkable *node = queue->tail->prev;

	if (node == queue->tail) {
		queue->current = NULL;
		return NULL;
	}

	queue->current = node->prev;
	return node;
}

linkable *deque_next(deque *queue)
{
	linkable *node = queue->current;

	if (node == queue->tail) {
		queue->current = NULL;
		return NULL;
	}

	queue->current = node->next;
	return node;
}

linkable *deque_prev(deque *queue)
{
	linkable *node = queue->current;

	if (node == queue->tail) {
		queue->current = NULL;
		return NULL;
	}

	queue->current = node->prev;
	return node;
}

linkable *deque_pop_front(deque *queue)
{
	linkable *node = queue->tail->next;

	if (node == queue->tail)
		return NULL;

	linkable_unlink(node);
	return node;
}

void deque_push_front(deque *queue, linkable *node)
{
	if (node->prev != NULL)
		linkable_unlink(node);

	node->prev = queue->tail;
	node->next = queue->tail->next;
	node->prev->next = node;
	node->next->prev = node;
}

void deque_push_back(deque *queue, linkable *linkable)
{
	if (linkable->prev != NULL)
		linkable_unlink(linkable);

	linkable->prev = queue->tail->prev;
	linkable->next = queue->tail;
	linkable->prev->next = linkable;
	linkable->next->prev = linkable;
}

void deque_clear(deque *queue, bool also_free)
{
	if (queue->tail->next == queue->tail)
		return;

	while (true) {
		linkable *node = queue->tail->next;

		if (node == queue->tail)
			return;

		if (also_free)
			linkable_free(node);
		else
			linkable_unlink(node);
	}
}

void deque_free(deque *queue, bool free_nodes)
{
	deque_clear(queue, free_nodes);
	free(queue->tail);
	free(queue);
}

