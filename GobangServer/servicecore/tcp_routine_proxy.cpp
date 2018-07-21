#include "tcp_routine_proxy.h"
#include "tcp_routine.h"
#include "service.h"

#include <errno.h>

#include <sys/epoll.h>


tcp_routine_proxy::tcp_routine_proxy():
	m_auto_work(false)
{
	if ((m_epoll_fd = epoll_create(1024)) < 0)
	{
		ERROR_LOG("epoll_create error");
		exit(-1);
	}


}

tcp_routine_proxy::~tcp_routine_proxy()
{
	INFO_LOG("~tcp_routine_proxy");

	tcp_routine_mgr::iterator it = m_routine_mgr.begin();
	for (; it != m_routine_mgr.end(); ++it)
	{
		delete it->second;
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
	ev.data.u64 = tr->m_routine_id;
	ev.events = tr->get_events();

	if (ev.events != 0 && tr->m_fd >= 0)
	{
		if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, tr->m_fd, &ev) == 0)
		{
			DEBUG_LOG("add_routine.epoll_ctl done epfd=%d routine_id=%lu fd=%d", m_epoll_fd, tr->m_routine_id, tr->m_fd);
		}
		else
		{
			ERROR_LOG("add_routine.epoll_ctl error epfd=%d routine_id=%lu fd=%d", m_epoll_fd, tr->m_routine_id, tr->m_fd);
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
	tcp_routine_mgr::iterator it = m_routine_mgr.find(routine_id);
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
			DEBUG_LOG("unregister_events done epfd=%d routine_id=%lu fd=%d", m_epoll_fd, tr->m_routine_id, tr->m_fd);
		}
		else
		{
			ERROR_LOG("unregister_events error epfd=%d routine_id=%lu fd=%d", m_epoll_fd, tr->m_routine_id, tr->m_fd);
		}
	}
}

void tcp_routine_proxy::modify_events(tcp_routine * tr)
{
	struct epoll_event ev;
	ev.data.u64 = tr->m_routine_id;
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
	tcp_routine_proxy *trp = (tcp_routine_proxy*)arg;

	do {
		trp->react();
	} while (trp->m_auto_work);

	return 0;
}

void tcp_routine_proxy::react()
{
	struct epoll_event arr_events[256];
	bzero(arr_events, sizeof(arr_events));

	unsigned fetch_num = sizeof(arr_events) / sizeof(arr_events[0]);
	if (m_routine_mgr.size() < fetch_num) fetch_num = m_routine_mgr.size();

	if (fetch_num)
	{
		int ret = epoll_wait(m_epoll_fd, arr_events, fetch_num, 0);
		if (ret < 0 && errno != EINTR)
		{
			ERROR_LOG("epoll_wait error");
		}

		for (int n = 0; n < ret; ++n)
		{
			process_events(arr_events[n].data.u64, arr_events[n].events);
		}
	}

	inspect();

	struct timespec rqtp;
	rqtp.tv_nsec = 20 * 1000;
	nanosleep(&rqtp, NULL);
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
		tr->m_writable = true;
		tr->on_write();
		if (tr->m_writable)
		{
			modify_events(tr);
		}
	}
	
	if (events & EPOLLIN)
	{
		if (tr->on_read() < 0)
		{
			service::get_instance()->on_peer_close(this, tr);
			tr->on_peer_close();
		}
	}

	if (events & EPOLLRDHUP)
	{
		service::get_instance()->on_peer_close(this, tr);
		tr->on_peer_close();
	}

	if (events & EPOLLHUP)
	{
		service::get_instance()->on_hangup(this, tr);
		tr->on_hangup();
	}

	if (events & EPOLLERR)
	{
		service::get_instance()->on_routine_error(this, tr);
		tr->on_error();
	}
}

void tcp_routine_proxy::inspect()
{
	tcp_routine_mgr::iterator it = m_routine_mgr.begin();
	for (; it != m_routine_mgr.end();)
	{
		tcp_routine_mgr::iterator tmpit = it++;
		tcp_routine *tr = tmpit->second;
		if (tr->m_del_flag)
		{
			close_routine(tmpit->first);
			m_routine_mgr.erase(tmpit);
		}
	}
}

void tcp_routine_proxy::set_user_data(uint64_t routine_id, const user_data & data)
{
	tcp_routine *tr = get_routine(routine_id);
	if (tr)
	{
		tr->set_user_data(data);
	}
}

void tcp_routine_proxy::get_user_data(uint64_t routine_id, user_data & data)
{
	tcp_routine *tr = get_routine(routine_id);
	if (tr)
	{
		tr->get_user_data(data);
	}
}

