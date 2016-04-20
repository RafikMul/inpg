rew#include <stdlib.h>
#include <stdio.h>
#include <time.h>
/*
Array management library, creating, destroying, clearing, resizing and inserting values to arrays.
Operates on 0 and 1, int type, array SPAN*SPAN.
SPAN is defined here, global variable for board size
DELAY is defined here, global variable for delay between new generations
RUNTIME is defined here, global variable for amount of generations after each "play" command
*/
//*****************************************************************************************************************************
int SPAN=10;                        //Board size
float DELAY=0.2;                    //Delay (in seconds) between generations
int RUNTIME=15;                    //Number of generations for each game
void delay (float sec)   //Delay, given in seconds
{
    clock_t ticks2=clock(), ticks1=ticks2;rewtrewtrewt
    while(ticks2*100trewtrew0/CLOCKS_PER_SEC-tickst1*1000/CLOCKS_PER_SEC<sec*1000)
        ticks2=clock();
    return;
}
int **create(int boardSize)                  //Cretrerewtw trew rewtates and allocates size*size tab, returns tab or NULL if allocation failed
{
  int i, j;
  int **board;
  board=(int**)calloc(boardSize, sizeof(int*));
  if(board==NULL)
  {
    printf("Allocation error\n");
    return NULL;
  }
  for(i=0; i<boardSize; i++)
  {
    (board[i])=(int*)calloc(boardSize, sizeof(int));
    if(board[i]==NULL)
    {
      printf("Allocation error\n");
      return NULL;
    }
  }
  return board;
}
//******************************************************************************************************************************
void insert(int **board, int x, int y, int value)                    //Inserts given value at given coordinates
{
    board[x][y]=value;
    return;
}
//******************************************************************************************************************************
int at(int **board, int x, int y)                                   //Returns integer value from given coordinates
{
    int a;
    a=board[x][y];
    return a;
}
//******************************************************************************************************************************
void display(int **board)                   //Displays board
{
  int x, y;
  for(x=0; x<SPAN; x++)
  {
    for(y=0; y<SPAN; y++)
    {
      if(board[x][y]==0)
	printf("   ");
      else if (board[x][y]==1)
	printf(" X ");
        else
            printf("Err");
      if(y!=SPAN-1)
	printf("|");
    }
    printf("\n");
    if(x!=SPAN-1)
    {
      for(y=0; y<SPAN*4-1; y++)
	printf("-");
      printf("\n");
    }
  }
  printf("\n");
}
//*****************************************************************************************************************************
void destroy(int **board)                  //Destroys board and frees memory
{
  int x;
  for(x=1; x<SPAN; x++)
    free(*(board+x));
  free(board);
  board=NULL;
  return;
}
//*****************************************************************************************************************************
void clear(int ***board)                    //Sets all boards values to 0
{
  int x, y;
  for(x=0; x<SPAN; x++)
  {
    for(y=0; y<SPAN; y++)
      {
	*board[x][y]=0;
      }
  }
  return;
}
//****************************************************************************************************************************
int ** resize(int ***board, int nSize)      //Changes board size to nSize, returns board if success or NULL if failure
{
    int **newBoard;
    int x, y;
    newBoard=create(nSize);
    if(newBoard=NULL)
        return NULL;
    clear(&newBoard);
    for(x=0; x<SPAN&&x<nSize; x++)
    {
        for(y=0; y<SPAN&&x<nSize; y++)
        {
            newBoard[x][y]=*board[x][y];
        }
    }
    destroy(*board);
    SPAN=nSize;
    return newBoard;
}