#pragma once

#include "singleton.h"



struct game_player
{
	game_player()
	{

	}
	unsigned game_player;
};

class data_mgr:public mysingleton<data_mgr>
{
private:
	data_mgr();

	friend class mysingleton<data_mgr>;
public:
	~data_mgr();


};