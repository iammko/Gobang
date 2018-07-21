#include "board.h"
#include "data_mgr.h"
#include "protos.pb.h"

board::board(unsigned room_id, unsigned board_id):m_room_id(room_id), m_board_id(board_id)
{
	init();

	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		m_players[i].m_player_id = 0;
		m_players[i].m_chess = cb_chesses[i];
	}
}

board::~board()
{
}

void board::init()
{
	m_gameover = false;
	m_turn_index = 0;
	m_turn_chess = cb_chesses[m_turn_index];
	m_game_state = 0;
	m_chess_way.clear();
	for (int i = 0; i < cb_lenth; ++i)
	{
		for (int j = 0; j < cb_lenth; ++j)
		{
			m_chesses[i][j] = '+';
		}
	}

	reset_ready();
}

void board::turn_chess_index()
{
	++m_turn_index;
	if (m_turn_index > 1)
		m_turn_index = 0;
}

bool board::join(game_player * gp)
{
	gp->set_room_id(m_room_id);
	gp->set_board_id(m_board_id);

	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == 0)
		{
			m_players[i].m_player_id = gp->get_player_id();
			gp->set_chess(m_players[i].m_chess);
			return true;
		}
	}

	return false;
}

bool board::exit(game_player * gp)
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == gp->get_player_id())
		{
			m_players[i].m_player_id = 0;
			reset_ready();
			game_player *another = get_another_player(gp);
			if (another)
			{
				if (another->get_state() >= cg_player_join_board
					&& another->get_state() <= cg_player_state_playing)
				{
					proto::exit_board_ret send;
					send.set_player_id(gp->get_player_id());
					send.set_result(1);
					int size = send.ByteSize();
					std::vector<char> bytes;
					bytes.resize(size);
					send.SerializeToArray(&bytes[0], size);

					another->send_msg(protocol_number_exit_board, &bytes[0], size);
					another->set_state(cg_player_state_free);
				}
			}

			return true;
		}
	}


	return false;
}

void construct_player_info(proto::player_info_ret &send,const chess_player &player)
{
	proto::player_info *info = send.add_players();
	if (info)
	{
		info->set_player_id(player.m_player_id);
		info->set_player_type(1);
	}
}

void send_player_info(game_player *gp, proto::player_info_ret &send)
{
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);

	gp->send_msg(protocol_number_player_info, &bytes[0], size);
}


bool board::send_info_each(game_player * src)
{
	proto::player_info_ret send_src;
	proto::player_info_ret send_dest;

	game_player *dest = NULL;

	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == src->get_player_id())
		{
			construct_player_info(send_dest, m_players[i]);
		}
		else
		{
			dest = data_mgr::get_instance()->get_player(m_players[i].m_player_id);
			if (!dest)	return  false;

			construct_player_info(send_src, m_players[i]);
		}
	}

	send_player_info(src, send_src);
	send_player_info(dest, send_dest);

	return true;
}

bool board::send_start_ret()
{
	proto::start_ret send;
	
	send.set_white_id(m_players[0].m_player_id);
	send.set_black_id(m_players[1].m_player_id);
	send.set_start_chess(m_turn_chess);

	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);

	send_msg_all(protocol_number_start, &bytes[0], size);

	return true;
}

bool board::send_do_step_ret(game_player * src,const proto::step_info *step)
{
	proto::do_step_ret send;

	int ret = do_step(step->x(), step->y(), src->get_player_id());

	proto::step_info *t_step = send.mutable_other_step();
	if (t_step)
	{
		t_step->set_x(step->x());
		t_step->set_y(step->y());
		t_step->set_stepno(step->stepno());
		DEBUG_LOG("send_do_step_ret x=%u, y=%u, stepno=%u, do_player=%u", step->x(), step->y(), step->stepno(), src->get_player_id());
	}

	if (ret == (int)cg_result_none)
	{
		int size = send.ByteSize();
		std::vector<char> bytes;
		bytes.resize(size);
		send.SerializeToArray(&bytes[0], size);
		send_msg_another(src, protocol_number_do_step, &bytes[0], size);

		return true;
	}
	else if (ret == (int)cg_result_win_lost)
	{
		send.set_result(cg_result_win_lost);
		send.set_win_id(m_players[m_turn_index].m_player_id);
	}
	else if (ret == (int)cg_result_draw)
	{
		send.set_result(cg_result_draw);
	}
	else
	{
		src->send_msg(protocol_number_do_step, NULL, 0);
		return false;
	}

	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	send_msg_all(protocol_number_do_step, &bytes[0], size);

	return true;
}

bool board::send_surrender_ret(game_player * gp)
{
	proto::do_step_ret send;
	send.set_result(cg_result_surrender);
	game_player *another = get_another_player(gp);
	if (another)
	{
		send.set_win_id(another->get_player_id());
	}

	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	send_msg_all(protocol_number_do_step, &bytes[0], size);

	return true;
}

unsigned board::player_count()
{
	unsigned count = 0;
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id)	++count;
	}
	return count;
}

bool board::all_ready()
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (!m_players[i].m_ready)
		{
			return false;
		}
	}
	return true;
}

void board::set_ready(unsigned player_id, bool is_ready)
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_players[i].m_player_id == player_id)
		{
			m_players[i].m_ready = is_ready;
			break;
		}
	}

	if (all_ready())
	{	
		init();
		send_start_ret();
	}
}

void board::reset_ready()
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		m_players[i].m_ready = false;
	}
}

