#include <stdio.h>
#include <stdlib.h>
#include <time.h>


struct room{
  int x_pos;
  int y_pos;
  int x_size;
  int y_size;
};

struct tile{
  int hardness;
  char floor;
};

int roomCheck(struct room roomA, struct room roomB)
{
  int aX1 = (roomA.x_pos - 1);
  int aX2 = (roomA.x_pos + roomA.x_size);
  int aY1 = (roomA.y_pos - 1);
  int aY2 = (roomA.y_pos + roomA.y_size);
  int bX1 = (roomB.x_pos - 1);
  int bX2 = (roomB.x_pos + roomB.x_size);
  int bY1 = (roomB.y_pos - 1);
  int bY2 = (roomB.y_pos + roomB.y_size);

  if(aX1 <= bX2 && aX2 >= bX1 && aY1 <= bY2 && aY2 >= bY1)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}
  
int main(int argc, char *argv[])
{
  struct tile dungeon[160][105]; //Dungeon layout with ASCII graphics
  /* 
   * Initialize all floor tiles to rock 
   * ' ' = Rock
   * '.' = Room
   * '#' = Tunnel
   */
  for(int j = 0; j < 105; j++)
    {
      for(int i = 0; i < 160; i++)
	{
	  dungeon[i][j].floor = ' ';
	}
    }
  
  /* 
   * Initilize dungeon border hardness to 255 
   * Hardness set at 255 will serve as the "immutable" flag
   */
  for(int j = 0; j < 105; j++)
    {
      dungeon[0][j].hardness = 255;
      dungeon[159][j].hardness = 255;
    }
  for(int i = 1; i < 159; i++)
    {
      dungeon[i][0].hardness = 255;
      dungeon[i][104].hardness = 255;
    }
  
  /* 
   * Randomize dungeon rock hardness between 1 and 254
   * 0 = Dungeon tunnel or dungeon room
   * 255 = Immutable dungeon border
   */
  int randno = atoi(argv[1]);
  srand(randno);
  for(int j = 1; j < 104; j++)
    {
      for(int i = 1; i < 159; i++)
	{
	  int hardness = (rand() % (254 + 1 - 1) + 1); //Random number between 1 and 254
	  dungeon[i][j].hardness = hardness;
	}
    }
  
  struct room rooms[10]; //Dungeon rooms
  /*
   * Generate 10 dungeon rooms
   */
  for(int i = 0; i < 10; i++)
    {
      struct room newroom; //Next room to be added to the array/dungeon
      int collisioncheck = 0; //While 0 in loop, create new room and check for collisions with other room
      while(collisioncheck == 0)
	{
	  /*
           * Generate rectangular room between 4x4 and 10x10 size
           */
	  newroom.x_pos = (rand() % (144 + 1 - 1) + 1);
	  newroom.x_size = (rand() % (15 + 1 - 7) + 7);
	  newroom.y_pos = (rand() % (89 + 1 - 1) + 1);
	  newroom.y_size = (rand() % (15 + 1 - 5) + 5);
	  collisioncheck = 1;
	  for(int j = 0; j < i; j++)
	    {
	      if(roomCheck(newroom, rooms[j]) == 1) //If room collides, break loop and regenerate room
		{
		  collisioncheck = 0;
		  break;
		}
		//If room doesn't collide, check with next existing room
	    }
	}
      rooms[i] = newroom;
      for(int j = newroom.y_pos; j <= (newroom.y_pos + newroom.y_size); j++)
	{
          for(int i = newroom.x_pos; i <= (newroom.x_pos + newroom.x_size); i++)
	    {
	      dungeon[i][j].floor = '.';
	      dungeon[i][j].hardness = 0;
	    }
	}
    }

  /*
   * Generate paths between rooms
   */
  for(int i = 1; i < 10; i++)
    {
      int aX = (rand() % ((rooms[i-1].x_size + rooms[i-1].x_pos) + 1 - rooms[i-1].x_pos) + rooms[i-1].x_pos);
      int aY = (rand() % ((rooms[i-1].y_size + rooms[i-1].y_pos) + 1 - rooms[i-1].y_pos) + rooms[i-1].y_pos);
      int bX = (rand() % ((rooms[i].x_size + rooms[i].x_pos) + 1 - rooms[i].x_pos) + rooms[i].x_pos);
      int bY = (rand() % ((rooms[i].y_size + rooms[i].y_pos) + 1 - rooms[i].y_pos) + rooms[i].y_pos);

      int xx = aX;
      int yy = aY;
      while(xx != bX)
	{
	  if(xx < bX)
	    {
	      xx++;
	    }
	  else
	    {
	      xx--;
	    }
	  if(dungeon[xx][yy].floor == ' ')
	    {
	      dungeon[xx][yy].floor = '#';
	      dungeon[xx][yy].hardness = 0;
	    }
	}

      while(yy != bY)
	{
	  if(yy < bY)
	    {
	      yy++;
	    }
	  else
	    {
	      yy--;
	    }
	  if(dungeon[xx][yy].floor == ' ')
	    {
	      dungeon[xx][yy].floor = '#';
	      dungeon[xx][yy].hardness = 0;
	    }
	}

    }
  
  /*
   * Print the generated dungeon
   */
  for(int j = 0; j < 105; j++)
    {
      for(int i = 0; i < 160; i++)
	{
	  printf("%c", dungeon[i][j].floor);
	}
      printf("\n");
    }


  return 0;
}

