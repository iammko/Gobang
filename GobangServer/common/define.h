#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#define MAXLINE 4096

#ifndef SAFE_DELETE 
#define SAVE_DELETE(p) { \
		if (p) {delete p; p = NULL;} \
	} 
#endif

#define INFO_LOG	report_event
#define DEBUG_LOG	report_event
#define ERROR_LOG	report_event

void    report_event(const char *format, ...);

const char cb_lenth = 15;
const char cb_black_chess = 'X';
const char cb_white_chess = 'O';
const char cb_chesses[2] = { 'O', 'X' };

enum cg_result_type
{
	cg_result_none = 0,
	cg_result_win_lost = 1,
	cg_result_draw = 2,
	cg_result_surrender = 3,
	cg_result_escape = 4,
	cg_result_count
};

enum cg_mode_type
{
	cg_mode_type_none = 0,
	cg_mode_type_offpve = 1,
	cg_mode_type_offpvp = 2,
	cg_mode_type_online_quickstart = 3,
	cg_mode_type_online_race = 4,
	cg_mode_type_max
};

enum cg_game_room_type
{
	cg_game_room_none = 0,
	cg_game_room_normal = 1,

	cg_game_room_count
};

#pragma	pack(1)
struct	service_msg_header
{
	service_msg_header() :msg_size(0)
	{
	}

	service_msg_header& operator=(const service_msg_header &smh)
	{
		this->msg_size = smh.msg_size;
		this->msg_type = smh.msg_type;
		return	*this;
	}

	service_msg_header(const service_msg_header &smh)
	{
		this->msg_size = smh.msg_size;
		this->msg_type = smh.msg_type;
	}
	uint32_t	msg_size;
	uint32_t	msg_type;
};
#pragma	pack()

enum cg_player_state_type
{
	cg_player_state_none = 0,
	cg_player_state_free = 1,
	cg_player_game_type = 2,
	cg_player_join_board = 3,
	cg_player_state_player_info = 4,
	cg_player_state_start_ready = 5,
	cg_player_state_playing = 6,
	cg_player_state_game_over = 7,

	cg_player_state_count
};