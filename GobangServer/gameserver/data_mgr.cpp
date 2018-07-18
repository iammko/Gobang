#include "data_mgr.h"
#include "game_service.h"
#include "tcp_routine.h"


game_player::game_player()
{
	m_player_id = 0;
	m_room_type = cg_game_room_none;
	m_room_id = 0;
	m_service_task = NULL;
}

game_player::~game_player()
{
	delete m_service_task;
}

void game_player::send_msg(protocol_number pn, const char * msg, unsigned len)
{
	m_service_task->send_msg(pn, msg, len);
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
	gp->m_player_id = get_unique_id();
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

