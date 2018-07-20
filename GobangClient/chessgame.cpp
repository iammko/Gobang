#include "chessgame.h"
#include "saveinput.h"
#include "mgrs.h"
#include "protos.pb.h"

chessgame::chessgame() : m_sock(this),m_menu(this),m_board(this)
{
	serveraddr *paddr = servermgr::get_instance()->get_serverinfo_byid(1);
	if (paddr)
	{
		m_sock.set_ip(paddr->m_ip.c_str());
		m_sock.set_port(paddr->m_port);
		m_sock.set_addrlen();
	}
	m_connected_flag = false;
}

chessgame::~chessgame()
{

}

void chessgame::start()
{
	while (1)
	{
		int choose = m_menu.off_online_menu();
		if (choose == BACK_VALUE) break;

		while (1)
		{
			int mode = m_menu.mode_menu(choose);
			if (mode == BACK_VALUE)	break;
			if (mode > cg_mode_type_none && mode < cg_mode_type_max)
			{
				m_game_type = (cg_mode_type)mode;
				while (start_game(mode))
				{

				}
				m_game_type = cg_mode_type_none;
			}
		}
		my_close();
	}
}

bool chessgame::start_game(const char mode)
{
	if (mode == cg_mode_type_offpve)
	{
		game_off_pve();
	}
	else if (mode == cg_mode_type_offpvp)
	{
		game_off_pvp();
	}
	else if (mode == cg_mode_type_online_quickstart)
	{
		game_online_quickstart();
		return false;
	}
	else if (mode == cg_mode_type_online_race)
	{
		game_online_race();
		return false;
	}
	else
	{
		return false;
	}
	int goon = 0;
	get_inputint("请选择继续(1)or返回(0)：", goon, 0, 1);

	return goon;
}

int chessgame::game_off_pve()
{
	return 0;
}

int chessgame::game_off_pvp()
{
	m_board.init();

	int x, y;
	saveinput input;
	while (1)
	{
		m_board.draw();

		int stepret = 0;
		while (1)
		{
			input.input_xy(x, y);
			stepret = m_board.do_step(x, y, 0);
			if (stepret >= 0)
				break;
			else
				m_board.show_do_step_info(stepret);
		}

		if (stepret) break;
	}

	return 0;
}

int chessgame::game_online_quickstart()
{
	int ret = send_game_type_req();
	if (ret != 1)	return 0;

	ret = send_board_join_req();
	if (ret != 1)	return 0;

	while (1)
	{
		m_board.init();
		m_menu.id_chess_menu();
		ret = send_player_info_req();
		if (ret == -1)	return 0;
		if (!m_menu.sure_or_not_menu("请输入(1准备 0退出)："))	return 0;

		ret = send_start_req();
		if (ret == -1)	return 0;

		m_board.draw();
		m_menu.start_oder_menu();
		if (get_turn_chess() == get_player_chess(m_player_id))
		{
			ret = game_do_input_xy_send();
			if (ret == -1)//错误
				return 0;
		}
		else
		{ 
			send_do_step_req(0, 0);
		}
		while (1)
		{
			if (get_game_over())	break;

			ret = game_do_input_xy_send();
			if (ret == -1)//错误
				return 0;
		}

GAMEOVER:
		if (!m_menu.sure_or_not_menu("请输入(1准备 0退出)：")) return 0;
	}
	

}

int chessgame::game_online_race()
{
	return 0;
}

int chessgame::game_do_input_xy_send()//0.成功 1.投降
{
	saveinput input;
	int x, y;
	while (1)
	{
		m_board.draw();
		m_menu.id_chess_menu();
		input.input_xy(x, y);
		if (x == 0 || y == 0)//投降
		{
			send_surrender_req();
			return 1;
		}
		if (m_board.do_step(x, y, m_player_id) >= 0)
		{
			m_board.draw();
			m_menu.id_chess_menu();
			break;
		}
	}
	if (send_do_step_req(x, y) == -1)	return -1;
	return 0;
}

int chessgame::get_inputint(const char * ptip, int & rno, int low, int up, int quit)
{
	saveinput input;
	return input.get_input_int(ptip, rno, low, up, quit);
}

int chessgame::my_connect(int serverid)
{
	if (m_sock.m_sock_fd == -1)
	{
		m_sock.create_socket();
		return	m_sock.my_connect();
	}
	return 1;
}

int chessgame::my_close()
{
	m_sock.my_close();
	m_sock.m_sock_fd = -1;
	return 0;
}

