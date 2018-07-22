#pragma  once

typedef void(*timer_call_back)(void *data);

struct time_node
{
	time_node(int rotation, timer_call_back on_timer_func, void * data) :
		m_rotation(rotation),
		m_on_timer(on_timer_func),
		m_data(data),
		m_prev(this),
		m_next(this),
		m_data(NULL),
		m_on_timer(NULL),
		m_register_time(0)
	{

	}
	time_node * m_prev;
	time_node * m_next;

	int m_rotation;
	
	void *m_data;
	timer_call_back m_on_timer;
	uint64_t m_register_time;
};

class timer_manager
{
public:	
	timer_manager();
	~timer_manager();

	time_node *register_timer(int time_out, timer_call_back on_timer_func, void *data);
	void delete_timer(time_node* node);
	void on_tick();
	void do_call_back();

	static const int _tick_num = 30;

	uint64_t m_cur_sec;
	int m_cur_tick;
	time_node* m_tick[_tick_num];
	std::list<time_node *> m_call_back_list;
};