#pragma once
#include "define.h"


class tcpsock
{
public:
	tcpsock()
	{
		msock_fd = -1;
		bzero(&maddr, sizeof(maddr));
		maddr.sin_family = AF_INET;
		maddrlen = 0;
	}

	~tcpsock()
	{

	}

	void createsocket()
	{
		msock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (msock_fd == -1)
		{
			perror("create socket error");
			exit(-1);
		}
	}

	void setsockfd(int fd)
	{
		msock_fd = fd;
	}

	void setip(const char *pip)
	{
		maddr.sin_addr.s_addr = inet_addr(pip);
	}

	void setport(int port)
	{
		maddr.sin_port = htons(port);
	}

	void setaddrlen(int len)
	{
		maddrlen = len;
	}

	int setnonblock()
	{
		return fcntl(msock_fd, F_SETFL, O_NONBLOCK);
	}

	int mywrite(const char *buff, int len)
	{
		return write(msock_fd, buff, len);
	}

	int myread(char *buff, int len)
	{
		return read(msock_fd, buff, len);
	}

	int myclose()
	{
		return close(msock_fd);
	}

	int msock_fd;
	socklen_t maddrlen;
	struct sockaddr_in maddr;
};

class stcpsock :public tcpsock
{
public:
	int mybind()
	{
		if (bind(msock_fd, (sockaddr*)&maddr, maddrlen) == -1)
		{
			perror("bind error");
			exit(-1);
		}
		return 0;
	}

	int mylisten(int n = 20)
	{
		if (listen(msock_fd, n) == -1)
		{
			perror("listen error");
			exit(-1);
		}
		return 0;
	}

	tcpsock myaccept()
	{
		socklen_t peeraddrlen;
		struct sockaddr_in peeraddr;
		int peer_fd = accept(msock_fd, (sockaddr*)&peeraddr, &peeraddrlen);
		tcpsock peersock;
		peersock.setsockfd(peer_fd);
		return peersock;
	}
};

class ctcpsock : public tcpsock
{
public:
	int myconnect()
	{
		return connect(msock_fd, (sockaddr*)&maddr, maddrlen);
	}
};