#pragma once

#include "define.h"
#include "protos.pb.h"
#include "proto_define.h"
#include "timer.h"

class game_player;

struct chess_player
{
	chess_player()
	{
		bzero(this, sizeof(*this));
	}
	unsigned m_player_id;
	char m_chess;
	bool m_ready;
};

struct chessstep
{
	chessstep()
	{
		bzero(this, sizeof(*this));
	}
	chessstep(char rx, char ry, unsigned int rplayer) :x(rx), y(ry), playerid(rplayer)
	{
	}
	char x, y;
	unsigned int playerid;
};

class chess_way
{
public:
	chess_way()
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

	void clear()
	{
		stepno = 0;
		steps.clear();
	}

private:
	char stepno;
	std::list<chessstep> steps;
};

class board
{
public:
	board(unsigned room_id, unsigned board_id);
	~board();

	void init();
	void turn_chess_index();

	bool join(game_player *gp);
	bool exit(game_player *gp);

	bool send_info_each(game_player *src);
	bool send_start_ret();
	bool send_do_step_ret(game_player *src,const proto::step_info *step);
	bool send_surrender_ret(game_player *gp);

	unsigned player_count();
	bool all_ready();
	void set_ready(unsigned player_id,bool is_ready = true);
	void reset_ready();

	void set_game_over(cg_result_type type);
	cg_result_type get_game_over();
	void set_all_state(game_player *gp, cg_player_state_type state);

	int do_step(char x, char y, unsigned playerid);
	int do_bot_step();
	int check_win(char x, char y);

	game_player *get_another_player(game_player *src);

	void send_msg_all(protocol_number pn, const char *msg, const unsigned len);
	void send_msg_another(game_player *src, protocol_number pn, const char *msg, const unsigned len);
	void send_msg_judges(protocol_number pn, const char *msg, const unsigned len);
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

	friend class game_room;

	int m_game_state;
	unsigned m_room_id;
	unsigned m_board_id;
	int m_gameover;
	char m_turn_chess;
	unsigned m_turn_index;
	chess_player m_players[2];
	char m_chesses[cb_lenth][cb_lenth];
	chess_way m_chess_way;
	std::list<chess_player> m_judges;
};