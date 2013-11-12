#pragma once
class State
{
	// private methods
	std::vector<std::pair<int, int>> boxOtherCoords(int x, int y);

	bool updateDomains(void);

public:
	struct cell {
		int value;
		std::vector<int> domain;
	};

	enum rule {
		SINGLE_DOMAIN,
		UNIQUE,
		NAKED_DOUBLES,
		NAKED_TRIPLES,

		RULE_COUNT
	};

	// member vars
	cell board[9][9]; //[row][col]
	std::string metadata;
	int num_backtracks;

	// member functions
	bool isFullyAssigned(void);
	std::pair<int, int> selectUnassigned(const bool random = false);
	
	void orderDomain(int x, int y);
	bool constraintPropagation(const rule strongest = NAKED_TRIPLES);

	std::string print(void);
	int unassignedCount(void);

	State(void);
	~State(void);
};

