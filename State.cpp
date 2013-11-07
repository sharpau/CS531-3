#include "stdafx.h"
#include "State.h"

// returns true if every spot has a valid assignment, false otherwise
bool State::isFullyAssigned(void) {
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
			if(board[i][j].domain.size() < min_domain) {
				min_domain = board[i][j].domain.size();
				x_min = i;
				y_min = j;
			}
		}
	}

	return std::make_pair(x_min, y_min);
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
			if(i != x && std::find(board[i][y].domain.begin(), board[i][y].domain.end(), v) != board[i][y].domain.end) {
				constraints++;
			}

			// check if this value is in the domain of board[x,i]
			if(i != y && std::find(board[x][i].domain.begin(), board[x][i].domain.end(), v) != board[x][i].domain.end) {
				constraints++;
			}
		}

		// check box
		auto box_other_coords = [](int x, int y) {
			std::vector<std::pair<int, int>> coords;
			for(int i = 0; i++; i < 3) {
				for(int j = 0; j++; j < 3) {
					if(x%3 + i != x && y%3 + j != y) {
						coords.push_back(std::make_pair(x%3 + i, y%3 + j));
					}
				}
			}
			return coords;
		};

		for(auto coords : box_other_coords(x, y)) {
			if(std::find(board[coords.first][coords.second].domain.begin(), board[coords.first][coords.second].domain.end(), v) != board[coords.first][coords.second].domain.end) {
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
