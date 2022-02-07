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

#ifndef H_JBUF
#define H_JBUF

#include <stdbool.h>
#include <stdint.h>

#include <collection/deque.h>

typedef struct jbuf {
	int8_t *buf;

	int32_t position;
	int32_t bit_position;

	int32_t cache_count;
} jbuf;

deque *init_buffer_cache(void);

#endif

