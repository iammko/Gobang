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
		SAVE_DELETE(p);
	}
public:
	static T * get_instance()
	{
		if (p == NULL)
		{
			if (p == NULL)
			{
				p = new T();
				atexit(destroy);
			}
		}
		return p;
	}
private:
	static T * p;
};

template<class T>
T *mysingleton<T>::p = NULL;