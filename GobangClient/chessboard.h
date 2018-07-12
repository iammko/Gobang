#pragma once
#include "define.h"

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

	int addstep(char x, char y, unsigned int player)
	{
		chessstep step(x, y, player);
		steps.push_back(step);

		return ++stepno;
	}

	char getstepno()
	{
		return stepno;
	}

	chessstep* getlaststep()
	{
		std::list<chessstep>::reverse_iterator it = steps.rbegin();
		if (it != steps.rend)
		{
			return &(*it);
		}
		return NULL;
	}

private:
	char stepno;
	std::list<chessstep> steps;
};

class chessboard
{
public:
	chessboard()
	{
		bzero(chesses, sizeof(chesses));
		turn = cb_white_chess;
		black_id = 0;
		white_id = 0;
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
				chesses[i][j] = '+';
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
					printf("%c", chesses[i][j]);
				else
					printf("%c---", chesses[i][j]);
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

	int dostep(char x, char y, unsigned int playerid)
	{
		if (!checkxy(x, y))
		{
			printf("输入坐标有误，请重新输入\n");
			return -1;
		}
		bool ret = true;
		if (playerid == white_id && turn == cb_white_chess)
		{
			ret = movechess(x, y, cb_white_chess);
		}
		else if(playerid == black_id && turn == cb_black_chess)
		{
			ret = movechess(x, y, cb_black_chess);
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

		addstep(x, y, playerid);

		if (checkwin())
		{
			printf("%c方胜利\n", turn);
			return turn;
		}

		if (stepway.getstepno() >= cb_lenth * cb_lenth)
		{
			printf("平局\n");
			return cb_result_draw;
		}

		turn = turn == cb_black_chess ? cb_white_chess : cb_black_chess;

		return 0;
	}




	int checkwin()
	{
		if (stepway.getstepno() < 9)	return false;

		char curXO = turn == cb_black_chess ? cb_white_chess : cb_black_chess;

		static int i = 5;
		while (--i)
		{
			return false;
		}

		return true;
	}
private:
	bool checkxy(int x, int y)
	{
		return x > 0 && x <= cb_lenth && y > 0 && y <= cb_lenth;
	}

	bool movechess(char x, char y, char c)
	{
		if (chesses[y-1][x-1] != '+')	return false;

		chesses[y-1][x-1] = c;
		return true;
	}

	void addstep(char x, char y, unsigned int playerid)
	{
		stepway.addstep(x, y, playerid);
	}

	chessstep* getlaststep()
	{
		return stepway.getlaststep();
	}
private:
	bool gameover;
	char turn;
	char chesses[cb_lenth][cb_lenth];
	unsigned int id;
	unsigned int black_id;
	unsigned int white_id;
	chessway stepway;
	std::list<unsigned int> judges;
};
