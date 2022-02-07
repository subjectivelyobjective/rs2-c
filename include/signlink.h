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

#ifndef H_SIGNLINK
#define H_SIGNLINK

#include <stdbool.h>
#include <stdint.h>

#define REVISION 317

#define CACHE_DIR ".file_store_"

#define MIDI_SZ 10

extern bool signlink_reporterror;

typedef struct signlink {
	uint32_t storeid;
	char *cache_dir;

	char midi[MIDI_SZ];
	int midifade;
} signlink;

signlink *signlink_new(void);

void signlink_free(signlink *sign);

int signlink_run(signlink *sign);

size_t get_path_max(void);

#endif

