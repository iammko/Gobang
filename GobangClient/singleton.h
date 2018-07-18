#pragma once

#include "define.h"

template<class T>
class mysingleton
{
protected:
	mysingleton(){}
	mysingleton(const mysingleton&) {}
	mysingleton& operator=(const mysingleton&) {}

	virtual ~mysingleton(){}
	static void destroy()
	{
		SAVE_DELETE(m_instance);
	}
public:
	static T * get_instance()
	{
		if (m_instance == NULL)
		{
			if (m_instance == NULL)
			{
				m_instance = new T();
				atexit(destroy);
			}
		}
		return m_instance;
	}
private:
	static T * m_instance;
};

template<class T>
T *mysingleton<T>::m_instance = NULL;