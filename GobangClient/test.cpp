#include "chessgame.h"
#include "mgrs.h"

int main()
{
	servermgr::getinstance()->loadconfig();

	chessgame game;

	game.start();

	return 0;
}