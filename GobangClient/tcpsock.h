#pragma once
#include "define.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


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

	void mcreatesocket()
	{
		msock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (msock_fd == -1)
		{
			perror("create socket error");
			exit(-1);
		}
	}

	void msetsockfd(int fd)
	{
		msock_fd = fd;
	}

	void msetip(const char *pip)
	{
		maddr.sin_addr.s_addr = inet_addr(pip);
	}

	void msetport(int port)
	{
		maddr.sin_port = htons(port);
	}

	int msetnonblock()
	{
		return fcntl(msock_fd, F_SETFL, O_NONBLOCK);
	}

	int mwrite(const char *buff, int len)
	{
		return write(msock_fd, buff, len);
	}

	int mread(char *buff, int len)
	{
		return read(msock_fd, buff, len);
	}

	int mclose()
	{
		return close(msock_fd);
	}

	int msock_fd;
	socklen_t maddrlen;
	struct sockaddr_in maddr;
};

class stcpsock :public tcpsock
{
	int mbind()
	{
		return bind(msock_fd, (sockaddr*)&maddr, maddrlen);
	}

	int mlisten(int n = 20)
	{
		return listen(msock_fd, n);
	}

	tcpsock maccept()
	{
		socklen_t peeraddrlen;
		struct sockaddr_in peeraddr;
		int peer_fd = accept(msock_fd, (sockaddr*)&peeraddr, &peeraddrlen);
		tcpsock peersock;
		peersock.msetsockfd(peer_fd);
		return peersock;
	}
};