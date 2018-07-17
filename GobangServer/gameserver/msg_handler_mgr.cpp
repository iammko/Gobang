#include "msg_handler_mgr.h"

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

	return NULL:
}

void msg_handler_mgr::register_all()
{

}

void msg_handler_mgr::register_handler(protocol_number pn, msg_hander * handler)
{
	m_handler_mgr[pn] = handler;
}
