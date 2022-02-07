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

#ifndef H_WORLDCONTROLLER
#define H_WORLDCONTROLLER

#include <stdbool.h>

#define INT_486_SZ 10000
#define INT_487_SZ 10000
#define INTERACTIVE_OBJ_CACHE_SZ 5000
#define TILE_LEN_SZ 104
#define TILE_W_SZ 104
#define TILE_H_SZ 4

extern bool worldcontroller_lowmem;

typedef struct worldcontroller {
	int32_t ***height_map;
	const int32_t length;
	const int32_t width;
	const int32_t height;
	// interactive_obj interactive_obj_cache[INTERACTIVE_OBJ_CACHE_SZ];
	int32_t intarr_486[INT_486_SZ];
	int32_t intarr_487[INT_487_SZ];
	int32_t map_sz_x;
	int32_t map_sz_y;
	int32_t map_sz_z;
	// tile gnd_arr[TILE_H_SZ][TILE_W_SZ][TILE_LEN_SZ];
	int32_t ***intarrarrarr_445;
} worldcontroller;

#endif

