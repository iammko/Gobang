#include "chessgame.h"
#include "mgrs.h"

int main()
{
	servermgr::get_instance()->loadconfig();

	chessgame game;

	game.start();

	return 0;
}