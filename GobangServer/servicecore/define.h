#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <map>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#define MAXLINE 4096

#ifndef SAFE_DELETE 
#define SAVE_DELETE(p) { \
		if (p) {delete p; p = NULL;} \
	} 
#endif



const char cb_lenth = 15;
const char cb_black_chess = 'X';
const char cb_white_chess = 'O';
const char cb_result_draw = 1;

enum cg_mode_type
{
	cg_mode_type_none = 0,
	cg_mode_type_offpve = 1,
	cg_mode_type_offpvp = 2,
	cg_mode_type_onlinepvp = 3,
	cg_mode_type_match = 4,
	cg_mode_type_max
};

