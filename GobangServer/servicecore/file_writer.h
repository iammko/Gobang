#pragma once

#include <stdint.h>
#include <string>

class   file_writer {
public:
	file_writer(const std::string &file, size_t slice_size_limit);
	~file_writer();

	void	append(const char *data, unsigned len);
	int get_fd();
private:
	uint64_t	m_write_bytes;
	uint64_t	m_slice_bytes;
	int		m_file_fd;
	time_t		m_last_time;
	std::string		m_write_buffer;

private:
	void		create_file();
	const std::string	m_file_name;
	const size_t	m_slice_limit;
};//end of class file_writer