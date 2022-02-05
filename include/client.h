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

#ifndef H_CLIENT
#define H_CLIENT

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "signlink.h"
#include "collection/deque.h"

#define DEFAULT_WIDTH 765
#define DEFAULT_HEIGHT 503
#define LOCAL_PLAYER_ID 2047
#define MAX_ENTITY_COUNT 2048
#define SCROLLBAR_GRIP_FOREGROUND 0x4d4233
#define SCROLLBAR_GRIP_HIGHLIGHT 0x766654
#define SCROLLBAR_GRIP_LOWLIGHT 0x332d25
#define SCROLLBAR_TRACK_COLOUR 0x23201b

#define ACTORS_TO_UPDATE_IDS_SZ 1000
#define ANIMATED_PIXELS_SZ 16384
#define CAMERA_SZ 5
#define CHAT_SZ 100
#define CHARACTER_EDIT_IDENTITY_KITS_SZ 7
#define CONSTRUCT_MAP_TILES_I_SZ 4
#define CONSTRUCT_MAP_TILES_J_SZ 13
#define CONSTRUCT_MAP_TILES_K_SZ 13
#define DEFAULT_SETTINGS_SZ 2000
#define BITFIELD_MAX_VALUE_SZ 32
#define EXPERIENCE_TABLE_SZ 99
#define FPS_DISP_SZ 10
#define FRIENDS_LIST_SZ 200
#define FRIEND_WORLD_IDS_SZ 200
#define GND_ARR_I_SZ 4
#define GND_ARR_J_SZ 104
#define GND_ARR_K_SZ 104
#define IGNORE_LIST_SZ 100
#define INPUT_BUF_SZ 128
#define KEY_STATUS_SZ 128
#define MENU_ACTION_SZ 500
#define NPC_IDS_SZ 16384
#define INTERFACE_SETTINGS_SZ 2000
#define OTIMS_SZ 10
#define OVERHEAD_MESSAGE_SZ 50
#define PLAYER_ACTION_TEXT_SZ 5
#define PRIVATE_MESSAGES_SZ 100
#define SKILLS_SZ 25
#define TAB_INTERFACE_IDS_SZ 15
#define TILES_SZ 104
#define TRACK_DELAY_SZ 50
#define TRACK_IDS_SZ 50
#define TRACK_LOOP_SZ 50
#define WALKING_QUEUE_SZ 4000

extern const char *SKILL_NAMES[SKILLS_SZ];

extern bool skill_enabled[SKILLS_SZ];

typedef enum rs_result {
    RS_RESULT_OK = 0,
    RS_RESULT_ERROR,
    RS_RESULT_NO_MEM
} rs_result;

