#pragma once

#include "circular_buffer.h"
#include "define.h"

class tcp_routine;

class tcp_routine_proxy
{
public:
	tcp_routine_proxy();
	~tcp_routine_proxy();

	void add_routine(tcp_routine* ts);
	void close_routine(uint64_t routine_id);
	tcp_routine* get_routine(uint64_t routine_id);

	void unregister_events(tcp_routine* tr);
	void modify_events(tcp_routine *tr);

	int do_loop(void *arg);
	void react();
	void process_events(uint64_t routine_id, uint32_t events);
protected:
	int m_epoll_fd;
	bool m_auto_work;

	typedef std::map<uint64_t, tcp_routine *> tcp_sock_mgr;
	tcp_sock_mgr m_routine_mgr;
};