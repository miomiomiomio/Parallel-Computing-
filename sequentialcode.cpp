#include<bits/stdc++.h>
#include"omp.h"
using namespace std;

int n;
int numSol = 0;
bool isSafe(int **board, int row, int col,int n)
{
	int i, j;

	for (i = 0; i < col; i++)
		if (board[row][i])
			return false;

	for (i = row, j = col; i >= 0 && j >= 0; i--, j--)
		if (board[i][j])
			return false;

	for (i = row, j = col; j >= 0 && i<n; i++, j--)
		if (board[i][j])
			return false;

	return true;
}

bool recursiveQueenPlacement(int **board, int col,int n)
{
	if (col >= n) {
		if (n % 2 == 1) {
			if (board[n / 2][0] == 0) {
				numSol++;
			}
		}
		else {
			numSol++;
		}

		numSol++;

		return true;
	}

	for (int i = 0; i < n; i++)
	{

		if (isSafe(board, i, col,n))
		{

			board[i][col] = 1;
			recursiveQueenPlacement(board, col + 1,n);
			board[i][col] = 0;
		}
	}

	return false;
}

int main(int argc, char* argv[]) {

	n = strtol(argv[1], NULL, 10);
	int branches = n / 2 + n % 2;

	int ***boards = NULL;
	boards = (int***)new int **[branches];
	for (int j = 0; j < n; j++) {
		boards[j] = (int**)new int*[n];
	}
	for (int j = 0; j < n; j++)
		for (int k = 0; k < n; k++)
			boards[j][k] = new int[n];

	for (int i = 0; i < branches; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {
				boards[i][j][k] = 0;
			}
		}
	}
        double start = omp_get_wtime();
	for (int i = 0; i < branches; i++) {
		boards[i][i][0] = 1;
	}
	
	for (int i = 0; i < branches; i++) {
		recursiveQueenPlacement(boards[i], 1,n);
	}
	double end = omp_get_wtime();
	cout << numSol << endl;
	cout << "Time = " << end - start <<"s"<<endl;
	/*for (int i = 0; i < branches; i++)
	{
		for (int j = 0; j < n; j++)
		{
			delete[] boards[i][j];
		}
		delete[] boards[i];
	}
	delete[] boards;*/
	
	return 0;
}
