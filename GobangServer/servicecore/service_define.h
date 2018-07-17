#pragma once



bool	test_byte_order();

struct  user_data {
	user_data() :user(0), id(0), ptr(0)
	{
	}
	user_data(const user_data &ud)
	{
		this->user = ud.user;
		this->id = ud.id;
		this->ptr = ud.ptr;
	}
	user_data&	operator=(const user_data &ud)
	{
		this->user = ud.user;
		this->id = ud.id;
		this->ptr = ud.ptr;
		return	*this;
	}

	uint64_t        user;
	uint64_t        id;
	void*           ptr;
};