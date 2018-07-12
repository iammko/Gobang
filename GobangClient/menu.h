#pragma once

#include <stdio.h>

#include "saveinput.h"
#include "chessboard.h"

class menu
{
public:
	int getxymenu(char &x, char &y)
	{
		saveinput myinput;

		int i = 50;
		while (--i)
		{
			x = y = 0;
			printf("请输入x:");
			int ret = myinput.input2no();
			if (ret < 0)
			{
				printf("invalid input, please input right number!\n");
				continue;
			}
			x = ret;

			printf("请输入y:");
			ret = myinput.input2no();
			if (ret < 0)
			{
				printf("invalid input, please input right number!\n");
				continue;
			}
			y = ret;

			if (x > cb_lenth || y > cb_lenth || x < 1 || y < 1)
			{
				printf("输入坐标有误，请重新输入\n");
				continue;
			}

			return 0;
		}

		return -1;
	}



private:


private:

};
