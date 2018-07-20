#pragma once
#include "define.h"

#include "chessboard.h"
#include "tcpsock.h"
#include "chessmenu.h"
#include "proto_define.h"

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

	int game_do_input_xy_send();

	int get_inputint(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0);

	int my_connect(int serverid = 1);
	int my_close();


	// return 1 success, 0 failed, -1 close
	int send_id_req();
	int send_game_type_req();
	int send_board_join_req(unsigned room_id = 0);
	int send_player_info_req();
	int send_start_req();
	int send_do_step_req(int x, int y);
	int send_surrender_req();
	int send_exit_board_req();

	void set_my_id(unsigned id);
	unsigned get_my_id();
	void set_other_id(unsigned id);
	unsigned get_other_id();

	void set_board_id(unsigned id);
	unsigned get_board_id();

	void exit_board();

	void set_turn_chess(char chess);
	char get_turn_chess();
	void set_player_chess(unsigned player_id, char chess);
	char get_player_chess(unsigned player_id);

	int do_step(int x, int y, unsigned player_id);
	void result_menu(int result, int win_id = 0);

	void set_game_over(int over = 1);
	int get_game_over();

	unsigned get_step_no();

	void set_state(cg_player_state_type type);
	cg_player_state_type get_state();
private:
	friend class tcpsock;
	friend class chessmenu;
	unsigned m_player_id;
	cg_mode_type m_game_type;
	tcpsock m_sock;
	chessmenu m_menu;
	chessboard m_board;
	cg_player_state_type m_state;
	bool m_connected_flag;
};