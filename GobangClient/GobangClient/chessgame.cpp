#include "chessgame.h"
#include "chessmenu.h"
#include "chessboard.h"
#include "saveinput.h"
#include "tcpsock.h"
#include "mgrs.h"

chessgame::chessgame() : mmenu(this)
{
	mscok.createsocket();
	serveraddr *paddr = servermgr::getinstance()->getserverinfobyid(1);
	if (paddr)
	{
		mscok.setip(paddr->ip.c_str());
		mscok.setport(paddr->port);
		mscok.setaddrlen();
	}
}

void chessgame::start()
{
	while (1)
	{
		int choose = mmenu.offonlinemenu();
		if (choose == BACK_VALUE) break;

		while (1)
		{
			int mode = mmenu.modemenu(choose);
			if (mode == BACK_VALUE)	break;
			if (mode > cg_mode_type_none && mode < cg_mode_type_max)
			{
				while (startgame(mode))
				{

				}
			}
		}
	}
}

bool chessgame::startgame(const char mode)
{
	if (mode == cg_mode_type_offpve)
	{
		gameoffpve();
	}
	else if (mode == cg_mode_type_offpvp)
	{
		gameoffpvp();
	}
	else if (mode == cg_mode_type_onlinepvp)
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
	int goon = 0;
	getinputint("请选择继续(1)or返回(0)：", goon, 0, 1);

	return goon;
}

int chessgame::gameoffpve()
{
	return 0;
}

int chessgame::gameoffpvp()
{
	mboard.init();

	int x, y;
	saveinput input;
	while (1)
	{
		mboard.draw();

		int stepret = 0;
		while (1)
		{
			input.inputxy(x, y);
			stepret = mboard.dostep(x, y, 0);
			if (stepret >= 0)	break;
		}

		if (stepret) break;
	}

	return 0;
}

int chessgame::gameonlinepvp()
{
	return 0;
}

int chessgame::gamematch()
{
	return 0;
}

int chessgame::getinputint(const char * ptip, int & rno, int low, int up, int quit)
{
	saveinput input;
	return input.getinputint(ptip, rno, low, up, quit);
}

int chessgame::myconnect(int serverid)
{
	return mscok.myconnect();
}


