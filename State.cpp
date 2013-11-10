#include "stdafx.h"
#include "State.h"

unsigned long backtracks;

// returns all other coord pairs in this location's local box
std::vector<std::pair<int, int>>
State::boxOtherCoords(int x, int y) {
	std::vector<std::pair<int, int>> coords;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			if((x-(x%3)) + i != x || (y-(y%3)) + j != y) {
				coords.push_back(std::make_pair((x-(x%3)) + i, (y-(y%3) + j)));
			}
		}
	}
	assert(coords.size() == 8);
	return coords;
}

// returns true if every spot has an assignment, false otherwise
bool State::isFullyAssigned(void) {
	std::vector<std::vector<int>> alldiffs;
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(board[i][j].value == 0) {
				return false;
			}
		}
	}

	return true;
}

// returns the best variable to find a value for
std::pair<int, int> 
State::selectUnassigned(void) {
	int x_min, y_min;
	int min_domain = INT_MAX;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(board[i][j].value == 0 && board[i][j].domain.size() < min_domain) {
				min_domain = board[i][j].domain.size();
				x_min = i;
				y_min = j;
			}
		}
	}

	if(min_domain == INT_MAX) {
		std::cout << "we have a problem";
	}

	return std::make_pair(x_min, y_min);
}

bool
State::updateDomains(void)
{
	// update domains of unassigned
	for(int x = 0; x < 9; x++) {
		for(int y = 0; y < 9; y++) {
			if(board[x][y].value == 0) {
				// remove from domain any values assigned to other cells in row/col/box
				// check rows and columns
				for(int i = 0; i < 9; i++) {
					if(i != x && board[i][y].value != 0) {
						// remove board[i][y].value from board[x][y].domain
						board[x][y].domain.erase(std::remove(board[x][y].domain.begin(), board[x][y].domain.end(), board[i][y].value), board[x][y].domain.end());
					}

					if(i != y && board[x][i].value != 0) {
						// remove board[x][i].value from board[x][y].domain
						board[x][y].domain.erase(std::remove(board[x][y].domain.begin(), board[x][y].domain.end(), board[x][i].value), board[x][y].domain.end());
					}
				}

				// check box
				for(auto xy : boxOtherCoords(x, y)) {
					if(board[xy.first][xy.second].value != 0) {
						// remove board[xy.first][xy.second].value from board[x][y].domain
						board[x][y].domain.erase(std::remove(board[x][y].domain.begin(), board[x][y].domain.end(), board[xy.first][xy.second].value), board[x][y].domain.end());
					}
				}
				
				if(board[x][y].domain.size() == 0) {
					return false;
				}
			}
		}
	}
}

