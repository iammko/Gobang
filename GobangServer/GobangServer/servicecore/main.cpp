#include "define.h"
#include "tcpsock.h"

int main(int argc, char *argv[])
{
	stcpsock ssock;
	ssock.createsocket();
	ssock.setip("127.0.0.1");
	ssock.setport(3601);

	ssock.mybind();
	ssock.mylisten();

	tcpsock peersock = ssock.myaccept();

	printf("this is test\n");
	return 0;
	
}

