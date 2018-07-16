#ifndef	_CIRCULAR_BUFFER_H_
#define	_CIRCULAR_BUFFER_H_


#include <string>
#include <iostream>


using	std::string;
using	namespace std;


class	circular_buffer
{
public:
	circular_buffer(unsigned capacity) :m_capacity(capacity), m_pos(0), m_len(0)
	{
		m_data.reserve(m_capacity / 4);
	}

	circular_buffer&	operator=(const circular_buffer &cb)
	{
		this->m_capacity = cb.m_capacity;
		this->m_pos = cb.m_pos;
		this->m_data = cb.m_data;
		this->m_len = cb.m_len;
		return	*this;
	}

	circular_buffer(const circular_buffer &cb)
	{
		this->m_capacity = cb.m_capacity;
		this->m_pos = cb.m_pos;
		this->m_data = cb.m_data;
		this->m_len = cb.m_len;
	}

	~circular_buffer()
	{
		m_capacity = 0;
		m_pos = 0;
		m_data.clear();
		m_len = 0;
	}

	bool		append(const void* buffer, const unsigned len)
	{
		if (m_len - m_pos + len <= m_capacity)
		{
			m_data.append((const char *)buffer, len);
			m_len += len;
			return	true;
		}
		else
		{
			return	false;
		}
	}

	unsigned	unused_len() const
	{
		return	m_capacity - (m_len - m_pos);
	}

	unsigned	data_len() const
	{
		return	m_len - m_pos;
	}

	const	char*	data_ptr() const
	{
		return	m_data.c_str() + m_pos;
	}

	bool		consume(unsigned len)
	{
		if (len + m_pos <= m_len)
		{
			m_pos += len;
			if (m_len > m_pos)
			{
				if (m_pos >= m_capacity / 4)
				{
					m_data.erase(0, m_pos);
					m_len -= m_pos;
					m_pos = 0;
				}
			}
			else	if (m_pos == m_data.size())
			{
				m_pos = 0;
				m_len = 0;
				m_data.clear();
			}
			return	true;
		}
		else
		{
			return	false;
		}
	}

private:
public:
	unsigned	m_capacity;
	unsigned	m_pos;
	unsigned	m_len;
	string		m_data;
};


#endif //_CIRCULAR_BUFFER_H_
