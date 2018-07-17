#pragma once
#include "define.h"

#include "tcpsock.h"
#include "chessmenu.h"
#include "chessboard.h"

class chessgame
{
public:
	chessgame(); 

	void start();

	bool start_game(const char mode);

	int game_off_pve();
	int game_off_pvp();
	int game_online_quickstart();
	int game_race();

	int get_inputint(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0);

	int my_connect(int serverid = 1);
	int my_close();


private:
	tcpsock m_sock;
	chessmenu m_menu;
	chessboard m_board;
};