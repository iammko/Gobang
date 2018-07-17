#include "define.h"

void    report_event(const char *format, ...)
{
	char	msg[4096];
	va_list ap;
	va_start(ap, format);
	vsnprintf(msg, sizeof(msg), format, ap);
	va_end(ap);
	cout << msg << endl;
}