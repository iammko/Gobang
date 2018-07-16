#include "tcp_routine_proxy.h"
#include "tcp_routine.h"

tcp_routine_proxy::tcp_routine_proxy():
	m_auto_work(false)
{
	if ((m_epoll_fd = epoll_create(1024)) < 0)
	{
		ERROR_LOG("epoll_create error");
		exit(-1);
	}

}



void tcp_routine_proxy::add_routine(tcp_routine * tr)
{
	if (m_routine_mgr.find(tr->m_routine_id) == m_routine_mgr.end())
	{
		m_routine_mgr[tr->m_routine_id] = tr;
	}
	tr->m_routine_proxy = this;

	struct epoll_event ev;
	ev.u64 = tr->m_routine_id;
	ev.events = tr->get_events();

	if (ev.events != 0 && tr->m_fd >= 0)
	{
		if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, tr->m_fd, &ev) == 0)
		{
			DEBUG_LOG("add_routine.epoll_ctl done");
		}
		else
		{
			ERROR_LOG("add_routine.epoll_ctl error");
		}
	}
}

void tcp_routine_proxy::close_routine(uint64_t routine_id)
{
	tcp_routine *tr = get_routine(routine_id);
	if (tr)
	{
		unregister_events(tr);
		delete tr;
	}
	else
	{
		ERROR_LOG("close_routine error");
	}
}

tcp_routine * tcp_routine_proxy::get_routine(uint64_t routine_id)
{
	tcp_sock_mgr::iterator it = m_routine_mgr.find(routine_id);
	if (it == m_routine_mgr.end())
	{
		return NULL;
	}

	return it->second;
}

void tcp_routine_proxy::unregister_events(tcp_routine * tr)
{
	struct epoll_event ev;
	ev.events = tr->get_events();

	if (ev.events != 0 && tr->m_fd >= 0)
	{
		if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, tr->m_fd, &ev) == 0)
		{
			DEBUG_LOG("unregister_events done");
		}
		else
		{
			ERROR_LOG("unregister_events error");
		}
	}
}

void tcp_routine_proxy::modify_events(tcp_routine * tr)
{
	struct epoll_event ev;
	ev.u64 = tr->m_routine_id;
	ev.events = tr->get_events();

	if (ev.events != 0 && tr->m_fd >= 0)
	{
		if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, tr->m_fd, &ev))
		{
			ERROR_LOG("modify_events error");
		}
	}
}

int tcp_routine_proxy::do_loop(void * arg)
{
	m_auto_work = true;
	do {
		react();
	} while (m_auto_work);

	return 0;
}

void tcp_routine_proxy::react()
{
	struct epoll_event arr_events[256];
	bzero(arr_events, sizeof(arr_events));

	int fetch_num = sizeof(arr_events) / sizeof(arr_events[0]);
	if (m_routine_mgr.size() < fetch_num) fetch_num = m_routine_mgr.size();

	if (fetch_num)
	{
		int ret = epoll_wait(m_epoll_fd, arr_events, fetch_num, 0);
		if (ret < 0 && errno != ENINR)
		{
			ERROR_LOG("epoll_wait error");
		}

		for (int n = 0; n < ret; ++n)
		{
			process_events(arr_events[n].u64, arr_events[n].events);
		}
	}

}

void tcp_routine_proxy::process_events(uint64_t routine_id, uint32_t events)
{
	tcp_routine *tr = get_routine(routine_id);
	if (NULL == tr)
	{
		ERROR_LOG("process_events error");
		return;
	}

	if (events & EPOLLOUT)
	{
		tr->on_write();
	}
	
	if (events & EPOLLIN)
	{
		tr->on_read();
	}

	if (events & EPOLLRDHUP)
	{
		tr->on_peer_close();
	}

	if (events & EPOLLHUP)
	{
		tr->on_hangup();
	}

	if (events & EPOLLERR)
	{
		tr->on_error();
	}
}
