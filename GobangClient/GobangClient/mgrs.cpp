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
			addr.ip = info->GetText();

			info = info->NextSiblingElement("port");
			if (!info)	exit(-1);
			addr.port = atoi(info->GetText());

			servers[id] = addr;

			data = data->NextSiblingElement("data");
		}
	}
}

serveraddr * servermgr::getserverinfobyid(int id)
{
	std::map<int, struct serveraddr>::iterator it = servers.find(id);
	if (it == servers.end())
	{
		return NULL;
	}

	return &(it->second);
}

servermgr::~servermgr()
{
}
