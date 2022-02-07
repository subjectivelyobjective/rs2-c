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

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "sodium.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "client.h"
#include "jbuf.h"
#include "rasterizer.h"
#include "region.h"
#include "signlink.h"
#include "collection/deque.h"
#include "collection/linkable.h"
#include "def/gameobjdef.h"
#include "scene/worldcontroller.h"
#include "util/color.h"
#include "util/util.h"

#define USAGE "Usage: node-id, port-offset, [lowmem/highmem], [free/members], storeid\n"

static char title[] = "rs2client317";

static const bool SKILL_ENABLED[] = {
	true, true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, false, true, false,
	false, false, false
};

static const uint32_t APPEARANCE_COLOURS1[] = {
	6798, 107, 10283, 16, 4797, 7744, 5799, 4634, 33697, 22433, 2983,
	54193
};

static const uint32_t APPEARANCE_COLOURS2[] = {
	8741, 12, 64030, 43162, 7735, 8404, 1701, 38430, 24094, 10153, 56621,
	4783, 1341, 16578, 35003, 25239
};

static const uint32_t APPEARANCE_COLOURS3[] = {
	5238, 8742, 12, 64030, 43162, 7735, 8404, 1701, 38430, 24094, 10153,
	56621, 4783, 1341, 16578, 35003
};

static const uint32_t APPEARANCE_COLOURS4[] = { 4626, 11146, 6439, 12, 4758, 10270 };

static const uint32_t APPEARANCE_COLOURS5[] = {
	4550, 4537, 5681, 5673, 5790, 6806, 8076, 4574
};

static const uint32_t BEARD_COLOURS[] = {
	9104, 10275, 7595, 3610, 7975, 8526, 918, 38802, 24466, 10145, 58654,
	5027, 1457, 16565, 34991, 25486
};

