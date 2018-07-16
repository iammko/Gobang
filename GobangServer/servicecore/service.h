#pragma once

#include <signal.h>

#include "define.h"
#include "singleton.h"

class tcp_ep;
class tcp_routine;

class service :public mysingleton<service>
{
public:
	service();
	virtual ~service();

	virtual void start_service();
	virtual void process_msg(tcp_routine_proxy *trp, tcp_routine *tr, circular_buffer *buff);

	void signal_handler(int sinm, siginfo_t *si, void *unused);
private:
	tcp_routine_proxy * m_routine_proxy;
};