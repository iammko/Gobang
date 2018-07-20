#pragma once
#include "define.h"
#include "proto_define.h"

class chessgame;
class circular_buffer;

class tcpsock
{
public:
	tcpsock(chessgame *game);

	~tcpsock();

	void create_socket()
	{
		m_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock_fd == -1)
		{
			perror("create socket error");
			exit(-1);
		}
	}

	void set_sockfd(int fd)
	{
		m_sock_fd = fd;
	}

	void set_ip(const char *pip)
	{
		m_addr.sin_addr.s_addr = inet_addr(pip);
	}

	void set_port(int port)
	{
		m_addr.sin_port = htons(port);
	}

	void set_addrlen()
	{
		m_addrlen = sizeof(m_addr);
	}

	int set_nonblock()
	{
		return fcntl(m_sock_fd, F_SETFL, O_NONBLOCK);
	}

	int do_proto(protocol_number pn, const char *msg, unsigned len);
	int recv_proto();

	int send_msg(protocol_number pn, const char *buff, unsigned len);
	int process_msg(protocol_number pn, const char *buff, unsigned len);

	int my_read(char *buff, unsigned len)
	{
		return read(m_sock_fd, buff, len);
	}

	int my_close()
	{
		return close(m_sock_fd);
	}

	int my_connect()
	{
		return connect(m_sock_fd, (sockaddr*)&m_addr, m_addrlen);
	}

private:
	friend class chessgame;
	chessgame* m_game;

	int m_sock_fd;
	socklen_t m_addrlen;
	struct sockaddr_in m_addr;

	circular_buffer *m_recv;
};