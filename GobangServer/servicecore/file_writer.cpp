#include "file_writer.h"


#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <iostream>


using	namespace std;

file_writer::file_writer(const string &file_name, size_t slice_kb) :
	m_file_fd(-1), m_file_name(file_name),
	m_slice_limit(slice_kb * 1024)
{
	create_file();
	m_write_bytes = 0;
	m_slice_bytes = 0;
	m_last_time = 0;
}

file_writer::~file_writer()
{
	close(m_file_fd);
}

void    file_writer::create_file()
{
	static  const   mode_t  file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	static  const   mode_t  file_flag = O_WRONLY | O_APPEND | O_CREAT;

	if (m_file_fd >= 0)
	{
		close(m_file_fd);
	}

	time_t          now = time(NULL);
	struct  tm      generated;

	localtime_r(&now, &generated);

	char    sz_file[256] = { 0 };
	snprintf(sz_file, sizeof(sz_file), "%s.%02d%02d%02d.%02d%02d",
		m_file_name.c_str(),
		generated.tm_year + 1900,
		generated.tm_mon + 1,
		generated.tm_mday,
		generated.tm_hour,
		generated.tm_min);

	m_file_fd = open(sz_file, file_flag, file_mode);
	if (m_file_fd >= 0)
	{
		fcntl(m_file_fd, F_SETFL, O_NONBLOCK);
		lseek(m_file_fd, 0, SEEK_END);
		m_last_time -= m_last_time % 86400;
	}
}

void    file_writer::append(const char *msg, unsigned msg_len)
{
	m_write_buffer.append(msg, msg_len);
	ssize_t ret = write(m_file_fd, m_write_buffer.c_str(), m_write_buffer.size());
	if (ret > 0)
	{
		m_write_buffer = m_write_buffer.substr(ret);
		m_write_bytes += ret;
		m_slice_bytes += ret;

		time_t now = time(NULL);
		if (m_slice_bytes >= m_slice_limit || now - now % 86400 != m_last_time)
		{
			create_file();
			m_slice_bytes = 0;
		}
	}
	else
	{
		cout << "write failed:" << strerror(errno) << endl;
	}
}

int file_writer::get_fd()
{
	return m_file_fd;
}
