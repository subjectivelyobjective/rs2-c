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

#ifndef LINKABLE_H
#define LINKABLE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct linkable {
	int64_t id;
	struct linkable *next;
	struct linkable *prev;
	void *ptrvalue;
} linkable;

linkable *linkable_new(void *ptr);

void linkable_unlink(linkable *node);
void linkable_free(linkable *node);

#endif

