#pragma once
#include "define.h"
#include "circular_buffer.h"

class tcp_routine_proxy;


class tcp_routine
{
public:
	tcp_routine(tcp_routine_proxy *tep, int fd);
	virtual ~tcp_routine();

	static int64_t gen_unique_id()
	{
		return __sync_fetch_and_add(&s_unique_id, 1);
	}

	int setnonblock()
	{
		return fcntl(m_fd, F_SETFL, O_NONBLOCK);
	}

	virtual uint32_t get_events() = 0;

	virtual int on_write() = 0;
	virtual int on_read() = 0;
	virtual void on_peer_close() = 0;
	virtual void on_hangup() = 0;
	virtual void on_error() = 0;

	void myclose()
	{
		if (m_fd >= 0)
		{
			close(m_fd);
			m_fd = -1;
		}
	}

protected:
	friend class tcp_routine_proxy;

protected:
	static long long s_unique_id;

	tcp_routine_proxy *m_routine_proxy;
	uint64_t m_routine_id;
	int m_fd;
	circular_buffer *m_send;
	circular_buffer *m_recv;

	bool m_del_flag;
	bool m_writable;
};

class tcp_listen_routine :public tcp_routine
{
private:
	tcp_listen_routine(tcp_routine_proxy *tep, int fd);
public:
	static tcp_listen_routine * create(tcp_routine_proxy *tep, const char *ipaddr, uint16_t port);

	virtual uint32_t get_events();

	virtual int on_read();
};

class tcp_session_routine : public tcp_routine
{
private:
	tcp_session_routine(tcp_routine_proxy *tep, int fd);
public:
	static tcp_session_routine * create(tcp_routine_proxy *tep, int fd);

	virtual uint32_t get_events();

	virtual int on_read();
	virtual int on_write();
	virtual void on_peer_close();
	virtual void on_hangup();
	virtual void on_error();

};