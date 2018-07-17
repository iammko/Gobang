#pragma once

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

	static int do_loop(void *arg);
	void react();
	void process_events(uint64_t routine_id, uint32_t events);
	void inspect();

	void set_user_data(uint64_t routine_id, const user_data&data);
	void get_user_data(uint64_t routine_id, user_data&data);
protected:
	int m_epoll_fd;
	bool m_auto_work;

	typedef std::map<uint64_t, tcp_routine *> tcp_routine_mgr;
	tcp_routine_mgr m_routine_mgr;
};