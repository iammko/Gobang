#pragma once

#include "define.h"


class chessmenu
{
public:
	void init()
	{

	}

	void showmenu()
	{
		const int prefix = 5;
		const char * options[] = {
			"五子棋",
			"(单机) PVE",
			"(单机) PVP",
			"(联机) PVP",
			"参加比赛"
		};
		int len = sizeof(options) / sizeof(options[0]);

		int maxlen = getmaxlen(options, len);

		system("clear");
		printstar(maxlen + prefix * 2);
		printf("\n\n");

		for (int i = 1; i < len; ++i)
		{
			for (int j = 0; j < prefix; ++j)
				printf(" ");
			if(i == 0)
				printf("%s\n", options[0]);
			else
				printf("%d.%s\n",i, options[i]);
		}

		printf("\n");
		printstar(maxlen + prefix * 2);
		printf("\n");
	}

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
			unsigned int tmplen = strlen(strarray[i]);
			if (maxlen < tmplen)
			{
				maxlen = tmplen;
			}
		}
		return maxlen;
	}

private:
	std::map<char, std::string> options;
};
