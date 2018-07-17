#pragma once

#include <signal.h>

#include "define.h"

class tcp_ep;
class tcp_routine;
class tcp_routine_proxy;
class circular_buffer;
class file_writer;

class user_data;

#pragma	pack(1)
struct	service_msg_header 
{
	service_msg_header() :msg_size(0)
	{
	}

	service_msg_header& operator=(const service_msg_header &smh)
	{
		this->msg_size = smh.msg_size;
		return	*this;
	}

	service_msg_header(const service_msg_header &smh)
	{
		this->msg_size = smh.msg_size;
	}
	uint32_t	msg_size;
};
#pragma	pack()

class service
{
public:
	service(const std::string &ipaddr, uint16_t port, const std::string &log_file);
	virtual ~service();
	static service *get_instance()
	{
		return m_service_instance;
	}

	virtual void start_service();
	virtual int stop_service();
	virtual int process_msg(tcp_routine_proxy *trp, tcp_routine *tr, circular_buffer *buff);
	virtual int process_msg(tcp_routine *tr, service_msg_header &msgheader, const char *msg, unsigned len) = 0;

	virtual	void	on_peer_close(tcp_routine_proxy* proxy, routine* r) = 0;
	virtual	void	on_hangup(tcp_routine_proxy* proxy, routine* r) = 0;
	virtual	void	on_routine_error(tcp_routine_proxy* proxy, routine* r) = 0;
	virtual	void	on_routine_created(uint64_t routine_id) = 0;
	virtual	void	on_routine_closed(tcp_routine_proxy* rt) = 0;

	void send_msg(uint64_t routine_id, const char *msg, unsigned len);

	uint64_t create_listen_routine(const char *ipaddr, uint16_t port);

	static void signal_handler(int sigid, siginfo_t *si, void *unused);

	void set_user_data(uint64_t routine_id, const user_data&data);
	void get_user_data(uint64_t routine_id, user_data&data);
private:
	static service * m_service_instance;

	tcp_routine_proxy * m_routine_proxy;
	std::string m_service_addr;
	uint16_t m_service_port;

	file_writer *m_log_writer;

	enum	service_state {
		service_state_init = 0,
		service_state_work = 1,
		service_state_stop = 2
	}m_service_state;
};