int chessgame::send_id_req()
{
	proto::id_req send;
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	
	m_sock.do_proto(protocol_number_id_req,&bytes[0], size);

	return 0;
}

int chessgame::send_game_type_req()
{
	proto::game_type_req send;
	send.set_game_type(m_game_type);
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);

	return m_sock.do_proto(protocol_number_game_type, &bytes[0], size);
}

int chessgame::send_board_join_req(unsigned room_id)
{
	printf("正在加入房间");
	int n = 3;
	struct timespec req;
	req.tv_sec = 0;
	req.tv_nsec = 300 * 1000 * 1000;
	while (n--)
	{
		printf(".");
		nanosleep(&req, NULL);
	}
	printf("\n");
	proto::join_board_req send;
	if (room_id)
	{
		send.set_board_id(room_id);
	}
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);

	return m_sock.do_proto(protocol_number_join_board, &bytes[0], size);
}

int chessgame::send_player_info_req()
{
	set_state(cg_player_state_wait);
	printf("正在等待玩家\n");
	proto::player_info_req send;
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);

	int ret = m_sock.do_proto(protocol_number_player_info, &bytes[0], size);
	if (ret == 1)
	{
		printf("%d玩家加入\n", get_other_id());
	}
	return ret;
}

int chessgame::send_start_req()
{
	set_state(cg_player_state_wait);
	printf("已准备！\n");
	proto::start_req send;
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	return m_sock.do_proto(protocol_number_start, &bytes[0], size);
}

int chessgame::send_do_step_req(int x, int y)
{
	proto::do_step_req send;
	if (x != 0 && y != 0)
	{
		proto::step_info *info = send.mutable_step();
		if (info)
		{
			info->set_x(x);
			info->set_y(y);
			info->set_stepno(get_step_no());
		}
	}

	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	return m_sock.do_proto(protocol_number_do_step, &bytes[0], size);
}

int chessgame::send_surrender_req()
{
	proto::surrender_req send;
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	return m_sock.do_proto(protocol_number_surrender, &bytes[0], size);
}

int chessgame::send_exit_board_req()
{
	proto::exit_board_req send;
	int size = send.ByteSize();
	std::vector<char> bytes;
	bytes.resize(size);
	send.SerializeToArray(&bytes[0], size);
	return m_sock.do_proto(protocol_number_exit_board, &bytes[0], size);
}

void chessgame::set_my_id(unsigned id)
{
	m_player_id = id;
	m_board.set_id(PLAYER_ME, id);
}

unsigned chessgame::get_my_id()
{
	return m_player_id;
}

void chessgame::set_other_id(unsigned id)
{
	m_board.set_id(PLAYER_OTHER, id);
}

unsigned chessgame::get_other_id()
{
	return m_board.get_id(PLAYER_OTHER);
}

void chessgame::set_board_id(unsigned id)
{
	m_board.m_board_id = id;
}

unsigned chessgame::get_board_id()
{
	return m_board.m_board_id;
}

void chessgame::exit_board()
{
	set_board_id(0);
}

void chessgame::set_turn_chess(char chess)
{
	m_board.m_turn = chess;
}

char chessgame::get_turn_chess()
{
	return m_board.m_turn;
}

void chessgame::set_player_chess(unsigned player_id, char chess)
{
	if (player_id == m_player_id)
	{
		m_board.set_player_chess(PLAYER_ME, chess);
	}
	else if (player_id == get_other_id())
	{
		m_board.set_player_chess(PLAYER_OTHER, chess);
	}

}

char chessgame::get_player_chess(unsigned player_id)
{
	if (player_id == m_player_id)
	{
		return m_board.get_player_chess(PLAYER_ME);
	}
	else if (player_id == get_other_id())
	{
		return m_board.get_player_chess(PLAYER_OTHER);
	}

	return 0;
}

int chessgame::do_step(int x, int y, unsigned player_id)
{
	return m_board.do_step(x, y, player_id);
}

void chessgame::result_menu(int result, int win_id)
{
	m_menu.result_menu(result, win_id);
}

void chessgame::set_game_over(int over)
{
	m_board.m_gameover = over;
}

int chessgame::get_game_over()
{
	return m_board.m_gameover;
}

unsigned chessgame::get_step_no()
{
	return m_board.m_stepway.get_stepno();
}

void chessgame::set_state(cg_player_state_type type)
{
	m_state = type;
}

cg_player_state_type chessgame::get_state()
{
	return m_state;
}


