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

typedef struct gamefont {
	int8_t **glyph_pixels;
	int32_t *glyph_width;
	int32_t *glyph_height;

	int32_t *horizontal_kerning;
	int32_t *vertical_kerning;

	int32_t glyph_display_width;

	int32_t font_height;

	bool strikethrough;
} gamefont;

#endif

