#pragma once
#include "define.h"

class tcp_routine_proxy;
class circular_buffer;


class tcp_routine
{
public:
	tcp_routine(tcp_routine_proxy *trp, int fd);
	virtual ~tcp_routine();

	static int64_t gen_unique_id()
	{
		return __sync_fetch_and_add(&s_unique_id, 1);
	}

	int setnonblock()
	{
		return fcntl(m_fd, F_SETFL, O_NONBLOCK);
	}

	uint64_t get_routine_id()
	{
		return m_routine_id;
	}

	int do_nonblock_write();
	int do_nonblock_write(const char *buf, unsigned len, unsigned &sent);

	virtual int inspect() = 0;

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

	void set_user_data(const user_data &data)
	{
		m_ud = data;
	}
	void get_user_data(user_data&data)
	{
		data = m_ud;
	}

	bool append_send_data(const char *data, unsigned len);
protected:
	friend class tcp_routine_proxy;

protected:
	static long long s_unique_id;

	tcp_routine_proxy *m_routine_proxy;
	uint64_t m_routine_id;
	int m_fd;
	
	circular_buffer *m_send;
	circular_buffer *m_recv;

	user_data m_ud;
	bool m_del_flag;
	bool m_writable;
};

class tcp_listen_routine :public tcp_routine
{
private:
	tcp_listen_routine(tcp_routine_proxy *trp, int fd);
public:
	static tcp_listen_routine * create(tcp_routine_proxy *trp, const char *ipaddr, uint16_t port);

	virtual int inspect() { return 0; }

	virtual uint32_t get_events();

	virtual int on_read();
	virtual int on_write() { return 0; }
	virtual void on_peer_close() {}
	virtual void on_hangup() {}
	virtual void on_error() {}
};

class tcp_session_routine : public tcp_routine
{
private:
	tcp_session_routine(tcp_routine_proxy *trp, int fd);
public:
	static tcp_session_routine * create(tcp_routine_proxy *trp, int fd);

	virtual int inspect() { return 0; }

	virtual uint32_t get_events();

	virtual int on_read();
	virtual int on_write();
	virtual void on_peer_close();
	virtual void on_hangup();
	virtual void on_error();

};