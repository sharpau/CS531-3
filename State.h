#pragma once
class State
{
public:
	struct tile {
		int value;
		std::vector<int> domain;
	};


	tile board[9][9];
	std::string metadata;

	bool isFullyAssigned(void);
	std::pair<int, int> selectUnassigned(void);
	
	void orderDomain(int x, int y);

	State(void);
	~State(void);
};

