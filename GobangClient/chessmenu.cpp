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
			connection_failed();
			DEBUG_LOG("connect failed");
			return BACK_VALUE;
		}
		else if(ret == 0)
		{
			system("clear");
			printf("连接成功。。。\n");
			m_game->send_id_req();
			sleep(1);
			system("clear");
		}
		format_menu(online, len2);
		printf("ID:%u\n", m_game->get_my_id());
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

bool chessmenu::continue_menu()
{

	return false;
}

void chessmenu::id_chess_menu()
{
	printf("桌号：%d\n", m_game->get_board_id());
	unsigned other_id = m_game->get_other_id();
	char my_chess = m_game->get_player_chess(m_game->get_my_id());
	char other_chess = m_game->get_player_chess(m_game->get_other_id());

	printf("你的ID：%d	", m_game->get_my_id());
	if (my_chess)	printf("棋子：%c", my_chess);
	printf("\n");
	if (other_id)
	{
		printf("对方ID：%d	", other_id);
		if (other_chess)	printf("棋子：%c", other_chess);
		printf("\n");
	}
}

bool chessmenu::start_oder_menu()
{
	id_chess_menu();
	printf("%c方先行\n", m_game->get_turn_chess());

	return true;
}

bool chessmenu::sure_or_not_menu(const char * tips)
{
	int choose = 0;
	saveinput input;
	input.get_input_int(tips, choose, 0, 1, 0);

	return choose;
}

void chessmenu::result_menu(int result, unsigned win_id)
{
	printf("游戏结束！\n");
	if (result == 1)
	{
		printf("玩家%d 获胜！\n", win_id);
	}
	else if (result == 2)
	{
		printf("平局！\n");
	}
	else if (result == 3)
	{
		unsigned lose = m_game->get_my_id();
		if (win_id != lose)	lose = m_game->get_other_id();
		printf("玩家%u投降了！\n", lose);
	}
	else if (result == (int)cg_result_escape)
	{
		printf("玩家逃跑了！\n");
	}
}

int chessmenu::connection_failed()
{
	system("clear");
	printf("连接服务器失败，按任意键返回。。。\n");
	char buf[16];
	fgets(buf, 10, stdin);
	return 0;
}
