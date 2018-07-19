#include "game_room.h"
#include "data_mgr.h"


game_room::game_room(cg_game_room_type type, unsigned room_id):
	m_room_type(type), 
	m_room_id(room_id),
	m_max_count(400)
{
	init();
}

game_room::~game_room()
{
}

void game_room::init()
{
	m_boards.resize(m_max_count);
	for (int i = 0; i < m_max_count; ++i)
	{
		m_boards[i] = board(i + 1);
	}
}

bool game_room::join_board(game_player *gp, unsigned board_id)
{
	board *b = alloc_room_by_type_id(gp->get_game_type(), board_id);
	if (!b) return false;

	if (b->join(gp))
	{
		unsigned player_count = b->player_count();
		if (player_count == 1)
		{
			m_emptys.erase(b->m_board_id);
			m_readys.insert(b->m_board_id);
		}
		else if (player_count == 2)
		{
			m_readys.erase(b->m_board_id);
			m_fulls.insert(b->m_board_id);
		}

		return true;
	}
	return false;
}

bool game_room::exit_board(game_player * gp)
{
	board *b = get_board_by_id(gp->get_board_id());
	if (!b)	return false;

	
	if (b->exit(gp))
	{
		unsigned player_count = b->player_count();
		if (player_count == 0)
		{
			m_readys.erase(b->m_board_id);
			m_emptys.insert(b->m_board_id);
		}
		else if (player_count == 1)
		{
			m_fulls.erase(b->m_board_id);
			m_readys.insert(b->m_board_id);
		}

		return true;
	}
	return false;
}

board* game_room::alloc_room_by_type_id(cg_mode_type type, unsigned board_id)
{
	if (type == cg_mode_type_online_quickstart)
	{
		board_set_t::iterator it = m_readys.begin();
		if (it != m_readys.end())
		{
			return &(it->second);
		}
		it = m_emptys.begin();
		if (it != m_emptys.end())
		{
			return &(it->second);
		}
		return NULL:
	}
	else
	{

	}
}

board * game_room::get_board_by_id(unsigned board_id)
{
	if (board_id > 0 && board_id < m_max_count)
	{
		return &(m_boards[board_id]);
	}

	return NULL;
}

game_room * game_room_mgr::get_game_room(cg_game_room_type type)
{
	game_room_map_t::iterator it = m_room_mgr.find(cg_game_room_type);
	if (it != m_room_mgr.end())
	{
		return &(it->second);
	}
	return NULL;
}

bool game_room_mgr::join_board(game_player *gp, unsigned board_id)
{
	game_room *t_room = get_game_room(gp->get_room_type());
	if (!t_room) return false;

	return t_room->join_board(board_id);
}