static const uint32_t OBJECT_TYPES[] = {
	0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

static const uint32_t SPOKEN_TEXT_COLOURS[] = {
	0xFFFF00, 0xFF0000, 0x00FF00, 0x00FFFF, 0xFF00FF, 0xFFFFFF
};

static const wchar_t VALID_USER_PASS_CHARS[] =
	L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"£$%^&*()-_=+[{]};:'@#~,<.>/?\\| ";

const char *SKILL_NAMES[] = {
	"attack", "defence", "strength", "hitpoints", "ranged", "prayer",
	"magic", "cooking", "woodcutting", "fletching", "fishing",
	"firemaking", "crafting", "smithing", "mining",
	"herblore", "agility", "thieving", "slayer", "farming", "runecraft",
	"-unused-", "-unused-", "-unused-", "-unused-"
};

static const char *TTF_PREFERENCE[] = {
	"Helvetica", "Nimbus Sans L", "Nimbus Sans", "Liberation Sans",
	"Roboto", "Arial"
};

static int32_t EXPERIENCE_TABLE[EXPERIENCE_TABLE_SZ];

static int32_t BITFIELD_MAX_VALUE[BITFIELD_MAX_VALUE_SZ];

static struct rsclient_info_disp info_disp;

static TTF_Font *reg_ttf = NULL;
static TTF_Font *bold_ttf = NULL;

static int32_t tick = 0;

static deque *buffer_cache = NULL;

void rsclient_stopmidi(rsclient *client)
{
	if (client != NULL && client->sign != NULL) {
		client->sign->midifade = 0;
		const char *stop = "stop";
		memcpy(client->sign->midi, stop, strnlen(stop, MIDI_SZ) + 1);
	}
}


void cleanup(rsclient *client)
{
	signlink_reporterror = false;

	rsclient_stopmidi(client);

	if (client != NULL && client->window != NULL)
		SDL_DestroyWindow(client->window);

	TTF_CloseFont(bold_ttf);
	TTF_CloseFont(reg_ttf);

	TTF_Quit();
	SDL_Quit();

	deque_free(buffer_cache, true);
	buffer_cache = NULL;

	if (client != NULL) {
		for (size_t i = 0; i < GND_ARR_I_SZ; i++) {
			for (size_t j = 0; j < GND_ARR_J_SZ; j++)
				free(client->gnd_arr[i][j]);

			free(client->gnd_arr[i]);
		}

		free(client->gnd_arr);

		signlink_free(client->sign);
		client->sign = NULL;

		client->pixel_surface = NULL;
		client->screen = NULL;
		client->window = NULL;

		free(client);
	}
}


rs_result rsclient_exit(rsclient *client, rs_result res)
{
	if (client != NULL) {
		client->game_state = -2;
		cleanup(client);
	}

	exit(res);
	return res;
}

rs_result rsclient_exit_usage(rsclient *client)
{
	printf(USAGE);
	return rsclient_exit(client, RS_RESULT_ERROR);
}

rs_result rsclient_error(rsclient *client, char *errmsg, bool exit)
{
	if (errmsg != NULL)
		fprintf(stderr, "[ERROR] %s\n", errmsg);

	if (exit)
		return rsclient_exit(client, RS_RESULT_ERROR);

	return RS_RESULT_ERROR;
}

void rsclient_new(rsclient *client)
{
	if (client == NULL)
		return;

	memcpy(client->skill_enabled, SKILL_ENABLED, SKILLS_SZ);

	for (size_t i = 0; i < TAB_INTERFACE_IDS_SZ; i++)
		client->tab_interface_ids[i] = -1;

	// Populate the experience table
	int32_t total_exp = 0;
	for (size_t n = 0; n < EXPERIENCE_TABLE_SZ; n++) {
		int32_t level = n + 1;
		int32_t exp = (int32_t) (level + 300l * pow(2l, level / 7l));
		total_exp += exp;
		EXPERIENCE_TABLE[n] = total_exp / 4;
	}

	total_exp = 2;
	for (size_t k = 0; k < BITFIELD_MAX_VALUE_SZ; k++) {
		BITFIELD_MAX_VALUE[k] = total_exp - 1;
		total_exp += total_exp;
	}

	client->gnd_arr =
		(deque ***) malloc(GND_ARR_I_SZ * sizeof(deque **));

	for (size_t i = 0; i < GND_ARR_I_SZ; i++) {
		client->gnd_arr[i] =
			(deque **) malloc(GND_ARR_J_SZ * sizeof(deque *));

		for (size_t j = 0; j < GND_ARR_J_SZ; j++)
			client->gnd_arr[i][j] =
				(deque *) malloc(GND_ARR_K_SZ * sizeof(deque));
	}

	client->sign = signlink_new();

	client->delay_time = 20;
	client->min_delay = 1;
	client->debug_requested = false;
	client->clear_screen = true;
	client->awt_focus = true;

	client->window = NULL;
	client->screen = NULL;
	client->pixel_surface = NULL;

	client->pixels = NULL;
	client->draw_cycle = 0;
	client->int_1213 = 0;

	client->effects_enabled = true;
	client->open_interface_id = -1;
	client->bool_872 = false; // undocumented
	client->draw_flames = false;
	client->report_abuse_input = "";
	client->player_list_id = -1;
	client->menu_open = false;
	client->input_string = "";
	client->next_camera_randomization_a = 1;
	client->next_camera_randomization_h = 2;
	client->next_camera_randomization_v = 2;
	client->loading_error = false;
	client->window_focused = true;
	client->current_song = -1;
	client->sprite_draw_x = -1;
	client->sprite_draw_y = -1;
	client->bool_972 = false; // undocumented
	client->last_region_id = -1;
	client->amount_or_name_input = "";
	client->camera_moved_write = false;
	client->walkable_interface_id = -1;
	client->character_model_changed = false;
	client->dialog_id = -1;
	client->character_edit_change_gender = true;
	client->flashing_side_bar = -1;
	client->loading_map = false;
	client->draw_tab_icons = false;
	client->chatbox_input_needed_str = "";
	client->in_tutorial_island = false;
	client->continued_dialogue = false;
	client->music_enabled = true;
	client->redraw_tab = false;
	client->logged_in = false;
	client->report_abuse_mute = false;
	client->load_generated_map = false;
	client->cutscene_active = false;
	client->randomisation_minimap_rotation = 2;
	client->randomisation_minimap_zoom = 1;
	client->entered_username = "";
	client->entered_password = "";
	client->generic_loading_error = false;
	client->report_abuse_interface_id = -1;
	client->camera_vertical = 128;
	client->inventory_overlay_interface_id = -1;
	client->chatbox_scroll_max = 78;
	client->prompt_input = "";
	client->current_tab_id = 3;
	client->redraw_chatbox = false;
	client->song_changing = true;
	client->update_chat_settings = false;
	client->last_item_dragged = false;
	client->rs_already_loaded = false;
	client->message_prompt_raised = false;
	client->login_message1 = "";
	client->login_message2 = "";
	client->chatbox_interface_id = -1;
	client->current_track_id = -1;
	client->current_track_loop = -1;
}

int rsclient_create_frame(rsclient *client, uint32_t width, uint32_t height)
{
	assert(client != NULL);
	assert(width != 0);
	assert(height !=0);

	client->width = width;
	client->height = height;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "[ERROR] SDL_Init(): %s\n", SDL_GetError());
		return RS_RESULT_ERROR;
	}

	if (TTF_Init() < 0) {
		fprintf(stderr, "[ERROR] TTF_Init(): %s\n",  TTF_GetError());
		return RS_RESULT_ERROR;
	}

	SDL_AudioSpec wanted_audio;

	wanted_audio.freq = 8000;
	wanted_audio.format = AUDIO_S16;
	wanted_audio.channels = 1;
	wanted_audio.silence = 0;
	wanted_audio.samples = 1024;
	wanted_audio.callback = NULL;

	if (SDL_OpenAudio(&wanted_audio, NULL) < 0)
		fprintf(stderr, "[ERROR] SDL_OpenAudio(): %s\n", SDL_GetError());

	client->window = SDL_CreateWindow(
					title, SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED, width, height,
					SDL_WINDOW_SHOWN);

	if (!client->window) {
		fprintf(stderr, "[ERROR] SDL_CreateWindow(): %s\n", SDL_GetError());
		return RS_RESULT_ERROR;
	}

	client->screen = SDL_GetWindowSurface(client->window);

	client->pixel_surface = SDL_CreateRGBSurface(
					0, width, height, 32, 0xff0000,
					0x00ff00, 0x0000ff, 0);

	client->pixels = client->pixel_surface->pixels;

	*info_disp.fps = '\0';
	info_disp.rect.x = 20;
	info_disp.rect.y = 20;
	info_disp.rect.w = 45;
	info_disp.rect.h = 15;

	return 0;
}

