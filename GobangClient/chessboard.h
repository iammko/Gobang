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
		if (!m_need_init)	return;

		m_gameover = 0;
		for (int i = 0; i < cb_lenth; ++i)
		{
			for (int j = 0; j < cb_lenth; ++j)
			{
				m_chesses[i][j] = '+';
			}
		}
		m_need_init = false;
	}

	void draw();

	void show_do_step_info(int info_no);
	//-1坐标有误
	//-2对方回合
	//-3位置非空
	//cg_result_win_lost
	//cg_result_draw
	//0正常
	int do_step(char x, char y, unsigned int playerid);
	int check_win(char x, char y);


	void set_player_chess(int index, char chess);
	char get_player_chess(int index);

	void set_id(int player_index, unsigned id);
	unsigned get_id(int player_index);
private:
	bool check_xy(int x, int y);

	bool move_chess(char x, char y, char c);

	void add_step(char x, char y, unsigned int playerid);

	chessstep* getlaststep();

	char up_chess(char x, char y);
	char lower_chess(char x, char y);
	char left_chess(char x, char y);
	char right_chess(char x, char y);
	char up_left_chess(char x, char y);
	char up_right_chess(char x, char y);
	char lower_left_chess(char x, char y);
	char lower_right_chess(char x, char y);
	char count_up2down(char x, char y);
	char count_left2right(char x, char y);
	char count_upleft2lowright(char x, char y);
	char count_lowleft2upright(char x, char y);

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

	bool m_need_init;
};
