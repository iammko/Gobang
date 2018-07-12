#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


class saveinput
{
public:
	saveinput() 
	{
		bzero(buf, sizeof(buf));
	}
	~saveinput() {}

	int inputxy(char &x, char &y)
	{
		while (1)
		{
			x = y = 0;
			if (tipinputchar("请输入x：", x) != 0)
			{
				continue;
			}

			if (tipinputchar("请输入y：", y) != 0)
			{
				continue;
			}

			return 0;
		}

		return -1;
	}

	int tipinputchar(const char * ptip, char &rno)
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

private:
	int input2no()
	{
		bzero(buf, sizeof(buf));

		char *ret = fgets(buf, sizeof(buf), stdin);
		if (NULL == ret)
		{
			return -1;
		}

		if (strlen(buf) > 10)
		{
			return -2;
		}

		for (int i = 0; i < strlen(buf); ++i)
		{
			if (i != 0 && buf[i] == '\n')	continue;
			if (!isdigit(buf[i]))	return -3;
		}

		return atoi(buf);
	}

private:
	char buf[4096];
};