void set_lowmem(rsclient *client, bool is_lowmem)
{
	client->lowmem = is_lowmem;
	gameobjdef_lowmem = is_lowmem;
	rasterizer_lowmem = is_lowmem;
	region_lowmem = is_lowmem;
	worldcontroller_lowmem = is_lowmem;
}

static void rsclient_update_login(rsclient *client)
{
	// TODO
}

static void rsclient_update_game(rsclient *client)
{
	// TODO
}

static void rsclient_process_on_demand_queue(rsclient *client)
{
	// TODO
}

static void rsclient_black_screen(rsclient *client)
{
	uint32_t area = client->width * client->height;

	for (size_t i = 0; i < area; i++)
		client->pixels[i] = 0x000000;
}

static void rsclient_draw(rsclient *client)
{
	if (client->window == NULL)
		return;

	snprintf(info_disp.fps, FPS_DISP_SZ, "Fps: %d", client->fps);

	SDL_Surface *fps_display =
		TTF_RenderText_Solid(bold_ttf, info_disp.fps, YELLOW);

	if (!fps_display)
		fprintf(stderr, "Failed to render text: %s\n", TTF_GetError());

	info_disp.rect.w = fps_display->w;
	info_disp.rect.h = fps_display->h;

	SDL_BlitSurface(client->pixel_surface, NULL, client->screen, NULL);
	SDL_BlitSurface(fps_display, NULL, client->screen, &info_disp.rect);
	SDL_UpdateWindowSurface(client->window);

	SDL_FreeSurface(fps_display);
}


static rs_result rsclient_process_game_loop(rsclient *client)
{
	if (	client->rs_already_loaded ||
		client->loading_error ||
		client->generic_loading_error)
			return rsclient_error(client, "Loading error processing game loop.", false);

	tick++;

	if (!client->logged_in)
		rsclient_update_login(client);
	else
		rsclient_update_game(client);

	rsclient_process_on_demand_queue(client);

	return RS_RESULT_OK;
}

static rs_result rsclient_process_drawing(rsclient *client)
{
	if (client != NULL) {
		rsclient_black_screen(client);

		rsclient_draw(client);
	} else {
		return 1;
	}

	return RS_RESULT_OK;
}

static void rsclient_poll_sdl_events(rsclient *client)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			rsclient_exit(client, EXIT_SUCCESS);
			break;
		}
	}
}

