#include "tcp_routine.h"
#include "service.h"


long long tcp_routine::s_unique_id = 1;

tcp_routine::tcp_routine(tcp_routine_proxy *tep, int fd) :
	m_routine_proxy(tep),
	m_fd(fd),
	m_del_flag(false),
	m_writable(false)
{
	m_send = new circular_buffer(128 * 1024);
	m_recv = new circular_buffer(128 * 1024);
}

tcp_routine::~tcp_routine()
{

	myclose();
	delete m_send;
	delete m_recv;
}


tcp_listen_routine::tcp_listen_routine(tcp_routine_proxy * tep, int fd):tcp_routine(tep, fd)
{

}

tcp_listen_routine * tcp_listen_routine::create(tcp_routine_proxy * tep, const char * ipaddr, uint16_t port)
{
	struct sockaddr_in  addr = { AF_INET, htons(port),{ 0u },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

	if (0 == inet_aton(ipaddr, &addr.sin_addr))
	{
		ERROR_LOG("inet_aton error");
		return NULL;
	}

	int listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == listen_fd)
	{
		ERROR_LOG("create socket error");
		return NULL;
	}

	if (-1 == fcntl(listen_fd, F_SETFL, O_NONBLOCK))
	{
		ERROR_LOG("fcntl error");
		close(listen_fd);
		return NULL;
	}

	int reuse_port = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));
	if (-1 == bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)))
	{
		ERROR_LOG("bind error");
		close(listen_fd);
		return NULL;
	}

	if (-1 == listen(listen_fd, 800))
	{
		ERROR_LOG("listen error");
		close(listen_fd);
		return NULL;
	}

	tcp_listen_routine *lssock = new tcp_listen_routine(tep, listen_fd);
	if (NULL == lssock)
	{
		ERROR_LOG("new stcpsock error");
		close(listen_fd);
		return NULL;
	}

	return lssock;
}

uint32_t tcp_listen_routine::get_events()
{
	return return EPOLLIN;
}

int tcp_listen_routine::on_read()
{
	while (true)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int conn_fd = accept(m_fd, (sockaddr *)&addr, &len);
		if (conn_fd >= 0)
		{
			tcp_session_routine * tsr = new tcp_session_routine(m_routine_proxy, conn_fd);
			if (NULL == tsr)
			{
				ERROR_LOG("on_read.accept bad_alloc for conn_fd ");
				close(conn_fd);
			}
			else
			{
				DEBUG_LOG("on_read.accept success conn_fd");
				m_routine_proxy->add_routine(tsr);
			}
			continue;
		}

		if (errno == EAGAIN || errno EWOULDBLOCK)
		{
			DEBUG_LOG("on_read accept completed");
			break;
		}
		else
		{
			ERROR_LOG("on_read accept errno occurred");
		}
	}

	return 0;
}


tcp_session_routine::tcp_session_routine(tcp_routine_proxy * tep, int fd):tcp_routine(tep, fd)
{

}

tcp_session_routine * tcp_session_routine::create(tcp_routine_proxy * tep, int fd)
{
	if (-1 == fcntl(fd, F_SETFL, O_NONBLOCK))
	{
		ERROR_LOG("create tcp_session_routine error");
		close(fd);
		return NULL;
	}

	return new tcp_session_routine(tep, fd);
}

uint32_t tcp_session_routine::get_events()
{
	uint32_t events = EPOLLIN | EPOLLRDHUP;

	if (m_send->data_len() > 0)
	{
		events |= EPOLLOUT;
	}
	return events;
}

int tcp_session_routine::on_read()
{
	int ret = 0;
	char buff[64 * 1024] = { 0 };
	unsigned max_recv = m_recv->unused_len();
	if (max_recv > sizeof(buff))
	{
		max_recv = sizeof(buff);
	}

	int len = read(m_fd, buff, max_recv);
	if (len > 0)
	{
		if (m_recv->append(buff, len))
		{
			DEBUG_LOG("on_read.read append buff");
		}
		else
		{
			DEBUG_LOG("on_read.read append buff failed");
		}

		ret = len;
	}
	else if (len == 0)
	{
		DEBUG_LOG("on_read.read peer close");
		ret = -1;
	}
	else if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		DEBUG_LOG("on_read.read nothing");
		ret = 1;
	}
	else
	{
		ERROR_LOG("on_read.read error occured errno");
		ret = -1;
	}

	if (ret > 0)
	{
		ret = service::getinstance()->process_msg(m_routine_proxy, this, m_recv);
	}
	else
	{
		service::getinstance()->process_msg(m_routine_proxy, this, m_recv);
	}

	return ret;
}

int tcp_session_routine::on_write()
{
	return 0;
}

int tcp_session_routine::on_peer_close()
{
	DEBUG_LOG("peer close connection");
}

int tcp_session_routine::on_hangup()
{
	DEBUG_LOG("peer hangup");
}

int tcp_session_routine::on_error()
{
	DEBUG_LOG("error happend");
}