// forward-checking constraint propagation: returns false if a var ends up with empty domain
bool 
State::constraintPropagation(const rule strongest)
{
	/*
		applied = false
		while(applied)
			applied = false
			// update domains
			for all cells
				remove from domain any values assigned to other cells in row/col/box
				if domain.size == 0
					return false
			// rule 1
			for all unassigned cells
				if cell.domain.size == 1
					assign that value
					applied = true
			// rule 2
			for all unassigned cells
				for all domain values
					if value unique among row
						assign
						applied = true
					if value unique among col
						assign
						applied = true
					if value unique among box
						assign
						applied = true
			// rule 3
			for all unassigned cells
				// naked doubles k = 2
				if domain size == 2
					for each other cell in col with domain size == 2
						if both domains contain the same numbers
							remove those numbers from all others in the column
							if any domain sizes == 0 return false
							applied = true
					for each other cell in row with domain size == 2
						if both domains contain the same numbers
							remove those numbers from all others in the row
							if any domain sizes == 0 return false
							applied = true
					for each other cell in box with domain size == 2
						if both domains contain the same numbers
							remove those numbers from all others in the box
							if any domain sizes == 0 return false
							applied = true
	*/
	
	if(!updateDomains()) {
		return false;
	}

	bool applied = true;
	while(applied) {
		applied = false; // only continue if some rule is applied

		if(strongest >= SINGLE_DOMAIN) {
			// rule #1 - domain of size 1
			for(int x = 0; x < 9; x++) {
				for(int y = 0; y < 9; y++) {
					if(board[x][y].value == 0 && board[x][y].domain.size() == 1) {
						board[x][y].value = board[x][y].domain[0];
						applied = true;

						if(!updateDomains()) {
							return false;
						}

					}
				}
			}
		}

		if(strongest >= UNIQUE) {
			// rule #2 - unique value in domain
			for(int x = 0; x < 9; x++) {
				for(int y = 0; y < 9; y++) {
					for(auto v : board[x][y].domain) {
						bool row_unique = true;
						bool col_unique = true;
						bool box_unique = true;

						for(int i = 0; i < 9; i++) {
							// if domain value exists in row cell's domain, it's not unique
							if(i != x && std::find(board[i][y].domain.begin(), board[i][y].domain.end(), v) != board[i][y].domain.end()) {
								row_unique = false;
							}

							// if domain value exists in column cell's domain, it's not unique
							if(i != y && std::find(board[x][i].domain.begin(), board[x][i].domain.end(), v) != board[x][i].domain.end()) {
								col_unique = false;
							}
						}

					
						for(auto xy : boxOtherCoords(x, y)) {
							// if domain value exists in box cell's domain, it's not unique
							if(std::find(board[xy.first][xy.second].domain.begin(), board[xy.first][xy.second].domain.end(), v) != board[xy.first][xy.second].domain.end()) {
								box_unique = false;
							}
						}
						if(row_unique || col_unique || box_unique) {
							board[x][y].value = v;
							applied = true;
						
							if(!updateDomains()) {
								return false;
							}

							break;
						}
					}
				}
			}
		}

		if(strongest >= NAKED_DOUBLES) {
			// rule #3 - naked doubles, k = 2
			/*
				for each cell 
					check if any other cells in row/col/box have same domain
					if so, record coords of the two and the domain. remove each domain item from all others in row/col/box.
			*/
			for(int x = 0; x < 9; x++) {
				for(int y = 0; y < 9; y++) {
					for(int i = 0; i < 9; i++) {
						bool sameDomRow = true;
						bool sameDomCol = true;
						// check for row cells with same domain
						for(auto v : board[x][y].domain) {
							// make sure everything in [x][y]'s domain is in [i][y]'s domain
							if(i != x && std::find(board[i][y].domain.begin(), board[i][y].domain.end(), v) == board[i][y].domain.end()) {
								sameDomRow = false;
							}

							// if domain value exists in column cell's domain, it's not unique
							if(i != y && std::find(board[x][i].domain.begin(), board[x][i].domain.end(), v) == board[x][i].domain.end()) {
								sameDomCol = false;
							}
						}

						// if they have the same domain and it is of size 2, remove from other domains
						if(sameDomRow && board[x][y].domain.size() == 2 && board[i][y].domain.size() == 2) {
							// remove from other domains in row
							for(auto v : board[x][y].domain) {
								for(int j = 0; j < 9; j++) {
									if(j != x && j != i) {
										// erase this val from [j][y]'s domain
										board[j][y].domain.erase(std::remove(board[j][y].domain.begin(), board[j][y].domain.end(), v), board[j][y].domain.end());
										applied = true;
						
										if(!updateDomains()) {
											return false;
										}
									}
								}
							}
							break;
						}
						if(sameDomCol && board[x][y].domain.size() == 2 && board[x][i].domain.size() == 2) {
							// remove from other domains in row
							for(auto v : board[x][y].domain) {
								for(int j = 0; j < 9; j++) {
									if(j != x && j != i) {
										// erase this val from [j][y]'s domain
										board[x][j].domain.erase(std::remove(board[x][j].domain.begin(), board[x][j].domain.end(), v), board[x][j].domain.end());
										applied = true;
						
										if(!updateDomains()) {
											return false;
										}
									}
								}
							}
							break;
						}
					}

					for(auto xy : boxOtherCoords(x, y)) {
						bool sameDomBox = true;
						for(auto v : board[x][y].domain) {
							// make sure everything in [x][y]'s domain is in [xy.first][xy.second]'s domain
							if(std::find(board[xy.first][xy.second].domain.begin(), board[xy.first][xy.second].domain.end(), v) == board[xy.first][xy.second].domain.end()) {
								sameDomBox = false;
							}
						}
						if(sameDomBox && board[x][y].domain.size() == 2 && board[xy.first][xy.second].domain.size() == 2) {
							// erase all values from [xy.first][xy.second]'s domain
							for(auto v : board[x][y].domain) {
								for(auto xy2 : boxOtherCoords(x, y)) {
									if(xy2.first != xy.first || xy2.second != xy.second) {
										board[xy2.first][xy2.second].domain.erase(std::remove(board[xy2.first][xy2.second].domain.begin(), board[xy2.first][xy2.second].domain.end(), v), board[xy2.first][xy2.second].domain.end());
										applied = true;
						
										if(!updateDomains()) {
											return false;
										}
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		if(strongest >= NAKED_TRIPLES) {
			// rule #4 - naked triples, k = 3
		}
	}

	return true;
}

// orders domain vector, least constrained to most
void
State::orderDomain(int x, int y) {
	std::vector<std::pair<int, int>> ordered_domain; // possible value, num of constraints
	for(auto v : board[x][y].domain) {
		int constraints = 0;
		// check rows and columns
		for(int i = 0; i < 9; i++) {
			// check if this value is in the domain of board[i,y]
			if(i != x && std::find(board[i][y].domain.begin(), board[i][y].domain.end(), v) != board[i][y].domain.end()) {
				constraints++;
			}

			// check if this value is in the domain of board[x,i]
			if(i != y && std::find(board[x][i].domain.begin(), board[x][i].domain.end(), v) != board[x][i].domain.end()) {
				constraints++;
			}
		}

		// check box
		for(auto xy : boxOtherCoords(x, y)) {
			if(std::find(board[xy.first][xy.second].domain.begin(), board[xy.first][xy.second].domain.end(), v) != board[xy.first][xy.second].domain.end()) {
				constraints++;
			}
		}
		ordered_domain.push_back(std::make_pair(v, constraints));
	}

	board[x][y].domain.clear();

	while(ordered_domain.size() > 0) {
		int min = INT_MAX;
		int idx = -1;

		// find min value
		for(int i = 0; i < ordered_domain.size(); i++) {
			if(ordered_domain[i].second < min) {
				idx = i;
				min = ordered_domain[i].second;
			}
		}

		// move it to domain
		board[x][y].domain.push_back(ordered_domain[idx].first);
		ordered_domain.erase(ordered_domain.begin() + idx);
	}
}

// prints solution, similar to input file format
std::string
State::print(void) {
	std::stringstream out;
	out << metadata;
	out << '\n';
	out << "backtracks: " << num_backtracks << '\n';
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			out << board[i][j].value;
			if(j == 8) {
				out << '\n';
			}
			else if(j == 5 || j == 2) {
				out << ' ';
			}
		}
		if(i % 3 == 2) {
			out << '\n';
		}
	}
	return out.str();
}

// initializes board to all unknowns with domain of 1-9
State::State(void)
{
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			board[i][j].value = 0;
			for(int k = 1; k <= 9; k++) {
				board[i][j].domain.push_back(k);
			}
		}
	}
}


State::~State(void)
{
}
