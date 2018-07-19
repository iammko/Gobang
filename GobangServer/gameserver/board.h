#pragma once

#include "define.h"
#include "protos.pb.h"
#include "proto_define.h"

class game_player;

struct chess_player
{
	chess_player()
	{
		bzero(this, sizeof(*this));
	}
	unsigned m_player_id;
	char m_chess;
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
	board(unsigned board_id);
	~board();

	void init();
	void turn_chess_index();

	bool join(game_player *gp);
	bool exit(game_player *gp);

	bool send_info_each(game_player *src);
	bool send_start_ret(game_player *gp);
	bool send_do_step_ret(game_player *src,const proto::step_info *step);
	bool send_surrender_ret(game_player *gp);

	unsigned player_count();

	int do_step(char x, char y, unsigned playerid);
	int do_bot_step();
	int check_win(char x, char y);

	game_player *get_another_player(game_player *src);

	void send_msg_all(protocol_number pn, const char *msg, const unsigned len);
	void send_msg_another(game_player *src, protocol_number pn, const char *msg, const unsigned len);
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

	unsigned m_room_id;
	unsigned m_board_id;
	int m_gameover;
	char m_turn_chess;
	char m_turn_index;
	chess_player m_players[2];
	char m_chesses[cb_lenth][cb_lenth];
	chess_way m_chess_way;
	std::list<chess_player> m_judges;
};