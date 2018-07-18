#include "msg_handler_mgr.h"
#include "game_service.h"
#include "data_mgr.h"
#include "protos.pb.h"

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
	proto::id_req decode;
	decode.ParseFromArray(msg, len);

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
	gp->choose_type(decode.game_type());

	proto::game_type_ret send;
	send.set_result(0);
	int size = send;
	if (size)
	{
		std::vector<char> bytes;	
		bytes.resize(size);
		send.SerializeToArray(&bytes[0], size);

		DEBUG_LOG("game_type_req_handler done id=%u", gp->get_player_id());
		gp->send_msg(protocol_number_game_type, , &bytes[0], size);
	}

	return 0;
}
