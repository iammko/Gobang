#include "chessgame.h"
#include "chessmenu.h"
#include "chessboard.h"
#include "saveinput.h"
#include "tcpsock.h"
#include "mgrs.h"

chessgame::chessgame() : m_menu(this)
{
	m_sock.createsocket();
	serveraddr *paddr = servermgr::getinstance()->get_serverinfo_byid(1);
	if (paddr)
	{
		m_sock.setip(paddr->m_ip.c_str());
		m_sock.setport(paddr->m_porto);
		m_sock.setaddrlen();
	}
}

void chessgame::start()
{
	while (1)
	{
		int choose = m_menu.off_online_menu();
		if (choose == BACK_VALUE) break;

		while (1)
		{
			int mode = m_menu.mode_menu(choose);
			if (mode == BACK_VALUE)	break;
			if (mode > cg_mode_type_none && mode < cg_mode_type_max)
			{
				while (start_game(mode))
				{

				}
			}
		}
	}
}

bool chessgame::start_game(const char mode)
{
	if (mode == cg_mode_type_offpve)
	{
		game_off_pve();
	}
	else if (mode == cg_mode_type_offpvp)
	{
		game_off_pvp();
	}
	else if (mode == cg_mode_type_onlinepvp)
	{
		game_online_quickstart();
	}
	else if (mode == cg_mode_type_match)
	{
		game_race();
	}
	else
	{
		return false;
	}
	int goon = 0;
	get_inputint("请选择继续(1)or返回(0)：", goon, 0, 1);

	return goon;
}

int chessgame::game_off_pve()
{
	return 0;
}

int chessgame::game_off_pvp()
{
	m_board.init();

	int x, y;
	saveinput input;
	while (1)
	{
		m_board.draw();

		int stepret = 0;
		while (1)
		{
			input.input_xy(x, y);
			stepret = m_board.do_step(x, y, 0);
			if (stepret >= 0)	break;
		}

		if (stepret) break;
	}

	return 0;
}

int chessgame::game_online_quickstart()
{

	return 0;
}

int chessgame::game_race()
{
	return 0;
}

int chessgame::get_inputint(const char * ptip, int & rno, int low, int up, int quit)
{
	saveinput input;
	return input.get_input_int(ptip, rno, low, up, quit);
}

int chessgame::my_connect(int serverid)
{
	return m_sock.my_connect();
}

int chessgame::my_close()
{
	return m_sock.myclose();
}


