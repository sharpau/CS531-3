#include "stdafx.h"
#include "State.h"


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
