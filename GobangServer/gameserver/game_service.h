#pragma once
#include "define.h"
#include "service.h"
#include "proto_define.h"

class tcp_routine;
class tcp_routine_proxy;

class service_task
{
public:
	service_task(tcp_routine* r);
	~service_task();

	void send_msg(protocol_number pn, const char *msg, unsigned len);
private:
	tcp_routine * m_routine;
};

class game_service : public service
{
public:
	game_service(const std::string &ipaddr, uint16_t port, const std::string &log_file);

	virtual	void	on_peer_close(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_hangup(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_routine_error(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_routine_created(tcp_routine* r);
	virtual	void	on_routine_closed(tcp_routine_proxy* rt) {}

	virtual int		process_msg(tcp_routine *tr,const service_msg_header &msgheader, const char *msg, unsigned len);
};