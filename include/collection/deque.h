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

