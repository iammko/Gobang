#pragma once
#include "define.h"

#include "tcpsock.h"
#include "chessmenu.h"
#include "chessboard.h"

class chessgame
{
public:
	chessgame(); 

	void start();

	bool startgame(const char mode);

	int gameoffpve();
	int gameoffpvp();
	int gameonlinepvp();
	int gamematch();

	int getinputint(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0);

	int myconnect(int serverid = 1);

private:
	ctcpsock mscok;
	chessmenu mmenu;
	chessboard mboard;
};