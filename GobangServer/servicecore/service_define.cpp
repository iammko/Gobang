#include "service_define.h"

bool	test_byte_order()
{
	union {
		char	chr[2];
		short	val;
	}u;

	u.val = 0x0100;
	return (bool)u.chr[0];
}