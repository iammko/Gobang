#pragma once
#include "service.h"

class tcp_routine;

class service_task
{
public:
	service_task();

private:
	tcp_routine * m_routine;
};

class game_service : public service
{
public:
	virtual	void	on_peer_close(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_hangup(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_routine_error(tcp_routine_proxy* proxy, tcp_routine* r);
	virtual	void	on_routine_created(uint64_t routine_id);
	virtual	void	on_routine_closed(tcp_routine_proxy* rt) {}

	virtual int process_msg(tcp_routine *tr, service_msg_header &msgheader, const char *msg, unsigned len);
};