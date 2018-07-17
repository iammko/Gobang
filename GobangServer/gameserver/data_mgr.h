#pragma once

#include "singleton.h"

#include <map>

struct game_player
{
	game_player()
	{
		m_player_id = 0;
	}
	unsigned m_player_id;
};

class data_mgr:public mysingleton<data_mgr>
{
private:
	data_mgr();

	friend class mysingleton<data_mgr>;
public:
	~data_mgr();


private:
	typedef std::map<uint64_t, game_player*> game_player_map_t;
	game_player_map_t game_player_mgr;
};