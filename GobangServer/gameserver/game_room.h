#pragma once
#include "define.h"

class board;

class game_room
{
public:
	game_room();
	~game_room();

	void init();

private:
	cg_game_room_type m_room_type;
	unsigned m_room_id;
	std::vector<board *> m_boards;
	std::set<unsigned> m_emptys;
	std::set<unsigned> m_readys;
	std::set<unsigned> m_fulls;

	unsigned m_max_count;
};