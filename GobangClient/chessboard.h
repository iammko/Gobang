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

		if (checkwin(x, y))
		{
			draw();
			printf("%c方胜利\n", turn);
			return turn;
		}

		if (stepway.getstepno() >= cb_lenth * cb_lenth)
		{
			draw();
			printf("平局\n");
			return cb_result_draw;
		}

		turn = turn == cb_black_chess ? cb_white_chess : cb_black_chess;

		return 0;
	}

	int checkwin(char x, char y)
	{
		const char wincount = 5-1;

		if (stepway.getstepno() < 1)	return false;

		if (countup2down(x, y) > wincount || countleft2right(x, y) > wincount || countlowleft2upright(x, y) > wincount || countupleft2lowright(x, y) > wincount)
			return true;


		return false;
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

	char upchess(char x, char y)
	{
		--y;
		if (y)
			return chesses[y-1][x-1];
		return 0;
	}

	char lowerchess(char x, char y)
	{
		++y;
		if (y <= cb_lenth)
			return chesses[y-1][x-1];
		return 0;
	}

	char leftchess(char x, char y)
	{
		--x;
		if(x)
			return chesses[y-1][x-1];
		return 0;
	}

	char rightchess(char x, char y)
	{
		++x;
		if (x <= cb_lenth)
			return chesses[y-1][x-1];
		return 0;
	}

	char upleftchess(char x, char y)
	{
		--x;
		--y;
		if(x && y)
			return chesses[y-1][x-1];
		return 0;
	}

	char uprightchess(char x, char y)
	{
		--y;
		++x;
		if(x <= cb_lenth && y)
			return chesses[y-1][x-1];
		return 0;
	}

	char lowerleftchess(char x, char y)
	{
		++y;
		--x;
		if (x && y <= cb_lenth)
			return chesses[y-1][x-1];
		return 0;
	}

	char lowerrightchess(char x, char y)
	{
		++y;
		++x;
		if (x <= cb_lenth && y <= cb_lenth)
			return chesses[y-1][x-1];
		return 0;
	}

	char countup2down(char x, char y)
	{
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (upchess(x, tmpy) != turn)
				break;
			--tmpy;
			++count;
		}

		tmpy = y;
		while (1)
		{
			if (lowerchess(x, tmpy) != turn)
				break;
			++tmpy;
			++count;
		}

		return count;
	}

	char countleft2right(char x, char y)
	{
		char tmpx = x;
		char count = 1;
		while (1)
		{
			if (leftchess(tmpx, y) != turn)
				break;
			--tmpx;
			++count;
		}

		tmpx = x;
		while (1)
		{
			if (rightchess(tmpx, y) != turn)
				break;
			++tmpx;
			++count;
		}

		return count;
	}

	char countupleft2lowright(char x, char y)
	{
		char tmpx = x;
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (upleftchess(tmpx, tmpy) != turn)
				break;
			--tmpx;
			--tmpy;
			++count;
		}

		tmpx = x;
		tmpy = y; 
		while (1)
		{
			if (lowerrightchess(tmpx, tmpy) != turn)
				break;
			++tmpx;
			++tmpy;
			++count;
		}

		return count;
	}

	char countlowleft2upright(char x, char y)
	{
		char tmpx = x;
		char tmpy = y;
		char count = 1;
		while (1)
		{
			if (lowerleftchess(tmpx, tmpy) != turn)
				break;
			--tmpx;
			++tmpy;
			++count;
		}

		tmpx = x;
		tmpy = y;
		while (1)
		{
			if (uprightchess(tmpx, tmpy) != turn)
				break;
			++tmpx;
			--tmpy;
			++count;
		}

		return count;
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