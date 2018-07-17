#pragma once

#include "singleton.h"
#include "proto_define.h"


class msg_hander
{
public:
	virtual done(game_player *gp, const char *msg, unsigned len) = 0;
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
private:
	typedef std::map<protocol_number, msg_hander*> handler_map_t;
	handler_map_t m_handler_mgr;
};