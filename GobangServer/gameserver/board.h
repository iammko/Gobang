#pragma once

class board
{
public:
	board();



private: 
	unsigned m_board_id;
	unsigned m_white_id;
	unsigned m_blacke_id;
	bool m_gameover;
	char m_turn_chess;
	char[cb_lenth][cb_lenth] m_chesses;
};