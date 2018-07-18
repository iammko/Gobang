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
	int choose_type(cg_game_room_type type);


private:
	friend class data_mgr;
	unsigned m_player_id;
	cg_game_room_type m_room_type;
	unsigned m_room_id;

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

	game_player *get_player(tcp_routine *rt);
	void add_player(tcp_routine *rt);
	void remove_player(tcp_routine *rt);
private:
	typedef std::map<uint64_t, game_player*> game_player_map_t;
	game_player_map_t m_game_player_mgr;
};