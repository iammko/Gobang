#include "game_service.h"
#include "game_mgr.h"

int game_service::process_msg(tcp_routine * tr, service_msg_header & msgheader, const char * msg, unsigned len)
{
	game_mgr::get_instance()->handle_msg(tr, msg, len);

	return 0;
}
