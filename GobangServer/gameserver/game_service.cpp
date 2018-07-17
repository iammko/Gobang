#include "game_service.h"

void game_service::on_peer_close(tcp_routine_proxy * proxy, tcp_routine * r)
{
}

void game_service::on_hangup(tcp_routine_proxy * proxy, tcp_routine * r)
{
}

void game_service::on_routine_error(tcp_routine_proxy * proxy, tcp_routine * r)
{
}

void game_service::on_routine_created(uint64_t routine_id)
{
}

int game_service::process_msg(tcp_routine * tr, service_msg_header & msgheader, const char * msg, unsigned len)
{
	

	return 0;
}
