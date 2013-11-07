// CS533-3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "State.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// TODO: file IO
	std::fstream problems_file("sudoku.txt");
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



	// backtracking pseudocode
	// TODO: main, select_unassigned_var, order_domain_values, inference
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

	return 0;
}