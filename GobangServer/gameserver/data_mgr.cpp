#include "data_mgr.h"
#include "game_service.h"
#include "tcp_routine.h"
#include "game_room.h"


game_player::game_player()
{
	m_player_id = 0;
	m_room_type = cg_game_room_none;
	m_room_id = 0;
	m_board_id = 0;
	m_chess_color = 0;
	m_service_task = NULL;
	m_ready_time = 0;
	m_board_index = 0;
}

game_player::~game_player()
{
	delete m_service_task;
}

void game_player::player_online()
{
}

void game_player::player_offline()
{
	game_room *gr = game_room_mgr::get_instance()->get_game_room(m_room_type);
	if (gr)
	{
		gr->exit_board(this);
	}
}

void game_player::send_msg(protocol_number pn, const char * msg, unsigned len)
{
	if (check_proto_state(pn))
	{
		m_service_task->send_msg(pn, msg, len);
	}
}

void game_player::set_state(cg_player_state_type state)
{
	m_state = state;
}

cg_player_state_type game_player::get_state()
{
	return m_state;
}

bool game_player::check_proto_state(protocol_number pn)
{
	if (pn == protocol_number_exit_board && m_state!= cg_player_state_free)
	{
		return true;
	}
	else if (pn == protocol_number_game_type && m_state == cg_player_game_type)
	{
		return true;
	}
	else if (pn == protocol_number_join_board && m_state == cg_player_join_board)
	{
		return true;
	}
	else if (pn == protocol_number_player_info && m_state == cg_player_state_player_info)
	{
		return true;
	}
	else if (pn == protocol_number_start && m_state == cg_player_state_start_ready)
	{
		return true;
	}
	else if (pn == protocol_number_do_step && m_state == cg_player_state_playing)
	{
		return true;
	}
	else if (pn == protocol_number_surrender && m_state == cg_player_state_playing)
	{
		return true;
	}
	else if (pn == protocol_number_id_req && m_state == cg_player_state_free)
	{
		return true;
	}


	return false;
}

bool game_player::check_state_equal(cg_player_state_type pn)
{
	if (m_state == pn)	return true;

	return false;
}

void game_player::set_room_id(unsigned room_id)
{
	m_room_id = room_id;
}

unsigned game_player::get_room_id()
{
	return m_room_id;
}

void game_player::set_board_id(unsigned board_id)
{
	m_board_id = board_id;
}

unsigned game_player::get_board_id()
{
	return m_board_id;
}

int game_player::set_game_type(cg_mode_type type)
{
	m_game_type = type;
	return 0;
}

cg_mode_type game_player::get_game_type()
{
	return m_game_type;
}

int game_player::set_room_type(cg_game_room_type type)
{
	m_room_type = type;

	return 0;
}

cg_game_room_type game_player::get_room_type()
{
	return m_room_type;
}

void game_player::set_board_index(int index)
{
	m_board_index = index;
}

int game_player::get_board_index()
{
	return m_board_index;
}

void game_player::set_chess(char chess)
{
	m_chess_color = chess;
}

char game_player::get_chess()
{
	return m_chess_color;
}


data_mgr::data_mgr()
{
}

data_mgr::~data_mgr()
{
	game_player_map_t::iterator it = m_game_player_mgr.begin();
	for (; it != m_game_player_mgr.end(); ++it)
	{
		delete it->second;
	}
}

void data_mgr::load_config()
{
}

void data_mgr::player_online(tcp_routine * rt)
{
}

void data_mgr::player_offline(tcp_routine * rt)
{
	game_player *player = get_player(rt);
	if (player)
	{
		player->player_offline();
	}
	remove_player(rt);
}

game_player * data_mgr::get_player(unsigned player_id)
{
	game_player_map_t::iterator it = m_game_player_mgr.find(player_id);
	if (it != m_game_player_mgr.end())
	{
		return it->second;
	}
	return NULL;
}

game_player * data_mgr::get_player(tcp_routine * rt)
{
	game_player_map_t::iterator it = m_game_player_mgr.find(rt->get_routine_id());
	if (it != m_game_player_mgr.end())
	{
		return it->second;
	}

	return NULL;
}

void data_mgr::add_player(tcp_routine * rt)
{
	service_task *st = new service_task(rt);
	if (NULL == st)
	{
		ERROR_LOG("add_player.new service_task failed bad alloc, routine_id=%lu fd=%d", rt->get_routine_id(), rt->get_fd());
		return;
	}

	game_player *gp = new game_player();
	if (NULL == gp)
	{
		ERROR_LOG("add_player.new game_player failed bad alloc, routine_id=%lu fd=%d", rt->get_routine_id(), rt->get_fd());
		return;
	}
	gp->m_player_id = rt->get_routine_id();
	gp->m_service_task = st;

	m_game_player_mgr[rt->get_routine_id()] = gp;
}

void data_mgr::remove_player(tcp_routine * rt)
{
	game_player_map_t::iterator it = m_game_player_mgr.find(rt->get_routine_id());
	if (it != m_game_player_mgr.end())
	{
		delete it->second;
		m_game_player_mgr.erase(it);
	}
}

