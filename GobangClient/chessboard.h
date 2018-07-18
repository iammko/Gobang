#pragma once
#include "define.h"

class chessgame;

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
	chessboard(chessgame *game):m_game(game)
	{
		bzero(m_chesses, sizeof(m_chesses));
		m_turn = cb_white_chess;
		m_black_id = 0;
		m_white_id = 0;
	}

	~chessboard()
	{

	}

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

	void draw()
	{
		system("clear");
		printf("%5c\n", 'x');
		printf("%c", ' ');
		for (int i = 0; i < cb_lenth; ++i)
		{
				printf("%4d", i+1);
		}
		printf("\n");

		for (int i = 0; i < cb_lenth; ++i)
		{
			if (i == 0)
				printf("%c%d%2c", 'y', i+1, ' ');
			else
				printf("%2d%2c", i + 1, ' ');
			for (int j = 0; j < cb_lenth; ++j)
			{
				if(j==cb_lenth - 1)
					printf("%c", m_chesses[i][j]);
				else
					printf("%c---", m_chesses[i][j]);
			}
			printf("\n");

			if (i != cb_lenth - 1)
			{
				printf("%c", ' ');
				for (int j = 0; j < cb_lenth; ++j)
				{
					printf("%4c", '|');
				}
				printf("\n");
			}
		}
		printf("\n");
	}

	int do_step(char x, char y, unsigned int playerid)
	{
		if (!check_xy(x, y))
		{
			printf("输入坐标有误，请重新输入\n");
			return -1;
		}
		bool ret = true;
		if (playerid == m_white_id && m_turn == cb_white_chess)
		{
			ret = move_chess(x, y, cb_white_chess);
		}
		else if(playerid == m_black_id && m_turn == cb_black_chess)
		{
			ret = move_chess(x, y, cb_black_chess);
		}
		else
		{
			printf("不是你的回合\n");
			return -2;
		}

		if (!ret)
		{
			printf("位置已被占\n");
			return -3;
		}

		add_step(x, y, playerid);

		if (check_win(x, y))
		{
			draw();
			printf("%c方胜利\n", m_turn);
			return m_turn;
		}

		if (m_stepway.get_stepno() >= cb_lenth * cb_lenth)
		{
			draw();
			printf("平局\n");
			return cb_result_draw;
		}

		m_turn = m_turn == cb_black_chess ? cb_white_chess : cb_black_chess;

		return 0;
	}

	int check_win(char x, char y)
	{
		const char wincount = 5-1;

		if (m_stepway.get_stepno() < 1)	return false;

		if (count_up2down(x, y) > wincount || count_left2right(x, y) > wincount || count_lowleft2upright(x, y) > wincount || count_upleft2lowright(x, y) > wincount)
			return true;


		return false;
	}
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
	chessgame * m_game;

	bool m_gameover;
	char m_turn;
	char m_chesses[cb_lenth][cb_lenth];
	unsigned int m_board_id;
	unsigned int m_black_id;
	unsigned int m_white_id;
	chessway m_stepway;
	std::list<unsigned int> m_judges;
};
