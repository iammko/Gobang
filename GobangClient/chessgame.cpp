#include "chessgame.h"
#include "chessmenu.h"
#include "chessboard.h"
#include "saveinput.h"
#include "tcpsock.h"
#include "mgrs.h"
#include "protos.pb.h"

chessgame::chessgame() : m_sock(this),m_menu(this),m_board(this)
{
	m_sock.create_socket();
	serveraddr *paddr = servermgr::get_instance()->get_serverinfo_byid(1);
	if (paddr)
	{
		m_sock.set_ip(paddr->m_ip.c_str());
		m_sock.set_port(paddr->m_port);
		m_sock.set_addrlen();
	}
	m_msg_data = new process_msg_data();
}

chessgame::~chessgame()
{
	delete m_msg_data;
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
				while (start_game(mode))
				{

				}
			}
		}
	}
}

bool chessgame::start_game(const char mode)
{
	m_game_type = (cg_mode_type)mode;
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
		return 0;
	}
	else if (mode == cg_mode_type_online_race)
	{
		game_online_race();
		return;
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
			if (stepret >= 0)	break;
		}

		if (stepret) break;
	}

	return 0;
}

int chessgame::game_online_quickstart()
{
	send_game_type_req();



	return 0;
}

int chessgame::game_online_race()
{
	return 0;
}

int chessgame::get_inputint(const char * ptip, int & rno, int low, int up, int quit)
{
	saveinput input;
	return input.get_input_int(ptip, rno, low, up, quit);
}

int chessgame::my_connect(int serverid)
{
	return m_sock.my_connect();
}

int chessgame::my_close()
{
	return m_sock.my_close();
}

void chessgame::set_process_msg_data(const void * data, len)
{
	SAFE_DELETE
	memcpy(m_msg_data.data)
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

	m_sock.do_proto(protocol_number_game_type, &bytes[0], size);

	return 0;
}

void chessgame::set_id(unsigned id)
{
	m_player_id = id;
}

unsigned chessgame::get_id()
{
	return m_player_id;
}


