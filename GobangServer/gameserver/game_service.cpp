#include "game_service.h"
#include "tcp_routine.h"
#include "tcp_routine_proxy.h"
#include "data_mgr.h"
#include "msg_handler_mgr.h"

service_task::service_task(tcp_routine* r):m_routine(r)
{
}

service_task::~service_task()
{
}

void service_task::send_msg(protocol_number pn, const char * msg, unsigned len)
{
	service_msg_header header;
	header.msg_type = pn;
	unsigned headerlen = sizeof(header);
	header.msg_size = len + headerlen;

	std::vector<char> bytes;
	bytes.resize(header.msg_size);
	memcpy(&bytes[0], &header, headerlen);
	memcpy(&bytes[0] + headerlen, msg, len);
	
	DEBUG_LOG("service_task.send_msg %u bytes data, %u bytes header", header.msg_size, headerlen);
	service::get_instance()->send_msg(m_routine->get_routine_id(), &bytes[0], header.msg_size);
}


game_service::game_service(const std::string & ipaddr, uint16_t port, const std::string & log_file):
	service(ipaddr, port, log_file)
{
}

void game_service::on_peer_close(tcp_routine_proxy * proxy, tcp_routine * r)
{
	data_mgr::get_instance()->remove_player(r);
}

void game_service::on_hangup(tcp_routine_proxy * proxy, tcp_routine * r)
{
	data_mgr::get_instance()->remove_player(r);
}

void game_service::on_routine_error(tcp_routine_proxy * proxy, tcp_routine * r)
{
	data_mgr::get_instance()->remove_player(r);
}

void game_service::on_routine_created(tcp_routine* r)
{
	data_mgr::get_instance()->add_player(r);
}

int game_service::process_msg(tcp_routine * tr, const service_msg_header & msgheader, const char * msg, unsigned len)
{
	game_player *gp= data_mgr::get_instance()->get_player(tr);
	if (NULL == gp)
	{
		DEBUG_LOG("game_service.process_msg unknown player routine_id=%d", tr->get_routine_id());
		return -1;
	}
	msg_handler_mgr::get_instance()->handle_msg(gp, msgheader, msg, len);

	return 0;
}

