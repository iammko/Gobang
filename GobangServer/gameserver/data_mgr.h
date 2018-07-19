#pragma once

#include "define.h"
#include "singleton.h"
#include "proto_define.h"
#include "service_define.h"

class service_task;
class tcp_routine;
class game_room;

class game_player
{
public:
	game_player();
	~game_player();


	unsigned get_player_id()
	{
		return m_player_id;
	}

	void send_msg(protocol_number pn, const char *msg, unsigned len);
	bool check_state();

	void set_room_id(unsigned room_id);
	unsigned get_room_id();

	void set_board_id(unsigned board_id);
	unsigned get_board_id();

	int set_game_type(cg_mode_type type);
	cg_mode_type get_game_type();

	int set_room_type(cg_game_room_type type);
	cg_game_room_type get_room_type();

	void set_chess(char chess);
	char get_chess();
private:
	friend class data_mgr;
	cg_mode_type m_game_type;
	unsigned m_player_id;
	cg_game_room_type m_room_type;
	unsigned m_room_id;
	unsigned m_board_id;
	char m_chess_color;

	service_task *m_service_task;
};

class data_mgr:public mysingleton<data_mgr>
{
private:
	data_mgr();

	friend class mysingleton<data_mgr>;
public:
	~data_mgr();

	unsigned get_unique_id() 
	{
		static unsigned unique_id = 0;
		return ++unique_id;
	}

	game_player *get_player(unsigned player_id);

	game_player *get_player(tcp_routine *rt);
	void add_player(tcp_routine *rt);
	void remove_player(tcp_routine *rt);
private:
	typedef std::map<uint64_t, game_player*> game_player_map_t;
	game_player_map_t m_game_player_mgr;
};