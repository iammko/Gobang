#include "service.h"
#include "tcp_routine_proxy.h"

void service::start_service()
{
	m_routine_proxy->do_loop(NULL);


}

void service::process_msg(tcp_routine_proxy * trp, tcp_routine * tr, circular_buffer * buff)
{

}

void service::process_msg(tcp_routine * tr, const char * msg, unsigned len)
{

}
