#include "chessboard.h"
#include "board.h"

board::board(unsigned board_id):m_board_id(board_id)
{
	m_gameover = false;
	m_turn_chess = 0;

	for (int i = 0; i < cb_lenth; ++i)
	{
		for (int j = 0; j < cb_lenth; ++j)
		{
			m_chesses[i][j] = '+';
		}
	}

	for (int i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		m_players[i].m_player_id = 0;
		if (i == 0)
			m_players[i].m_chess = cb_white_chess;
		else
			m_players[i].m_chess = cb_black_chess;
	}
}

board::~board()
{
}

bool board::join(game_player * gp)
{
	gp->set_room_id(m_room_id);
	gp->set_board_id(m_board_id);

	for (int i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == 0)
		{
			m_players[i].m_player_id = gp->get_player_id();
			gp->set_chess(m_players[i].m_chess);
			return true;
		}
	}

	return false;
}

bool board::exit(game_player * gp)
{
	for (int i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == gp->get_player_id())
		{
			m_players[i].m_player_id = 0;
			return true;
		}
	}


	return false;
}

unsigned board::player_count()
{
	unsigned count = 0;
	for (int i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id)	++count;
	}
	return count;
}