bool board::all_ready_state(game_player *gp)
{
	
	return false;
}

int board::do_step(char x, char y, unsigned playerid)
{
	if (!check_xy(x, y))
	{
		return -1;
	}

	bool ret = true;
	bool move_flag = false;
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (m_turn_chess == m_players[i].m_chess && playerid == m_players[i].m_player_id)
		{
			ret = move_chess(x, y, m_turn_chess);
			move_flag = true;
		}
	}

	if (!move_flag)
	{
		return -2;
	}

	if (!ret)
	{
		return -3;
	}

	add_step(x, y, playerid);

	if (check_win(x, y))
	{
		return cg_result_win_lost;
	}

	if (m_chess_way.get_stepno() >= cb_lenth * cb_lenth)
	{
		return cg_result_draw;
	}

	turn_chess_index();
	m_turn_chess = cb_chesses[m_turn_index];

	return cg_result_none;
}

int board::do_bot_step()
{
	return 0;
}

int board::check_win(char x, char y)
{
	const char wincount = 5 - 1;

	if (m_chess_way.get_stepno() < wincount*2)	return false;

	if (count_up2down(x, y) > wincount || count_left2right(x, y) > wincount || count_lowleft2upright(x, y) > wincount || count_upleft2lowright(x, y) > wincount)
		return true;


	return false;
}

game_player * board::get_another_player(game_player * src)
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		if (src->get_player_id() != m_players[i].m_player_id)
		{
			game_player *another = data_mgr::get_instance()->get_player(m_players[i].m_player_id);
			if (another)
			{
				return another;
			}
		}
	}
	return NULL;
}

void board::send_msg_all(protocol_number pn, const char * msg, const unsigned len)
{
	for (unsigned i = 0; i < sizeof(m_players) / sizeof(m_players[0]); ++i)
	{
		game_player *player = data_mgr::get_instance()->get_player(m_players[i].m_player_id);
		if (player)
		{
			player->send_msg(pn, msg, len);
			if (pn == protocol_number_start)	player->set_state(cg_player_state_playing);
		}
	}
}

void board::send_msg_another(game_player * src, protocol_number pn, const char * msg, const unsigned len)
{
	game_player *another = get_another_player(src);
	if (another)
	{
		DEBUG_LOG("send_msg_another from player=%u to player=%u proto=%u", src->get_player_id(), another->get_player_id(), pn);
		another->send_msg(pn, msg, len);
	}
}

bool board::check_xy(int x, int y)
{
	return x > 0 && x <= cb_lenth && y > 0 && y <= cb_lenth;
}

bool board::move_chess(char x, char y, char c)
{
	if (m_chesses[y - 1][x - 1] != '+')	return false;

	m_chesses[y - 1][x - 1] = c;
	return true;
}

void board::add_step(char x, char y, unsigned int playerid)
{
	m_chess_way.add_step(x, y, playerid);
}

chessstep * board::getlaststep()
{
	return m_chess_way.get_laststep();
}

char board::up_chess(char x, char y)
{
	--y;
	if (y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::lower_chess(char x, char y)
{
	++y;
	if (y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::left_chess(char x, char y)
{
	--x;
	if (x)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::right_chess(char x, char y)
{
	++x;
	if (x <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::up_left_chess(char x, char y)
{
	--x;
	--y;
	if (x && y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::up_right_chess(char x, char y)
{
	--y;
	++x;
	if (x <= cb_lenth && y)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::lower_left_chess(char x, char y)
{
	++y;
	--x;
	if (x && y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::lower_right_chess(char x, char y)
{
	++y;
	++x;
	if (x <= cb_lenth && y <= cb_lenth)
		return m_chesses[y - 1][x - 1];
	return 0;
}

char board::count_up2down(char x, char y)
{
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (up_chess(x, tmpy) != m_turn_chess)
			break;
		--tmpy;
		++count;
	}

	tmpy = y;
	while (1)
	{
		if (lower_chess(x, tmpy) != m_turn_chess)
			break;
		++tmpy;
		++count;
	}

	return count;
}

char board::count_left2right(char x, char y)
{
	char tmpx = x;
	char count = 1;
	while (1)
	{
		if (left_chess(tmpx, y) != m_turn_chess)
			break;
		--tmpx;
		++count;
	}

	tmpx = x;
	while (1)
	{
		if (right_chess(tmpx, y) != m_turn_chess)
			break;
		++tmpx;
		++count;
	}

	return count;
}

char board::count_upleft2lowright(char x, char y)
{
	char tmpx = x;
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (up_left_chess(tmpx, tmpy) != m_turn_chess)
			break;
		--tmpx;
		--tmpy;
		++count;
	}

	tmpx = x;
	tmpy = y;
	while (1)
	{
		if (lower_right_chess(tmpx, tmpy) != m_turn_chess)
			break;
		++tmpx;
		++tmpy;
		++count;
	}

	return count;
}

char board::count_lowleft2upright(char x, char y)
{
	char tmpx = x;
	char tmpy = y;
	char count = 1;
	while (1)
	{
		if (lower_left_chess(tmpx, tmpy) != m_turn_chess)
			break;
		--tmpx;
		++tmpy;
		++count;
	}

	tmpx = x;
	tmpy = y;
	while (1)
	{
		if (up_right_chess(tmpx, tmpy) != m_turn_chess)
			break;
		++tmpx;
		--tmpy;
		++count;
	}

	return count;
}
