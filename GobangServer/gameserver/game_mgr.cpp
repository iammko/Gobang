#include "game_mgr.h"
#include "game_service.h"
#include "msg_handler_mgr.h"
#include "game_room.h"
#include "data_mgr.h"

game_mgr::game_mgr()
{
}

game_mgr::~game_mgr()
{
}

void game_mgr::start()
{
	loadconfig();

	game_service server("127.0.0.1", 3601, "log");
	server.start_service();
}

void game_mgr::loadconfig()
{
	msg_handler_mgr::get_instance()->register_all();
	game_room_mgr::get_instance()->load_config();
	data_mgr::get_instance()->load_config();
}


