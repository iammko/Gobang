#include "game_mgr.h"

game_mgr::game_mgr()
{
}

game_mgr::~game_mgr()
{
}

void game_mgr::start()
{
	msg_handler_mgr::register_all();
}

void game_mgr::handle_msg(tcp_routine * tr, const char * msg, unsigned len)
{
}