typedef struct rsclient {
	uint32_t node_id;
	uint32_t port_offset;
	uint32_t storeid;

	bool lowmem;
	bool members_world;

	signlink *sign;

	int32_t game_state;
	int32_t delay_time;
	int32_t min_delay;
	uint64_t otims[OTIMS_SZ];
	int32_t fps;
	bool debug_requested;
	uint32_t width;
	uint32_t height;

	int32_t *pixels;
	SDL_Window *window;
	SDL_Surface *screen;
	SDL_Surface *pixel_surface;

	bool clear_screen;
	bool awt_focus;
	int32_t mouse_button;
	int32_t mouse_x;
	int32_t mouse_y;
	int32_t event_mouse_button;
	int32_t event_click_x;
	int32_t event_click_y;
	int32_t event_click_time;
	int32_t click_type;
	int32_t click_x;
	int32_t click_y;
	int64_t click_time;
	int32_t key_status[KEY_STATUS_SZ];
	int32_t input_buf[INPUT_BUF_SZ];
	int32_t read_index;
	int32_t write_index;

	int32_t tab_interface_ids[TAB_INTERFACE_IDS_SZ];

	int32_t draw_cycle;
	int32_t int_1213; // undocumented
	int32_t distance_values[TILES_SZ][TILES_SZ];
	int32_t friend_world_ids[FRIEND_WORLD_IDS_SZ];
	deque ***gnd_arr;
	// jbuf text_stream;
	// npc npcs[NPC_IDS_SZ];
	uint32_t npc_ids[NPC_IDS_SZ];
	int32_t actors_to_update_ids[ACTORS_TO_UPDATE_IDS_SZ];
	// jbuf login_stream;
	bool effects_enabled;
	int32_t open_interface_id;
	int32_t skill_experience[SKILLS_SZ];
	bool bool_872;
	int32_t camera_jitter[CAMERA_SZ];
	int32_t current_track_id;
	bool custom_camera_active[CAMERA_SZ];
	bool draw_flames;
	char *report_abuse_input;
	int32_t player_list_id;
	bool menu_open;
	char *input_string;
	//player players[MAX_ENTITY_COUNT];
	int32_t local_players[MAX_ENTITY_COUNT];
	int32_t players_observed[MAX_ENTITY_COUNT];
	int32_t next_camera_randomization_a;
	int32_t next_camera_randomization_h;
	int32_t next_camera_randomization_v;
	// jbuf player_appearance_data[MAX_ENTITY_COUNT];
	int32_t waypoints[TILES_SZ][TILES_SZ];
	int8_t animated_pixels[ANIMATED_PIXELS_SZ];
	int32_t skill_level[SKILLS_SZ];
	int64_t ignore_list_longs[IGNORE_LIST_SZ];
	bool loading_error;
	int32_t camera_freq[CAMERA_SZ];
	int32_t tile_render_count[TILES_SZ][TILES_SZ];
	int32_t chat_types[CHAT_SZ];
	char *chat_names[CHAT_SZ];
	char *chat_messages[CHAT_SZ];
	// indexed_img side_icon_image[13];
	bool window_focused;
	int64_t friends_list_longs[FRIENDS_LIST_SZ];
	int32_t current_song;
	int32_t sprite_draw_x;
	int32_t sprite_draw_y;
	// file_cache caches[5];
	int32_t interface_settings[INTERFACE_SETTINGS_SZ];
	bool bool_972;
	int32_t overhead_text_draw_x[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_draw_y[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_height[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_width[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_colour[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_effect[OVERHEAD_MESSAGE_SZ];
	int32_t overhead_text_cycle[OVERHEAD_MESSAGE_SZ];
	char *overhead_text_message[OVERHEAD_MESSAGE_SZ];
	int32_t last_region_id;
	// this.hitMarkImage = new Sprite[20];
	int32_t character_edit_colours[5];
	char *amount_or_name_input;
	// this.projectileQueue = new DoubleEndedQueue();
	bool camera_moved_write;
	int32_t walkable_interface_id;
	int32_t unknown_camera_var[CAMERA_SZ]; // undocumented
	bool character_model_changed;
	int32_t dialog_id;
	int32_t skill_max_level[SKILLS_SZ];
	int32_t default_settings[DEFAULT_SETTINGS_SZ];
	bool character_edit_change_gender;
	int32_t flashing_side_bar;
	// this.stationaryGraphicQueue = new DoubleEndedQueue();
	// this.chatboxInterface = new RSInterface();
	int32_t character_edit_identity_kits[CHARACTER_EDIT_IDENTITY_KITS_SZ];
	bool loading_map;
	char *friends_list[FRIENDS_LIST_SZ];
	// this.inStream = Buffer.create();
	int32_t expected_crcs[9];
	int32_t menu_action_data1[MENU_ACTION_SZ];
	int32_t menu_action_data2[MENU_ACTION_SZ];
	int32_t menu_action_data3[MENU_ACTION_SZ];
	int32_t menu_action_id[MENU_ACTION_SZ];
	// this.headIcons = new Sprite[20];
	bool draw_tab_icons;
	char *chatbox_input_needed_str;
	char *player_action_text[5];
	bool player_action_unpinned[5];
	int32_t construct_map_tiles[CONSTRUCT_MAP_TILES_I_SZ][CONSTRUCT_MAP_TILES_J_SZ][CONSTRUCT_MAP_TILES_K_SZ];
	bool in_tutorial_island;
	bool continued_dialogue;
	// // this.crosses = new Sprite[8];
	bool music_enabled;
	bool redraw_tab;
	bool logged_in;
	bool report_abuse_mute;
	bool load_generated_map;
	bool cutscene_active;
	int32_t randomisation_minimap_rotation;
	int32_t randomisation_minimap_zoom;
	char *entered_username;
	char *entered_password;
	bool generic_loading_error;
	int32_t report_abuse_interface_id;
	// this.spawnObjectList = new DoubleEndedQueue();
	int32_t camera_vertical;
	int32_t inventory_overlay_interface_id;
	// this.stream = Buffer.create();
	// this.menuActionName = new String[500];
	int32_t camera_amplitude[CAMERA_SZ];
	int32_t track_ids[TRACK_IDS_SZ];
	int32_t chatbox_scroll_max;
	char *prompt_input;
	// this.modIcons = new IndexedImage[2];
	int32_t current_tab_id;
	bool redraw_chatbox;
	bool song_changing;
	// this.currentCollisionMap = new CollisionMap[4];
	bool update_chat_settings;
	int32_t private_messages[PRIVATE_MESSAGES_SZ];
	int32_t track_loop[TRACK_LOOP_SZ];
	bool last_item_dragged;
	int32_t track_delay;
	bool rs_already_loaded;
	bool message_prompt_raised;
	char *login_message1;
	char *login_message2;
	int32_t chatbox_interface_id;
	int32_t walking_queue_x[WALKING_QUEUE_SZ];
	int32_t walking_queue_y[WALKING_QUEUE_SZ];
	int32_t current_track_loop;
	// this.titleScreen = new TitleScreen();
	// this.minimap = new Minimap();
} rsclient;

struct rsclient_info_disp {
	char fps[FPS_DISP_SZ];
	SDL_Rect rect;
};

#endif