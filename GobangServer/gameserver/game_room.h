#pragma once
#include "define.h"
#include "singleton.h"

class board;
class game_player;

class game_room
{
public:
	game_room(cg_game_room_type type, unsigned room_id);
	~game_room();

	void init();

	bool join_board(game_player *gp, unsigned board_id);
	bool exit_board(game_player *gp);

	board* alloc_room_by_type_id(cg_mode_type type, unsigned board_id);
	board* get_board_by_id(unsigned board_id);
private:
	cg_game_room_type m_room_type;
	unsigned m_room_id;
	std::vector<board> m_boards;
	typedef std::set<unsigned> board_set_t;
	board_set_t m_emptys;
	board_set_t m_readys;
	board_set_t m_fulls;

	unsigned m_max_count;
};

class game_room_mgr : public mysingleton<game_room_mgr>
{
private:
	game_room_mgr();

	game_room *get_game_room(cg_game_room_type type);

	bool join_board(game_player *gp, unsigned board_id);

	friend class mysingleton<game_room_mgr>;
public:
	typedef std::map<cg_game_room_type, game_room> game_room_map_t;
	game_room_map_t m_room_mgr;

};