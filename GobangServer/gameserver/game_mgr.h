#pragma once

#include "singleton.h"

class tcp_routine;

class game_mgr : public mysingleton<game_mgr>
{
private:
	game_mgr();

	friend class mysingleton<game_mgr>;
public:
	~game_mgr();

	void start();
	void handle_msg(tcp_routine *tr, const char *msg, unsigned len);
};