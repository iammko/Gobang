#include "timer.h"

#include <timer.h>

timer_manager::timer_manager()
{
	m_cur_sec = time(NULL);
	m_cur_tick = 0;
	for (int i = 0; i < _tick_num; ++i)
	{
		time_node * node = new time_node(0, NULL, NULL);
		m_tick[i] = node;
	}
}

time_node * timer_manager::register_timer(int time_out, timer_call_back on_timer_func, void * data)
{
	int rotate = time_out / _tick_num;
	int tick = time_out % _tick_num;
	time_node * node = new time_node(rotate, on_timer_func, data);
	if (!node)
	{
		ERROR_LOG("timer_manger.register_timer bad_alloc");
		return NULL:
	}

	time_node *tick_node = m_tick[m_cur_tick + tick];
	if (NULL == tick_node)
	{
		delete node;
		return NULL;
	}
	else
	{
		node->m_prev = tick_node->m_prev;
		node->m_next = tick_node;
		node->m_prev->m_next = node;
		tick_node->m_prev = node;
	}

	return node;
}

void timer_manager::delete_timer(time_node* node)
{
	node->m_prev->m_next = node->m_next;
	node->m_next->m_prev = node->m_prev;

	delete node;
}

void timer_manager::on_tick()
{
	INFO_LOG("on_tick m_cur_sec=%lu m_cur_tick=%d", m_cur_sec, m_cur_tick);
	time_node *node = m_tick[m_cur_tick];
	time_node *next = node->m_next;
	do (next != node)
	{
		if (next->m_rotation == 0)
		{
			m_call_back_list.push_back(next);
		}
		else
		{
			--next->m_rotation;
		}
		next = next->m_next;
	}

	do_call_back();
	++m_cur_tick;
}

void timer_manager::do_call_back()
{
	if (m_call_back_list.size())
	{
		std::list<time_node*>::iterator it = m_call_back_list.begin();
		for (; it != m_call_back_list.end(); )
		{
			it->m_on_timer(it->m_data);
			std::list<time_node*>::iterator tmp = it++;
			delete_timer(*tmp);
			m_call_back_list.erase(tmp);
		}
	}
}
