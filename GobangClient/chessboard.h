#pragma once
#include "define.h"

class chessgame;
#define PLAYER_ME 0
#define PLAYER_OTHER 1

struct chess_player
{
	chess_player()
	{
		bzero(this, sizeof(*this));
	}
	unsigned m_player_id;
	char m_chess;
};

struct chessstep
{
	chessstep()
	{
		bzero(this, sizeof(*this));
	}
	chessstep(char rx, char ry, unsigned int rplayer):x(rx),y(ry), playerid(rplayer)
	{
	}
	char x, y;
	unsigned int playerid;
};

class chessway
{
public:
	chessway()
	{
		stepno = 0;
	}

	int add_step(char x, char y, unsigned int player)
	{
		chessstep step(x, y, player);
		steps.push_back(step);

		return ++stepno;
	}

	char get_stepno()
	{
		return stepno;
	}

	chessstep* get_laststep()
	{
		if (steps.size())
			return &(steps.back());

		return NULL;
	}

private:
	char stepno;
	std::list<chessstep> steps;
};

class chessboard
{
public:
	chessboard(chessgame *game);

	~chessboard();

	void init()
	{
		for (int i = 0; i < cb_lenth; ++i)
		{
			for (int j = 0; j < cb_lenth; ++j)
			{
				m_chesses[i][j] = '+';
			}
		}
	}

	void draw();

	int do_step(char x, char y, unsigned int playerid);
	

	int check_win(char x, char y)
	{
		const char wincount = 5-1;

		if (m_stepway.get_stepno() < 1)	return false;

		if (count_up2down(x, y) > wincount || count_left2right(x, y) > wincount || count_lowleft2upright(x, y) > wincount || count_upleft2lowright(x, y) > wincount)
			return true;


		return false;
	}

	void set_player_chess(int index, char chess);
	char get_player_chess(int index);

	void set_id(int player_index, unsigned id);
	unsigned get_id(int player_index);
private:
	bool check_xy(int x, int y)
	{
		return x > 0 && x <= cb_lenth && y > 0 && y <= cb_lenth;
	}

	bool move_chess(char x, char y, char c)
	{
		if (m_chesses[y-1][x-1] != '+')	return false;

		m_chesses[y-1][x-1] = c;
		return true;
	}

	void add_step(char x, char y, unsigned int playerid)
	{
		m_stepway.add_step(x, y, playerid);
	}

	chessstep* getlaststep()
	{
		return m_stepway.get_laststep();
	}

	char up_chess(char x, char y)
	{
		--y;
		if (y)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char lower_chess(char x, char y)
	{
		++y;
		if (y <= cb_lenth)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char left_chess(char x, char y)
	{
		--x;
		if(x)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char right_chess(char x, char y)
	{
		++x;
		if (x <= cb_lenth)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char up_left_chess(char x, char y)
	{
		--x;
		--y;
		if(x && y)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char up_right_chess(char x, char y)
	{
		--y;
		++x;
		if(x <= cb_lenth && y)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char lower_left_chess(char x, char y)
	{
		++y;
		--x;
		if (x && y <= cb_lenth)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char lower_right_chess(char x, char y)
	{
		++y;
		++x;
		if (x <= cb_lenth && y <= cb_lenth)
			return m_chesses[y-1][x-1];
		return 0;
	}

	char count_up2down(char x, char y)
	{
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (up_chess(x, tmpy) != m_turn)
				break;
			--tmpy;
			++count;
		}

		tmpy = y;
		while (1)
		{
			if (lower_chess(x, tmpy) != m_turn)
				break;
			++tmpy;
			++count;
		}

		return count;
	}

	char count_left2right(char x, char y)
	{
		char tmpx = x;
		char count = 1;
		while (1)
		{
			if (left_chess(tmpx, y) != m_turn)
				break;
			--tmpx;
			++count;
		}

		tmpx = x;
		while (1)
		{
			if (right_chess(tmpx, y) != m_turn)
				break;
			++tmpx;
			++count;
		}

		return count;
	}

	char count_upleft2lowright(char x, char y)
	{
		char tmpx = x;
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (up_left_chess(tmpx, tmpy) != m_turn)
				break;
			--tmpx;
			--tmpy;
			++count;
		}

		tmpx = x;
		tmpy = y; 
		while (1)
		{
			if (lower_right_chess(tmpx, tmpy) != m_turn)
				break;
			++tmpx;
			++tmpy;
			++count;
		}

		return count;
	}

	char count_lowleft2upright(char x, char y)
	{
		char tmpx = x;
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (lower_left_chess(tmpx, tmpy) != m_turn)
				break;
			--tmpx;
			++tmpy;
			++count;
		}

		tmpx = x;
		tmpy = y;
		while (1)
		{
			if (up_right_chess(tmpx, tmpy) != m_turn)
				break;
			++tmpx;
			--tmpy;
			++count;
		}

		return count;
	}
private:
	friend class chessgame;
	chessgame * m_game;

	int m_gameover;
	char m_turn;
	char m_chesses[cb_lenth][cb_lenth];
	unsigned int m_board_id;
	chess_player m_chess_players[2];
	chessway m_stepway;
	std::list<unsigned int> m_judges;
};
