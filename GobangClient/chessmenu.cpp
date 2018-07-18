#include "chessmenu.h"
#include "chessgame.h"

chessmenu::chessmenu(chessgame * game)
{
	m_game = game;
}

int chessmenu::off_online_menu()
{
	const char *options[] = {
		"离线模式",
		"在线模式",
	};

	int len = sizeof(options) / sizeof(options[0]);
	format_menu(options, len);

	int choose = 0;
	saveinput input;
	input.get_input_int("请选择模式(0退出)：", choose, 0, len, 0);

	return choose;
}

int chessmenu::mode_menu(int choose)
{
	const char * offline[] = {
		"(离线) PVE",
		"(离线) PVP",
	};
	const char *online[] = {
		"(在线) PVP",
		"(在线) 参加比赛"
	};
	int len1 = sizeof(offline) / sizeof(offline[0]);
	int len2 = sizeof(online) / sizeof(online[0]);
	if (choose == 1)
	{
		format_menu(offline, len1);
	}
	else if (choose == 2)
	{
		int ret = m_game->my_connect();
		if (ret == -1)
		{
			system("clear");
			printf("连接服务器失败，按任意键返回。。。\n");
			char buf[16];
			fgets(buf, 10, stdin);
			return BACK_VALUE;
		}
		else
		{
			system("clear");
			printf("连接成功。。。\n");
			m_game->send_id_req();
			sleep(2);
			system("clear");
		}
		format_menu(online, len2);
		printf("ID:%u\n", m_game->get_id());
	}
	else
	{
		return BACK_VALUE;
	}

	int mode = 0;
	saveinput input;
	input.get_input_int("请选择游戏模式(0返回)：", mode, cg_mode_type_none, cg_mode_type_max - 1, 0);
	if (mode == 0)
	{
		m_game->my_close();
	}


	if (choose == 2 && mode)
	{
		mode += len1;
	}

	return mode;
}
