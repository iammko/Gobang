#include "msg_handler_mgr.h"
#include "game_service.h"
#include "data_mgr.h"
#include "protos.pb.h"
#include "game_room.h"
#include "board.h"

msg_handler_mgr::msg_handler_mgr()
{
}

msg_handler_mgr::~msg_handler_mgr()
{
}

msg_hander * msg_handler_mgr::get_handler(protocol_number pn)
{
	handler_map_t::iterator it = m_handler_mgr.find(pn);
	if (it != m_handler_mgr.end())
	{
		return it->second;
	}
	return NULL;
}

void msg_handler_mgr::register_all()
{
	register_handler(protocol_number_id_req,new cs_id_req_handler());
	register_handler(protocol_number_game_type, new game_type_req_handler());
	register_handler(protocol_number_join_board, new join_board_req_handler());
	register_handler(protocol_number_player_info, new player_info_req_handler());
	register_handler(protocol_number_start, new start_req_handler());
	register_handler(protocol_number_do_step, new do_step_req_handler());
	register_handler(protocol_number_surrender, new surrender_req_handler()); 
	register_handler(protocol_number_exit_board, new exit_board_req_handler());
}

void msg_handler_mgr::register_handler(protocol_number pn, msg_hander * handler)
{
	if (get_handler(pn) != NULL)
	{
		DEBUG_LOG("msg_handler_mgr.register_handler repeated handler proto=%u", pn);
		return;
	}

	m_handler_mgr[pn] = handler;
}

void msg_handler_mgr::handle_msg(game_player * gp,const  service_msg_header & msgheader, const char * msg, unsigned len)
{
	msg_hander *mh = get_handler((protocol_number)msgheader.msg_type);
	if (NULL == mh)
	{
		DEBUG_LOG("msg_handler_mgr.handle_msg.get_handler unregister msg_type msg_type=%u, msg_size=%lu, data_len=%u", msgheader.msg_type, msgheader.msg_size, len);
	}

	mh->done(gp, msg, len);
}

int cs_id_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	//proto::id_req decode;
	//decode.ParseFromArray(msg, len);

	proto::id_ret send;
	send.set_id(gp->get_player_id());
	int size = send.ByteSize();
	if (size)
	{
		std::vector<char> bytes;
		bytes.resize(size);
		send.SerializeToArray(&bytes[0], size);

		DEBUG_LOG("cs_id_req_handler done id=%u", gp->get_player_id());
		gp->send_msg(protocol_number_id_req, &bytes[0], size);
	}

	return 0;
}

int game_type_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	proto::game_type_req decode;
	decode.ParseFromArray(msg, len);

	if (!gp->check_state())	return 0;
	gp->set_game_type((cg_mode_type)decode.game_type());
	if (decode.game_type() == cg_mode_type_online_quickstart)
	{
		gp->set_room_type(cg_game_room_normal);
	}

	proto::game_type_ret send;
	send.set_result(1);
	int size = send.ByteSize();
	if (size)
	{
		std::vector<char> bytes;	
		bytes.resize(size);
		send.SerializeToArray(&bytes[0], size);

		DEBUG_LOG("game_type_req_handler done id=%u", gp->get_player_id());
		gp->send_msg(protocol_number_game_type , &bytes[0], size);
	}

	return 0;
}

int join_board_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	proto::join_board_req decode;
	decode.ParseFromArray(msg, len);

	bool ret = 0;
	if (decode.has_board_id())
	{
		ret = game_room_mgr::get_instance()->join_board(gp, decode.board_id());
	}
	else
	{
		ret = game_room_mgr::get_instance()->join_board(gp, 0);
	}
	proto::join_board_ret send;
	send.set_result(ret);
	send.set_board_id(gp->get_board_id());
	int size = send.ByteSize();
	if (size)
	{
		std::vector<char> bytes;
		bytes.resize(size);
		send.SerializeToArray(&bytes[0], size);

		DEBUG_LOG("join_board_req_handler done id=%u", gp->get_player_id());
		gp->send_msg(protocol_number_join_board, &bytes[0], size);
	}

	return 0;
}

int player_info_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	//proto::player_info_req decode;
	//decode.ParseFromArray(msg, len);

	game_room *gm = game_room_mgr::get_instance()->get_game_room(gp->get_room_type());
	if (gm)
	{
		board *b = gm->get_board_by_id(gp->get_board_id());
		if (b)
		{
			if (b->player_count() == 2)
			{
				b->send_info_each(gp);
			}
		}
	}


	return 0;
}

int start_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	//proto::start_req decode;
	//decode.ParseFromArray(msg, len);

	game_room *gm = game_room_mgr::get_instance()->get_game_room(gp->get_room_type());
	if (gm)
	{
		board *b = gm->get_board_by_id(gp->get_board_id());
		if (b)
		{
			gp->set_state(cg_player_state_wait);
			b->set_ready(gp->get_player_id(), true);
		}
	}

	return 0;
}

int do_step_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	proto::do_step_req decode;
	decode.ParseFromArray(msg, len);

	if (!decode.has_step())	return 0;

	game_room *gm = game_room_mgr::get_instance()->get_game_room(gp->get_room_type());
	if (gm)
	{
		board *b = gm->get_board_by_id(gp->get_board_id());
		if (b)
		{
			if (b->player_count() == 2)
			{
				const proto::step_info *step = decode.mutable_step();
				if (step)
				{
					b->send_do_step_ret(gp, step);
				}
			}
		}
	}


	return 0;
}

int surrender_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	//proto::surrender_req decode;
	//decode.ParseFromArray(msg, len);

	game_room *gm = game_room_mgr::get_instance()->get_game_room(gp->get_room_type());
	if (gm)
	{
		board *b = gm->get_board_by_id(gp->get_board_id());
		if (b)
		{
			if (b->player_count() == 2)
			{
				b->send_surrender_ret(gp);
			}
		}
	}

	return 0;
}

int exit_board_req_handler::done(game_player * gp, const char * msg, unsigned len)
{
	proto::exit_board_ret send;
	send.set_player_id(gp->get_player_id());
	send.set_result(0);

	game_room *gm = game_room_mgr::get_instance()->get_game_room(gp->get_room_type());
	if (gm)
	{
		if (gm->exit_board(gp))
		{
			send.set_result(1);
			int size = send.ByteSize();
			std::vector<char> bytes;
			bytes.resize(size);
			send.SerializeToArray(&bytes[0], size);
			gp->send_msg(protocol_number_exit_board, &bytes[0], size);
		}
	}
	return 0;
}
