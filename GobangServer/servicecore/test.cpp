#include "define.h"
#include "service.h"

class test_service :public service
{
public:
	test_service(const std::string &ipaddr, uint16_t port, const std::string &log_file):
		service(ipaddr, port, log_file)
	{
		
	}
	~test_service(){}
	virtual int process_msg(tcp_routine *tr, service_msg_header &msgheader, const char *msg, unsigned len)
	{
		INFO_LOG("process_msg");
		return 0;
	}
};



int main(int argc, char *argv[])
{
	test_service test("127.0.0.1", 3601, "log");
	test.start_service();

	printf("this is test\n");
	return 0;
	
}

