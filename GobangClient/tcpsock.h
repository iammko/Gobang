#pragma once
#include "define.h"


class tcpsock
{
public:
	tcpsock()
	{
		m_sock_fd = -1;
		bzero(&m_addr, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		m_addrlen = 0;
	}

	~tcpsock()
	{

	}

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

	int my_write(const char *buff, int len)
	{
		return write(m_sock_fd, buff, len);
	}

	int my_read(char *buff, int len)
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

	int m_sock_fd;
	socklen_t m_addrlen;
	struct sockaddr_in m_addr;
};