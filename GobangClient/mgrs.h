#pragma once
#include "singleton.h"
#include "define.h"

struct serveraddr
{
	serveraddr()
	{
		m_id = 0;
		m_port = 0;
	}
	int m_id;
	std::string m_ip;
	uint16_t m_port;
};

class servermgr : public mysingleton<servermgr>
{
private:
	servermgr();

	friend class mysingleton<servermgr>;
public:
	void loadconfig(); 

	struct serveraddr* get_serverinfo_byid(int id);
	

	~servermgr();

private:
	std::map<int, struct serveraddr> m_servers;
};