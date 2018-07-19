#include "tcpsock.h"
#include "circular_buffer.h"
#include "chessgame.h"

#include "protos.pb.h"

#include <errno.h>


tcpsock::tcpsock(chessgame * game) : m_game(game)
{
	m_sock_fd = -1;
	bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addrlen = 0;

	m_recv = new circular_buffer(64 * 1024);
}

tcpsock::~tcpsock()
{
	delete m_recv;
}


int tcpsock::send_msg(protocol_number pn, const char * buff, unsigned len)
{
	char buf[32*4096] = { 0 };

	service_msg_header header;
	header.msg_type = pn;
	unsigned headlen = sizeof(header);
	header.msg_size = headlen + len;
	
	if (header.msg_size > 32*4096)	return -1;

	memcpy(buf, &header, headlen);
	memcpy(buf + headlen, buff, len);

	unsigned sent = 0;
	while (sent < header.msg_size)
	{
		int ret = write(m_sock_fd, buf+sent, header.msg_size -sent);
		if (ret > 0)
		{
			sent += ret;
		}
	}
	return 0;
}

int tcpsock::process_msg(protocol_number pn, const char * buff, unsigned len)
{
	if (pn == protocol_number_id_req)
	{
		proto::id_ret decode;
		decode.ParseFromArray(buff, len);
		if (decode.has_id())
			m_game->set_my_id(decode.id());
	}
	else if (pn == protocol_number_game_type)
	{
		proto::game_type_ret decode;
		decode.ParseFromArray(buff, len);
		if (decode.has_result())
		{
			return decode.result();
		}
	}
	else if (pn == protocol_number_join_board)
	{
		proto::join_board_ret decode;
		decode.ParseFromArray(buff, len);
		if (decode.has_result())
		{
			if (decode.has_board_id() && decode.board_id() && decode.result())
			{
				m_game->set_board_id(decode.board_id());
			}
			return decode.result();
		}
	}
	else if (pn == protocol_number_player_info)
	{
		proto::player_info_ret decode;
		decode.ParseFromArray(buff, len);
		for (int i = 0; i < decode.players_size(); ++i)
		{
			const proto::player_info &info = decode.players(i);
			if (info.player_type() == 1)
			{//玩家
				m_game->set_other_id(info.player_id());
			}
		}
	}
	else if (pn == protocol_number_start)
	{
		proto::start_ret decode;
		decode.ParseFromArray(buff, len);
		if (decode.has_white_id())
		{
			m_game->set_player_chess(decode.white_id(), cb_white_chess);
		}
		if (decode.has_black_id())
		{
			m_game->set_player_chess(decode.black_id(), cb_black_chess);
		}
		if (decode.has_start_chess())
		{
			m_game->set_turn_chess(decode.start_chess());
		}
	}
	else if (pn == protocol_number_do_step)
	{
		proto::do_step_ret decode;
		decode.ParseFromArray(buff, len);
		if (decode.has_other_step())
		{
			proto::step_info *info = decode.mutable_other_step();
			if (info)
			{
				m_game->do_step(info->x(), info->y(), m_game->get_other_id());
			}
		}
		if (decode.has_result())
		{
			m_game->set_game_over(decode.result());
			if (decode.result() != 2 && decode.has_win_id())
			{
				m_game->result_menu(decode.result(), decode.win_id());
			}
			else
			{
				m_game->result_menu(decode.result());
			}
		}
		
	}
	else if (pn == protocol_number_surrender)
	{

	}


	return 1;
}

int tcpsock::do_proto(protocol_number pn, const char *msg, unsigned len)
{
	send_msg(pn, msg, len);
	int ret = recv_proto();
	if (ret == -1)
	{
		m_game->m_menu.connection_failed();
	}

	return ret;
}

int tcpsock::recv_proto()
{
	char buf[32 * 4096];
	service_msg_header header;

	int ret = my_read(buf, sizeof(buf));
	if (ret > 0)
	{
		if (ret >= (int)sizeof(header))
		{
			memcpy(&header, buf, sizeof(header));
			m_recv->append(buf + sizeof(header), ret - sizeof(header));
			if (ret < (int)header.msg_size)
			{
				int recv = 0;
				while ((unsigned)recv < header.msg_size - sizeof(header))
				{
					int ret = my_read(buf, sizeof(buf));
					if (ret > 0)
					{
						recv += ret;
						m_recv->append(buf, ret);
					}
					else if (ret == 0)
					{
						return -1;
					}
				}
			}
			else if (ret > (int)header.msg_size)
			{
				return -1;
			}
			return process_msg((protocol_number)header.msg_type, m_recv->data_ptr(), m_recv->data_len());
		}
	}
	else if (ret == 0)
	{
		ret = -1;
	}
	else if (errno == EAGAIN || errno == EWOULDBLOCK)
	{
		ret = 0;
	}
	else
	{
		ret = -1;
	}

	return ret;
}


