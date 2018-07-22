#include "service.h"
#include "tcp_routine.h"
#include "tcp_routine_proxy.h"
#include "circular_buffer.h"
#include "file_writer.h"


service * service::m_service_instance = NULL;

service::service(const std::string &ipaddr, uint16_t port, const std::string &log_file):
	m_service_addr(ipaddr),
	m_service_port(port),
	m_service_state(service_state_init)
{
	m_routine_proxy = new tcp_routine_proxy();
	m_log_writer = new file_writer(log_file, 64*1024);
	m_service_instance = this;

	dup2(m_log_writer->get_fd(), STDOUT_FILENO);
	dup2(m_log_writer->get_fd(), STDERR_FILENO);
}

service::~service()
{
	delete m_routine_proxy;
	delete m_log_writer;
}

time_node * service::register_timer(int timeout, timer_call_back call_back, void * data)
{
	return m_routine_proxy->m_timer_manager.register_timer(timeout, call_back, data);
}

void service::start_service()
{
	create_listen_routine(m_service_addr.c_str(), m_service_port);

	struct sigaction        sa_interupted;

	sa_interupted.sa_flags = SA_SIGINFO;
	sigemptyset(&sa_interupted.sa_mask);
	sa_interupted.sa_sigaction = signal_handler;

	sigaction(SIGHUP, &sa_interupted, NULL);
	sigaction(SIGINT, &sa_interupted, NULL);
	sigaction(SIGQUIT, &sa_interupted, NULL);
	sigaction(SIGPIPE, &sa_interupted, NULL);
	sigaction(SIGTERM, &sa_interupted, NULL);

	m_service_state = service_state_work;
	while (m_service_state == service_state_work)
	{
		m_routine_proxy->react();
	}

	stop_service();
}

int service::stop_service()
{
	m_service_state = service_state_stop;



	return 0;
}

int service::process_msg(tcp_routine_proxy * trp, tcp_routine * tr, circular_buffer * buff)
{
	static const size_t header_len = sizeof(service_msg_header);
	if (buff->data_len() >= header_len)
	{
		service_msg_header header;
		memcpy(&header, buff->data_ptr(), sizeof(header));

		if (header.msg_size > buff->data_len())
		{
			DEBUG_LOG("process_msg recive incomplete msg msgsize=%u buflen=%u", header.msg_size, buff->data_len());
			return 0;
		}
		else if (header.msg_size < buff->data_len())
		{
			ERROR_LOG("process_msg msgheader.msgsize < buflen  msgsize=%u buflen=%u", header.msg_size, buff->data_len());
			int ret = process_msg(tr, header, buff->data_ptr() + header_len, header.msg_size - header_len);
			if (ret == 0)
			{
				buff->consume(header.msg_size);
			}
			else
			{
				ERROR_LOG("process_msg.process_msg error");
				return -1;
			}
			//return -1;
		}
		else
		{
			int ret = process_msg(tr, header, buff->data_ptr() + header_len, buff->data_len() - header_len);
			if (ret == 0)
			{
				buff->consume(header.msg_size);
			}
			else
			{
				ERROR_LOG("process_msg.process_msg error");
				return -1;
			}
		}
	}

	return 0;
}

void service::send_msg(uint64_t routine_id, const char * msg, unsigned len)
{
	tcp_routine *tr = m_routine_proxy->get_routine(routine_id);
	if (tr)
	{
		tr->append_send_data(msg, len);
	}
	else
	{
		DEBUG_LOG("send_msg unknown routine_id=%lu", routine_id);
	}
}

uint64_t service::create_listen_routine(const char * ipaddr, uint16_t port)
{

	tcp_listen_routine *tlr = tcp_listen_routine::create(m_routine_proxy, ipaddr, port);
	if (NULL == tlr)
	{
		return -1;
	}

	m_routine_proxy->add_routine(tlr);
	INFO_LOG("create_listen_routine routine_id=%lu, ip=%s, port=%u", tlr->get_routine_id(), ipaddr, port);

	return tlr->get_routine_id();
}

void service::signal_handler(int sigid, siginfo_t * si, void * unused)
{
	DEBUG_LOG("recive signal=%d", sigid);

	if (sigid == SIGPIPE || sigid == SIGHUP)
	{
		DEBUG_LOG("signal_handler ignore");
	}
	else if (sigid == SIGTERM || sigid == SIGINT)
	{
		DEBUG_LOG("signal_handler stop_service");
		service::get_instance()->m_service_state = service_state_stop;
	}
}

void service::set_user_data(uint64_t routine_id, const user_data & data)
{
	m_routine_proxy->set_user_data(routine_id, data);
}

void service::get_user_data(uint64_t routine_id, user_data & data)
{
	m_routine_proxy->get_user_data(routine_id, data);
}
