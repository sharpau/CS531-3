#pragma once
class State
{
	// private methods
	std::vector<std::pair<int, int>> boxOtherCoords(int x, int y);

public:
	struct cell {
		int value;
		std::vector<int> domain;
	};

	// member vars
	cell board[9][9]; //[row][col]
	std::string metadata;

	// member functions
	bool isFullyAssigned(void);
	std::pair<int, int> selectUnassigned(void);
	
	void orderDomain(int x, int y);
	bool constraintPropagation(void);

	std::string print(void);

	State(void);
	~State(void);
};