static rs_result rsclient_load_ttf(rsclient *client)
{
	if (client == NULL || client->sign == NULL) {
		fprintf(stderr, "Client or signlink is NULL.\n");
		return RS_RESULT_ERROR;
	}

	char *cache_dir = client->sign->cache_dir;

	if (cache_dir == NULL) {
		fprintf(stderr, "Cache directory is NULL.\n");
		return RS_RESULT_ERROR;
	}


	char *bold = rs_strconcat(cache_dir, "nimbus-sans-l_bold.ttf");
	char *reg = rs_strconcat(cache_dir, "nimbus-sans-l_regular.ttf");

	if (bold == NULL || reg == NULL) {
		fprintf(stderr, "TTF path is null.\n");
		free(bold);
		free(reg);
		return RS_RESULT_ERROR;
	}

	bold_ttf = TTF_OpenFont(bold, 13);
	reg_ttf = TTF_OpenFont(reg, 13);

	if  (bold_ttf == NULL || reg_ttf == NULL) {
		fprintf(stderr,
			"Failed to load a TTF font. SDL_ttf Error: %s\n",
			TTF_GetError());
		free(bold);
		free(reg);
		return RS_RESULT_ERROR;
	}

	free(bold);
	free(reg);
	return RS_RESULT_OK;
}
// Animation loop
static int rsclient_start(void *_client)
{
	rsclient *client = (rsclient *) _client;

	int32_t opos = 0;
	int32_t ratio = 256;
	int32_t delay = 1;
	int32_t count = 0;
	int32_t intex = 0;

	for (size_t otim = 0; otim < OTIMS_SZ; otim++)
		client->otims[otim] = SDL_GetTicks64();

	while (client->game_state >= 0) {
		if (client->game_state > 0) {
			if (--client->game_state == 0) {
				rsclient_exit(client, EXIT_SUCCESS);
				return 0;
			}
		}

		int32_t i2 = ratio;
		int32_t j2 = delay;
		ratio = 300;
		delay = 1;

		uint64_t now = SDL_GetTicks64();

		if (client->otims[opos] == 0) {
			ratio = i2;
			delay = j2;
		} else if (now > client->otims[opos]) {
			ratio = (int32_t) (2560 * client->delay_time / (now - client->otims[opos]));
		}

		if (ratio < 25)
			ratio = 25;

		if (ratio > 256) {
			ratio = 256;
			delay = (int32_t) (client->delay_time - (now - client->otims[opos]) / 10l);
		}

		if (delay > client->delay_time)
			delay = client->delay_time;

		client->otims[opos] = now;
		opos = (opos + 1) % 10;

		if (delay > 1)
			for (size_t otim = 0; otim < 10; otim++)
				if (client->otims[otim] != 0l)
					client->otims[otim] += delay;

		if (delay < client->min_delay)
			delay = client->min_delay;

		SDL_Delay(delay);

		for (; count < 256; count += ratio) {
			rsclient_poll_sdl_events(client);

			client->click_type = client->event_mouse_button;
			client->click_x = client->event_click_x;
			client->click_y = client->event_click_y;
			client->click_time = client->event_click_time;
			client->event_mouse_button = 0;

			rsclient_process_game_loop(client);

			client->read_index = client->write_index;
		}

		count &= 0xff;

		if (client->delay_time > 0)
			client->fps =
				(1000 * ratio) / (client->delay_time * 256);

		rsclient_process_drawing(client);

		if (client->debug_requested) {
			printf("ntime: %ju\n", (uintmax_t) now);
			for (int i = 0; i < 10; i++) {
				int32_t otim = ((opos - i - 1) + 20) % 10;
				printf("otim%d:%ju\n", otim, 
					(uintmax_t) client->otims[otim]);
			}

			printf("fps:%d ratio: %d count: %d\n", client->fps,
							ratio, count);
			printf("del:%d deltime:%d mindel:%d\n", delay,
				client->delay_time, client->min_delay);
			printf("intex:%d opos:%d\n", intex, opos);

			client->debug_requested = false;
			intex = 0;
		}

		if (client->game_state == -1)
			return rsclient_exit(client, EXIT_SUCCESS);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (sodium_init() < 0)
		return rsclient_error(NULL, "libsodium failed to initialize.",
						true);

	setlocale(LC_ALL, "C.UTF-8");

	printf("RS2 user client - release #%d\n", REVISION);

	if (argc != 6)
		return rsclient_exit_usage(NULL);

	rsclient *client = malloc(sizeof(rsclient));

	if (client == NULL)
		return rsclient_error(NULL, "Couldn't allocate client.", true);

	rsclient_new(client);

	client->node_id = atoi(argv[1]);
	client->port_offset = atoi(argv[2]);

	if (strcmp(argv[3], "lowmem") == 0)
		set_lowmem(client, true);
	else if (strcmp(argv[3], "highmem") == 0)
		set_lowmem(client, false);
	else
		rsclient_exit_usage(client);

	if (strcmp(argv[4], "free") == 0)
		client->members_world = false;
	else if (strcmp(argv[4], "members") == 0)
		client->members_world = true;
 	else
		rsclient_exit_usage(client);

	const uint32_t storeid = (uint32_t) strtol(argv[5], NULL, 10);

	client->sign->storeid = storeid;
	signlink_run(client->sign);

	if (buffer_cache == NULL)
		buffer_cache = init_buffer_cache();

	rs_result res = rsclient_create_frame(client, DEFAULT_WIDTH,
					DEFAULT_HEIGHT);

	if (res)
		return rsclient_error(NULL, "Couldn't create client frame.",
						true);

	rs_result ttf_load_err = rsclient_load_ttf(client);

	if (ttf_load_err)
		return rsclient_error(client, "Failed to load TTF fonts.",
						true);

	SDL_Thread *thread_id = SDL_CreateThread(rsclient_start, "Rendering",
					(void *) client);
	SDL_WaitThread(thread_id, NULL);

	printf("lol: %s\n", client->sign->cache_dir);

	return rsclient_exit(client, EXIT_SUCCESS);
}

