#include "tcp_routine.h"
#include "service.h"
#include "circular_buffer.h"
#include "tcp_routine_proxy.h"

#include <errno.h>

#include <sys/epoll.h>



long long tcp_routine::s_unique_id = 1;

tcp_routine::tcp_routine(tcp_routine_proxy *trp, int fd) :
	m_routine_proxy(trp),
	m_fd(fd),
	m_del_flag(false),
	m_writable(false)
{
	m_send = new circular_buffer(128 * 1024);
	m_recv = new circular_buffer(128 * 1024);

	m_routine_id = gen_unique_id();
}

tcp_routine::~tcp_routine()
{

	myclose();
	delete m_send;
	delete m_recv;
}

tcp_listen_routine::tcp_listen_routine(tcp_routine_proxy * trp, int fd):tcp_routine(trp, fd)
{

}

tcp_listen_routine * tcp_listen_routine::create(tcp_routine_proxy * trp, const char * ipaddr, uint16_t port)
{
	struct sockaddr_in  addr = { AF_INET, htons(port),{ 0u },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

	if (0 == inet_aton(ipaddr, &addr.sin_addr))
	{
		ERROR_LOG("inet_aton error errno=%d errstr=%s", errno, strerror(errno));
		return NULL;
	}

	int listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == listen_fd)	
	{
		ERROR_LOG("create socket error errno=%d errstr=%s", errno, strerror(errno));
		return NULL;
	}

	if (-1 == fcntl(listen_fd, F_SETFL, O_NONBLOCK))
	{
		ERROR_LOG("fcntl error errno=%d errstr=%s", errno, strerror(errno));
		close(listen_fd);
		return NULL;
	}

	int reuse_port = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));
	if (-1 == bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)))
	{
		ERROR_LOG("bind error errno=%d errstr=%s", errno, strerror(errno));
		close(listen_fd);
		return NULL;
	}

	if (-1 == listen(listen_fd, 800))
	{
		ERROR_LOG("listen error errno=%d errstr=%s", errno, strerror(errno));
		close(listen_fd);
		return NULL;
	}

	tcp_listen_routine *tlr = new tcp_listen_routine(trp, listen_fd);
	if (NULL == tlr)
	{
		ERROR_LOG("new stcpsock error errno=%d errstr=%s", errno, strerror(errno));
		close(listen_fd);
		return NULL;
	}

	return tlr;
}

uint32_t tcp_listen_routine::get_events()
{
	return EPOLLIN;
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
			tcp_session_routine * tsr = tcp_session_routine::create(m_routine_proxy, conn_fd);
			if (NULL == tsr)
			{
				ERROR_LOG("on_read.accept bad_alloc for conn_fd\n");
				close(conn_fd);
			}
			else
			{
				service::get_instance()->on_routine_created(tsr->m_routine_id);
				DEBUG_LOG("on_read.accept success conn_fd=%d\n", conn_fd);
				m_routine_proxy->add_routine(tsr);
			}
			continue;
		}

		if (errno == EAGAIN || errno == EWOULDBLOCK)
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


tcp_session_routine::tcp_session_routine(tcp_routine_proxy * trp, int fd):tcp_routine(trp, fd)
{

}

tcp_session_routine * tcp_session_routine::create(tcp_routine_proxy * trp, int fd)
{
	if (-1 == fcntl(fd, F_SETFL, O_NONBLOCK))
	{
		ERROR_LOG("create tcp_session_routine error");
		close(fd);
		return NULL;
	}

	return new tcp_session_routine(trp, fd);
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
			DEBUG_LOG("on_read.read append buff size=%d", len);
		}
		else
		{
			DEBUG_LOG("on_read.read append buff failed size=%d", len);
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
		ERROR_LOG("on_read.read error occured errno=%d strerr=%s", errno, strerror(errno));
		ret = -1;
	}

	if (ret > 0)
	{
		ret = service::get_instance()->process_msg(m_routine_proxy, this, m_recv);
	}
	else
	{
		service::get_instance()->process_msg(m_routine_proxy, this, m_recv);
	}

	return ret;
}

int tcp_session_routine::on_write()
{
	return do_nonblock_write();
}

void tcp_session_routine::on_peer_close()
{
	DEBUG_LOG("peer close connection");
}

void tcp_session_routine::on_hangup()
{
	DEBUG_LOG("peer hangup");
}

void tcp_session_routine::on_error()
{
	DEBUG_LOG("error happend");
}

bool tcp_routine::append_send_data(const char * data, unsigned len)
{
	if (m_writable && m_send->data_len() == 0)
	{
		unsigned sent = 0;
		int ret = do_nonblock_write(data, len, sent);
		if (ret >= 0)
		{
			if (sent < len)
			{
				m_writable = false;
				return m_send->append(data + sent, len - sent);
			}
			return true;
		}
	}

	if (m_send->append(data, len))
	{
		DEBUG_LOG("append_send_data.append %u bytes data success", len);
		if (do_nonblock_write() >= 0)
		{
			if (m_send->data_len() > 0)	m_writable = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		DEBUG_LOG("append_send_data.append %u bytes data failed", len);
		return false;
	}

	return false;
}

int tcp_routine::do_nonblock_write()
{
	unsigned sent = 0;
	int result = do_nonblock_write(m_send->data_ptr(), m_send->data_len(), sent);
	if (result >= 0)
	{
		m_send->consume(sent);
		DEBUG_LOG("do_nonblock_write %d bytes, %d bytes not send", sent, m_send->data_len());
	}
	return sent;
}

int tcp_routine::do_nonblock_write(const char * buf, unsigned len, unsigned & sent)
{
	while (sent < len)
	{
		int ret = write(m_fd, buf + send, len - sent);
		if (ret >= 0)
		{
			sent += ret;
			DEBUG_LOG("on_write.write success len=%d", ret);
			continue;
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			m_writable = false;
			return 0;
		}
		else if (errno == EINTR)
		{
			ERROR_LOG("on_write.write interrupted");
			continue;
		}
		else
		{
			ERROR_LOG("on_write.write error");
			return -1;
		}
	}

	return sent;
}
