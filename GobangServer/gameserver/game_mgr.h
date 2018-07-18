#pragma once

#include "singleton.h"

class tcp_routine;
class service_task;

class game_mgr : public mysingleton<game_mgr>
{
private:
	game_mgr();

	friend class mysingleton<game_mgr>;
public:
	~game_mgr();

	void start();
	void loadconfig();

private:
};