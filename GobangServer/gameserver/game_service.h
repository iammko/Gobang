#pragma once
#include "service.h"

class game_service : public service
{
public:
	virtual int process_msg(tcp_routine *tr, service_msg_header &msgheader, const char *msg, unsigned len);
};