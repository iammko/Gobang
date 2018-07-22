#pragma once

#include "define.h"
#include "singleton.h"
#include "proto_define.h"

class game_player;
class service_msg_header;


class msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len) = 0;
};

class msg_handler_mgr : public mysingleton<msg_handler_mgr>
{
private:
	msg_handler_mgr();

	friend class mysingleton<msg_handler_mgr>;
public:
	~msg_handler_mgr();
	msg_hander* get_handler(protocol_number pn);

	void register_all();
	void register_handler(protocol_number pn, msg_hander *handler);

	void handle_msg(game_player *gp,const service_msg_header & msgheader, const char * msg, unsigned len);
private:
	typedef std::map<protocol_number, msg_hander*> handler_map_t;
	handler_map_t m_handler_mgr;
};


class cs_id_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class game_type_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class join_board_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class player_info_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class start_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class do_step_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class surrender_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class exit_board_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};

class board_list_req_handler :public msg_hander
{
public:
	virtual int done(game_player *gp, const char *msg, unsigned len);
};