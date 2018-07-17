#pragma once
#include "define.h"


class saveinput
{
public:
	saveinput() 
	{
		bzero(m_buf, sizeof(m_buf));
	}
	~saveinput() {}

	int input_xy(int &x, int &y)
	{
		while (1)
		{
			x = y = 0;
			if (tip_input_int("请输入x：", x) != 0)
			{
				continue;
			}

			if (tip_input_int("请输入y：", y) != 0)
			{
				continue;
			}

			return 0;
		}

		return -1;
	}

	int tip_input_int(const char * ptip, int &rno)
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

	int get_input_int(const char *ptip, int &rno, int low = 0, int up = INT_MAX, int quit = 0)
	{
		int tmpno = 0;
		while (1)
		{
			int ret = tip_input_int(ptip, tmpno);
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
		bzero(m_buf, sizeof(m_buf));
		fflush(stdin);
		char *ret = fgets(m_buf, sizeof(m_buf), stdin);
		if (NULL == ret)
		{
			return -1;
		}

		if (strlen(m_buf) > 10)
		{
			return -2;
		}

		for (int i = 0; i < (int)strlen(m_buf); ++i)
		{
			if (i != 0 && m_buf[i] == '\n')	continue;
			if (!isdigit(m_buf[i]))	return -3;
		}

		return atoi(m_buf);
	}

private:
	char m_buf[4096];
};