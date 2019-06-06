#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "omp.h"
#define N n
typedef enum {false,
              true
             } bool;

int n;
int numSol = 0; 

bool isLegal(int board[N][N], int row, int col)
{
    int i, j;
    for (i = 0; i < col; i++)
        if (board[row][i])
            return false;
 
    for (i=row, j=col; i>=0 && j>=0; i--, j--)
        if (board[i][j])
            return false;
 
    for (i=row, j=col; j>=0 && i<N; i++, j--)
        if (board[i][j])
            return false;
 
    return true;
}
 
bool recursiveMethod(int board[N][N], int col)
{
    if (col >= N) {
      if(N % 2 == 1){
        if(board[N/2][0] == 0 ){
          numSol++;  
        }
      } else {
        numSol++;
      }
    	numSol++;
 		  return true;
    }
    for (int i = 0; i < N; i++)
    {
        if ( isLegal(board, i, col) )
        {
            board[i][col] = 1;
            recursiveMethod(board, col + 1);
            board[i][col] = 0; 
        }
    }

    return false;
}

int calculateDepth(int threads, int boardSize) {

  if(threads*6 <= ((boardSize/2) + (boardSize % 2))){
    return 1;
  } else if (threads*6 <= ((boardSize/2)+(boardSize % 2))*boardSize) {
    return 2;
  }
  else { return 3;}
}

int getBranches(int depth, int boardSize){
  int base = boardSize/2 + (boardSize % 2); 
  if(depth == 1){
     return base ;
  } 
  else if (depth == 2){
    return base*boardSize;
  } 
  else {
    return base*boardSize*boardSize;
  }
}

int pow(int x, int y)
{
  int sum = x;
  for(int i = 0; i < y-1; i++){
    sum = sum*x;
  }
  return sum;
}

int main(int argc, char* argv[]) {
  int rank, size;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
  n = strtol(argv[1], NULL, 10);
  int requiredDepth = calculateDepth(size, N);
  int branches = getBranches(requiredDepth, N);
  int boards[branches][N][N];
  for(int i = 0; i < branches; i++){
    for(int j = 0; j < N; j++){
      for(int k = 0; k < N; k++){
        boards[i][j][k] = 0;
      }
    }
  }
int numProcs=omp_get_num_procs();
 double start= omp_get_wtime();

 #pragma omp parallel for num_threads(numProcs)
  for(int i = 0; i < branches; i++){
    for(int j = 0; j < (requiredDepth); j++){
      if(j == requiredDepth-1)
      {   
        if(isLegal(boards[i], (i % N), j))
        {
          boards[i][i % N][j] = 1;
        }
        else 
        {
          boards[i][i % N][j] = 1;
          boards[i][0][0] = -1;
        }
      } 
	  else 
      {
        if(isLegal(boards[i], (i/ ((requiredDepth - 1 - j) * N) % N), j)){
          boards[i][(i/ pow(N, (requiredDepth -1- j) )) % N][j] = 1;
        }
        else 
        {
          boards[i][(i/ pow(N, (requiredDepth -1- j) )) % N][j] = 1;
          boards[i][0][0] = -1;
        }
      }      
    }
  }
    
  int *gather;
  gather = malloc(N*sizeof(int));

  int *apointer;
  apointer = malloc(N*sizeof(int));

  int *recvbuf;
  recvbuf = malloc(sizeof(int));

  MPI_Status stat;
 
  if(rank != 0){
    bool quit = false;
    
    while(!quit) 
    {
      int value = 0;
      MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Recv(recvbuf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
      if(*recvbuf == -1)
      {
        quit = true;
      } 
	  else if ( *recvbuf == (branches -1) && boards[*recvbuf][0][0] != -1){
        recursiveMethod(boards[*recvbuf], requiredDepth);
        
      }
      else 
      {
        if(boards[*recvbuf][0][0] != -1)
        {
          recursiveMethod(boards[*recvbuf], requiredDepth);
        }
        
      }
    }
  } 
  else 
  {
    int branche = 0;
    while(branche < branches){
      MPI_Recv(recvbuf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
      MPI_Send(&branche, 1, MPI_INT, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
      branche++;
    }

    int state = 0;
    int stop = -1;
    while(state < size-1)
    {
      MPI_Recv(recvbuf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
      MPI_Send(&stop, 1, MPI_INT, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
      state++;
    }

  }
  apointer[0] = numSol;
  MPI_Reduce(apointer, gather, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if(rank == 0)
  { 
    double end = omp_get_wtime();
    printf("%d\n", *gather);
	printf("Time = %fs\n",end-start);
  }
  free(gather);
  free(apointer);

  MPI_Finalize(); 
  return 0;
}
