#pragma once

#include "define.h"

struct chess_player
{
	chess_player()
	{
		bzero(this, sizeof(*this));
	}
	unsigned m_player_id;
	char m_chess;
};

class board
{
public:
	board(unsigned board_id);
	~board();

	bool join(game_player *gp);

	bool exit(game_player *gp);

	unsigned player_count();

private: 
	friend class game_room;

	unsigned m_room_id;
	unsigned m_board_id;
	int m_gameover;
	char m_turn_chess;
	chess_player m_players[2];
	char[cb_lenth][cb_lenth] m_chesses;
	std::list<chess_player> m_judges;
};