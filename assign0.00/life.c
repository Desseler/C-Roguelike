#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void helper(int x, int y, char brd[][24], char tempbrd[][24])
{
  int count = 0;
  int alive = 0;
  if(brd[x][y] == 'O')
    {
      alive = 1;
    }
  for(int j = (y - 1); j < (y + 2); j++)
    {
      for(int i = (x - 1); i < (x + 2); i++)
	{
	  int xx = i;
	  int yy = j;
	  if(xx < 0)
	    {
	      xx = 79;
	    }
	  if(xx > 79)
	    {
	      xx = 0;
	    }
	  if(yy < 0)
	    {
	      yy = 23;
	    }
	  if(yy > 23)
	    {
	      yy = 0;
	    }
	  if(brd[xx][yy] == 'O')
            {
              count++;
	      if(i == x && j == y)
		{
		  count--;
		}
	      
            }
	}
    }
  if(alive == 0 && count == 3)
    {
      tempbrd[x][y] = 'O';
      return;
    }
  if(alive == 1 && count == 0 || count == 1)
    {
      tempbrd[x][y] = ' ';
      return;
    }
  if(alive == 1 && count == 2 || count == 3)
    {
      tempbrd[x][y] = 'O';
      return;
    }
  if(alive == 1 && count > 3)
    {
      tempbrd[x][y] = ' ';
      return;
    }
}

int main(int argc, char *argv[])
{
  char board[80][24];
  char tempboard[80][24];
  for(int j = 0; j < 24; j++)
    {
      for(int i = 0; i < 80; i++)
	{
	  board[i][j] = ' ';
	  tempboard[i][j] = ' ';
	}
    }

  for(int i = 1; i < argc; i++)
    {
      int x = atoi(argv[i]);
      i++;
      int y = atoi(argv[i]);
      board[x][y] = 'O';
      tempboard[x][y] = 'O';
    }

  while(1 < 2)
    {
      for(int j = 0; j < 24; j++)
	{
	  for(int i = 0; i < 80; i++)
	    {
	      helper(i, j, board, tempboard);
	    }
	}
      for(int j = 0; j < 24; j++)
	{
	  for(int i = 0; i < 80; i++)
	    {
	      board[i][j] = tempboard[i][j];
	    }
	}
      
      for(int j = 0; j < 24; j++)
	{
	  for(int i = 0; i < 80; i++)
	    {
	      printf("%c", board[i][j]);
	    }
	  printf("\n");
	}
      usleep(83333);
    }
  
  
  return 0;
  
}

