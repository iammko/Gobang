#pragma once
#include "define.h"


class saveinput
{
public:
	saveinput() 
	{
		bzero(buf, sizeof(buf));
	}
	~saveinput() {}

	int inputxy(int &x, int &y)
	{
		while (1)
		{
			x = y = 0;
			if (tipinputint("请输入x：", x) != 0)
			{
				continue;
			}

			if (tipinputint("请输入y：", y) != 0)
			{
				continue;
			}

			return 0;
		}

		return -1;
	}

	int tipinputint(const char * ptip, int &rno)
	{
		printf("%s", ptip);
		int ret = input2no();
		if (ret < 0)
		{
			printf("invalid input, please input right number!\n");
			return -1;
		}
		rno = ret;
		return 0;
	}

	int getinputint(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0)
	{
		int tmpno = 0;
		while (1)
		{
			int ret = tipinputint(ptip, tmpno);
			if (ret == 0)
			{
				if ((tmpno > low && tmpno <= up )|| tmpno == quit)
					break;
				else
					printf("请输入正确的数！\n");

			}
		}
		rno = tmpno;
		return 0;
	}
private:
	int input2no()
	{
		bzero(buf, sizeof(buf));
		fflush(stdin);
		char *ret = fgets(buf, sizeof(buf), stdin);
		if (NULL == ret)
		{
			return -1;
		}

		if (strlen(buf) > 10)
		{
			return -2;
		}

		for (int i = 0; i < (int)strlen(buf); ++i)
		{
			if (i != 0 && buf[i] == '\n')	continue;
			if (!isdigit(buf[i]))	return -3;
		}

		return atoi(buf);
	}

private:
	char buf[4096];
};