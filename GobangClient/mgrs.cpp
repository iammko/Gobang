#include "mgrs.h"
#include "tinyxml2.h"

servermgr::servermgr()
{
}

void servermgr::loadconfig()
{
	{
		using namespace tinyxml2;
		XMLDocument xml;
		XMLError errxml = xml.LoadFile("./xml/serverip.xml");
		if (XML_SUCCESS != errxml)
		{
			printf("xmlerrno:%d\n", errxml);
			perror("load serverip.xml failed");
			exit(-1);
		}

		XMLElement* root = xml.RootElement();
		if (!root)
		{
			perror("serverip.xml no root");
			exit(-1);
		}

		XMLElement* data = root->FirstChildElement("data");
		if (!data)
		{
			perror("serverip.xml no data");
			exit(-1);
		}

		while (data)
		{
			int id = 0;
			struct serveraddr addr;
			XMLElement * info = data->FirstChildElement("id");
			if (!info) exit(-1);
			id = atoi(info->GetText());

			info = info->NextSiblingElement("ip");
			if (!info)	exit(-1);
			addr.m_ip = info->GetText();

			info = info->NextSiblingElement("port");
			if (!info)	exit(-1);
			addr.m_port = atoi(info->GetText());

			m_servers[id] = addr;

			data = data->NextSiblingElement("data");
		}
	}
}

serveraddr * servermgr::get_serverinfo_byid(int id)
{
	std::map<int, struct serveraddr>::iterator it = m_servers.find(id);
	if (it == m_servers.end())
	{
		return NULL;
	}

	return &(it->second);
}

servermgr::~servermgr()
{
}
