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

	int offonlinemenu();

	int modemenu(int choose);

private:
	void printstar(int n)
	{
		for (int i = 0; i < n; ++i)
		{
			printf("*");
		}
	}

	int getmaxlen(const char *strarray[], int len)
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

	void formatmenu(const char *options[], int len)
	{
		int maxlen = getmaxlen(options, len);

		const int prefix = 5;
		system("clear");
		printstar(maxlen + prefix * 2);
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
		printstar(maxlen + prefix * 2);
		printf("\n");
	}
private:
	chessgame * pgame;
	std::map<char, std::string> options;
};
