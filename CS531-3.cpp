// CS533-3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "State.h"

extern unsigned long backtracks;
std::vector<State> solutions;

std::vector<State> loadProblems(std::string filename) {
	std::fstream problems_file(filename);
	std::vector<State> problems;

	if(problems_file.is_open()) {
		while(!problems_file.eof()) {
			State in;
			
			char meta[256];
			problems_file.getline(meta, 256);

			in.metadata = std::string(meta);
			if(in.metadata.length() == 0) {
				break;
			}

			// read rows 0-8
			for(int row = 0; row < 9; row++) {
				char buf[256];
				problems_file.getline(buf, 256);

				std::string line(buf);
				int idx = 0;
				for(int pos = 0; pos < line.size(); pos++) {
					if(line[pos] >= '0' && line[pos] <= '9') {
						in.board[row][idx].value = line[pos] - '0';

						if(line[pos] != '0') {
							in.board[row][idx].domain.clear();
							in.board[row][idx].domain.push_back(line[pos] - '0');
						}
						idx++;

					}
				}
			}
			problems.push_back(in);
			problems_file.getline(meta, 256);
		}
	}
	return problems;
}

// returns success or failure
bool backtrack(State problem) {
	// backtracking pseudocode
	// TODO: inference
	/*
	 params: assignment (initially empty), csp
	 if assign complete return
	 var <- select unassigned var
	 for each val in order-domain-values(var) do
		if value consistent with assignment
			add var = value to assignment
			inferences <- inference(csp, var, value)
			if inferences != failure then
				add inferences to assignment
				result <- backtrack(assignment, csp)
				if result != failure then
					return result
		remove var = value and inferences from assignment
	return failure
	*/

	if(problem.isFullyAssigned()) {
		solutions.push_back(problem);
		return true;
	}

	// select most constrained variable
	auto var = problem.selectUnassigned();
	//State::cell & cur = problem.board[var.first][var.second];

	// order cur.domain values from least to most constraining
	problem.orderDomain(var.first, var.second);

	for(auto v : problem.board[var.first][var.second].domain) {
		State new_val = problem;
		new_val.board[var.first][var.second].value = v;

		bool inference = new_val.constraintPropagation();
		if(inference) {
			// valid inference, continue to next var
			bool result = backtrack(new_val);
			if(result) {
				return result;
			}
		}
		backtracks++;
		// bad value choice, State new_val is reset
	}
	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto problems = loadProblems("sudoku.txt");
	
	for(int i = 0; i < 20; i++) {
		backtracks = 0;
		problems[i].constraintPropagation();
		bool result = backtrack(problems[i]);
		solutions.back().num_backtracks = backtracks;
	}

	for(auto s : solutions) {
		std::cout << s.print();
	}

	return 0;
}