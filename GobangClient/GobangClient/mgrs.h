#pragma once
#include "singleton.h"
#include "define.h"

struct serveraddr
{
	serveraddr()
	{
		id = 0;
		port = 0;
	}
	int id;
	std::string ip;
	short port;
};

class servermgr : public mysingleton<servermgr>
{
private:
	servermgr();

	friend class mysingleton<servermgr>;
public:
	void loadconfig(); 

	struct serveraddr* getserverinfobyid(int id);
	

	~servermgr();

private:
	std::map<int, struct serveraddr> servers;
};