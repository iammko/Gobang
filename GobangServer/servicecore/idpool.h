#pragma once
#include "define.h"

class idmgr
{
public:
	idmgr();
	~idmgr();
	void init();

	unsigned int fetchid();
	void backid(unsigned int id);





private:
	std::set<unsigned int> ids;
};