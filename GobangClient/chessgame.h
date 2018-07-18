#pragma once
#include "define.h"

#include "tcpsock.h"
#include "chessmenu.h"
#include "chessboard.h"
#include "proto_define.h"

class circular_buffer;

class chessgame
{
public:
	chessgame(); 
	~chessgame();

	void start();

	bool start_game(const char mode);

	int game_off_pve();
	int game_off_pvp();
	int game_online_quickstart();
	int game_online_race();

	int get_inputint(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0);

	int my_connect(int serverid = 1);
	int my_close();

	int send_id_req();
	int send_game_type_req();

	void set_id(unsigned id);
	unsigned get_id();
private:
	unsigned m_player_id;
	cg_mode_type m_game_type;
	tcpsock m_sock;
	chessmenu m_menu;
	chessboard m_board;
};