#pragma once
class State
{
	struct tile {
		int value;
		std::vector<int> domain;
	};



public:
	tile board[9][9];
	std::string metadata;

	State(void);
	~State(void);
};

