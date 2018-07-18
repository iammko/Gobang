#include "define.h"
#include "service.h"
#include "game_mgr.h"


int main(int argc, char *argv[])
{

	game_mgr::get_instance()->start();

	printf("this is test\n");
	return 0;
}

