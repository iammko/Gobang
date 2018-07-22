#include "game_room.h"
#include "data_mgr.h"
#include "board.h"


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
	m_boards.reserve(m_max_count);
	for (unsigned i = 0; i < m_max_count; ++i)
	{
		board tmp(m_room_id, i + 1);
		m_boards.push_back(tmp);
		m_emptys.insert(i + 1);
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

bool game_room::send_board_list(game_player * gp, int pre_next)
{
	proto::board_list_ret send;
	int index = gp->get_board_index();
	if (pre_next == 1)
	{
		--index;
		if (index < 0)
		{
			index = m_boards.size() / BOARDS_COUNT_PER - 1;
		}
	}
	else if (pre_next == 2)
	{
		++index;
		if (index > (int)m_boards.size() / BOARDS_COUNT_PER - 1)
		{
			index = 0;
		}
	}

	if (index < 0 || index > (int)m_boards.size() / BOARDS_COUNT_PER - 1)	index = 0;

	int begin = index * BOARDS_COUNT_PER;
	for (int i = 0; i < BOARDS_COUNT_PER && begin < (int)m_boards.size(); ++i, ++begin)
	{
		proto::board_info *info = send.add_boards();
		if (info)
		{
			info->set_board_id(m_boards[begin].m_board_id+100);
			info->set_player_count(m_boards[begin].player_count());
			info->set_state(m_boards[begin].m_game_state);
		}
	}
	gp->set_board_index(index);

	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	gp->send_msg(protocol_number_board_list, &bytes[0], size);

	return false;
}

board* game_room::alloc_room_by_type_id(cg_mode_type type, unsigned board_id)
{
	if (type == cg_mode_type_online_quickstart)
	{
		board_set_t::iterator it = m_readys.begin();
		if (it != m_readys.end())
		{
			return get_board_by_id(*it);
		}
		it = m_emptys.begin();
		if (it != m_emptys.end())
		{
			return get_board_by_id(*it);
		}
	}
	else if (type == cg_mode_type_online_room_mode)
	{
		if (board_id-101 < m_boards.size())
		{
			return &(m_boards[board_id - 101]);
		}
	}

	return NULL;
}

board * game_room::get_board_by_id(unsigned board_id)
{
	if (board_id > 0 && board_id < m_max_count)
	{
		return &(m_boards[board_id-1]);
	}

	return NULL;
}

game_room_mgr::game_room_mgr()
{
	m_room_mgr.insert(std::pair<cg_game_room_type, game_room>(cg_game_room_normal, game_room(cg_game_room_normal, 1)));
}

void game_room_mgr::load_config()
{
}

game_room * game_room_mgr::get_game_room(cg_game_room_type type)
{
	game_room_map_t::iterator it = m_room_mgr.find(type);
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

	return t_room->join_board(gp, board_id);
}

