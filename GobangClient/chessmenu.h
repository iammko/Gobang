#pragma once

#include "define.h"
#include "saveinput.h"

class chessgame;

#define BACK_VALUE 0

class chessmenu
{
public:
	chessmenu(chessgame *game);
	
	void init();

	int off_online_menu();

	int mode_menu(int choose);
	bool continue_menu();
	void id_chess_menu();
	bool start_oder_menu();

	bool sure_or_not_menu(const char *tips);

	void result_menu(int result, int win_id);
	int connection_failed();
private:
	void print_star(int n)
	{
		for (int i = 0; i < n; ++i)
		{
			printf("*");
		}
	}

	int get_maxlen(const char *strarray[], int len)
	{
		int maxlen = 0;
		for (int i = 0; i < len; ++i)
		{
			int tmplen = strlen(strarray[i]);
			if (maxlen < tmplen)
			{
				maxlen = tmplen;
			}
		}
		return maxlen;
	}

	void format_menu(const char *options[], int len)
	{
		int maxlen = get_maxlen(options, len);

		const int prefix = 5;
		system("clear");
		print_star(maxlen + prefix * 2);
		printf("\n\n");

		for (int i = 0; i < prefix; ++i)
			printf(" ");
		printf("五子棋\n");
		for (int i = 0; i < len; ++i)
		{
			for (int j = 0; j < prefix; ++j)
				printf(" ");
			printf("%d.%s\n", i+1, options[i]);
		}

		printf("\n");
		print_star(maxlen + prefix * 2);
		printf("\n");
	}
private:
	chessgame * m_game;
};
