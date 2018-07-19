#include "chessboard.h"


chessboard::chessboard(chessgame * game) : m_game(game)
{
	bzero(m_chesses, sizeof(m_chesses));
	m_turn = cb_white_chess;
}

chessboard::~chessboard()
{
}

void chessboard::draw()
{
	system("clear");
	printf("%5c\n", 'x');
	printf("%c", ' ');
	for (int i = 0; i < cb_lenth; ++i)
	{
		printf("%4d", i + 1);
	}
	printf("\n");

	for (int i = 0; i < cb_lenth; ++i)
	{
		if (i == 0)
			printf("%c%d%2c", 'y', i + 1, ' ');
		else
			printf("%2d%2c", i + 1, ' ');
		for (int j = 0; j < cb_lenth; ++j)
		{
			if (j == cb_lenth - 1)
				printf("%c", m_chesses[i][j]);
			else
				printf("%c---", m_chesses[i][j]);
		}
		printf("\n");

		if (i != cb_lenth - 1)
		{
			printf("%c", ' ');
			for (int j = 0; j < cb_lenth; ++j)
			{
				printf("%4c", '|');
			}
			printf("\n");
		}
	}
	printf("\n");
}

int chessboard::do_step(char x, char y, unsigned int playerid)
{
	if (!check_xy(x, y))
	{
		printf("输入坐标有误，请重新输入\n");
		return -1;
	}
	bool ret = true;
	bool move_flag = false;
	for (int i = 0; i < sizeof(m_chess_players) / sizeof(m_chess_players[0]); ++i)
	{
		if (m_turn == m_chess_players[i].m_chess && playerid == m_chess_players[i].m_player_id)
		{
			ret = move_chess(x, y, m_turn);
			move_flag = true;
		}
	}

	if (!move_flag)
	{
		printf("不是你的回合\n");
		return -2;
	}

	if (!ret)
	{
		printf("位置已被占\n");
		return -3;
	}

	add_step(x, y, playerid);

	if (check_win(x, y))
	{
		draw();
		printf("%c方胜利\n", m_turn);
		return m_turn;
	}

	if (m_stepway.get_stepno() >= cb_lenth * cb_lenth)
	{
		draw();
		printf("平局\n");
		return cg_result_draw;
	}

	m_turn = m_turn == cb_black_chess ? cb_white_chess : cb_black_chess;

	return 0;
}

int chessboard::check_win(char x, char y)
{
	const char wincount = 5 - 1;

	if (m_stepway.get_stepno() < 1)	return false;

	if (count_up2down(x, y) > wincount || count_left2right(x, y) > wincount || count_lowleft2upright(x, y) > wincount || count_upleft2lowright(x, y) > wincount)
		return true;


	return false;
}

void chessboard::set_player_chess(int index, char chess)
{
	m_chess_players[index].m_chess = chess;
}

char chessboard::get_player_chess(int index)
{
	return m_chess_players[index].m_chess;
}

void chessboard::set_id(int player_index, unsigned id)
{
	m_chess_players[player_index].m_player_id = id;
}

unsigned chessboard::get_id(int player_index)
{
	return m_chess_players[player_index].m_player_id;
}

bool chessboard::check_xy(int x, int y)
{
	return x > 0 && x <= cb_lenth && y > 0 && y <= cb_lenth;
}

bool chessboard::move_chess(char x, char y, char c)
{
	if (m_chesses[y - 1][x - 1] != '+')	return false;

	m_chesses[y - 1][x - 1] = c;
	return true;
}

void chessboard::add_step(char x, char y, unsigned int playerid)
{
	m_stepway.add_step(x, y, playerid);
}

chessstep * chessboard::getlaststep()
{
	return m_stepway.get_laststep();
}

char chessboard::up_chess(char x, char y)
{
	--y;
	if (y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::lower_chess(char x, char y)
{
	++y;
	if (y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::left_chess(char x, char y)
{
	--x;
	if (x)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::right_chess(char x, char y)
{
	++x;
	if (x <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::up_left_chess(char x, char y)
{
	--x;
	--y;
	if (x && y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::up_right_chess(char x, char y)
{
	--y;
	++x;
	if (x <= cb_lenth && y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::lower_left_chess(char x, char y)
{
	++y;
	--x;
	if (x && y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::lower_right_chess(char x, char y)
{
	++y;
	++x;
	if (x <= cb_lenth && y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char chessboard::count_up2down(char x, char y)
{
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (up_chess(x, tmpy) != m_turn)
			break;
		--tmpy;
		++count;
	}

	tmpy = y;
	while (1)
	{
		if (lower_chess(x, tmpy) != m_turn)
			break;
		++tmpy;
		++count;
	}

	return count;
}

char chessboard::count_left2right(char x, char y)
{
	char tmpx = x;
	char count = 1;
	while (1)
	{
		if (left_chess(tmpx, y) != m_turn)
			break;
		--tmpx;
		++count;
	}

	tmpx = x;
	while (1)
	{
		if (right_chess(tmpx, y) != m_turn)
			break;
		++tmpx;
		++count;
	}

	return count;
}

char chessboard::count_upleft2lowright(char x, char y)
{
	char tmpx = x;
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (up_left_chess(tmpx, tmpy) != m_turn)
			break;
		--tmpx;
		--tmpy;
		++count;
	}

	tmpx = x;
	tmpy = y;
	while (1)
	{
		if (lower_right_chess(tmpx, tmpy) != m_turn)
			break;
		++tmpx;
		++tmpy;
		++count;
	}

	return count;
}

char chessboard::count_lowleft2upright(char x, char y)
{
	char tmpx = x;
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (lower_left_chess(tmpx, tmpy) != m_turn)
			break;
		--tmpx;
		++tmpy;
		++count;
	}

	tmpx = x;
	tmpy = y;
	while (1)
	{
		if (up_right_chess(tmpx, tmpy) != m_turn)
			break;
		++tmpx;
		--tmpy;
		++count;
	}

	return count;
}
