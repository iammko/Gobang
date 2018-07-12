#pragma once

#include "chessmenu.h"
#include "chessboard.h"
#include "saveinput.h"
#include "tcpsock.h"

class chessgame
{
public:
	void start()
	{
		while (1)
		{
			menu.showmenu();

			char mode = cg_mode_type_none;
			while (1)
			{
				getinputchar("请选择游戏模式(0退出)：", mode, cg_mode_type_none+1, cg_mode_type_max-1);

				if (mode == 0)	return;

				if (mode > cg_mode_type_none && mode < cg_mode_type_max) break;
			}

			while (1)
			{
				startgame(mode);
				char goon = 0;
				getinputchar("请选择继续(1)or返回(0)：", goon, 0, 1);
				if (goon == 0)	break;
			}
		}


	}

	bool startgame(const char mode)
	{
		if (mode == cg_mode_type_offpve)
		{
			gameoffpve();
		}
		else if (mode == cg_mode_type_offpvp)
		{
			gameoffpvp();
		}
		else if(mode == cg_mode_type_onlinepvp)
		{
			gameonlinepvp();
		}
		else if (mode == cg_mode_type_match)
		{
			gamematch();
		}
		else
		{
			return false;
		}
		return true;
	}

	int gameoffpve()
	{

	}

	int gameoffpvp()
	{
		board.init();

		char x, y;
		saveinput input;
		while (1)
		{
			board.draw();

			while (1)
			{
				input.inputxy(x, y);
				int stepret = board.dostep(x, y, 0);
				if (stepret == 0)	break;
			}

			if (board.checkwin())	break;
		}

	}

	int gameonlinepvp()
	{

	}

	int gamematch()
	{

	}

	int getinputchar(const char *ptip, char &rno, char low = 0, char up = 0xF, char quit = 0)
	{
		saveinput input;
		char tmpno = 0;
		while (1)
		{
			int ret = input.tipinputchar(ptip, tmpno);
			if (ret == 0)
			{
				if(tmpno > low && tmpno <= up || tmpno == quit)
					break;
				else
					printf("输入有误！\n");
				
			}
		}
		rno = tmpno;
		return 0;
	}

private:

	chessmenu menu;
	chessboard board;
};