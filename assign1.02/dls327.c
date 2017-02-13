#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <time.h>

/*
 * dungeon.c from assignment1.01
 * by Trevor Luster
 * Modified for assignment1.02
 */
struct room{
  int x_pos;
  int y_pos;
  int x_size;
  int y_size;
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

void genHardness(int h[160][105])
{
  /*                                                                                                                                                                    
   * Initilize dungeon border hardness to 255                                                                                                                           
   * Hardness set at 255 will serve as the "immutable" flag                                                                                                             
   */
  for(int j = 0; j < 105; j++)
    {
      h[0][j] = 255;
      h[159][j] = 255;
    }
  for(int i = 1; i < 159; i++)
    {
      h[i][0] = 255;
      h[i][104] = 255;
    }

  /*                                                                                                                                                                    
   * Randomize dungeon rock hardness between 1 and 254                                                                                                                  
   * 0 = Dungeon tunnel or dungeon room                                                                                                                                 
   * 255 = Immutable dungeon border                                                                                                                                     
   */
  srand(time(NULL));
  for(int j = 1; j < 104; j++)
    {
      for(int i = 1; i < 159; i++)
	{
	  int hardness = (rand() % (254 + 1 - 1) + 1); //Random number between 1 and 254
	  h[i][j] = hardness;
	}
    }  
}

void genRooms(struct room *rooms, int h[160][105])
{
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
     	      h[i][j] = 0;
	    }
	}
    }  
}
/*
 * End modified dungeon.c code
 */



/*
 * Start dls327 main code
 *
 * ./dls327
 * ./dls327 --save
 * ./dls327 --load
 * ./dls327 --save --load
 * ./dls327 --load --save
 */

typedef enum action {
  action_default,
  action_save,
  action_load,
  action_save_load,
} action_t;

int h[160][105];
struct room *rooms = NULL;

int main(int argc, char *argv[])
{
  FILE *f;

  action_t action;

  if(argc > 3)
    {
      fprintf(stderr, "Wrong number of parameters.\n");

      return -1;
    }
  if(argc == 1)
    {
      action = action_default;
    } else { 
    if(!strcmp(argv[1], "--save"))
      {
	if(argc == 3)
	  {
	    if(!strcmp(argv[2], "--load"))
	      {
		action = action_save_load;
	      }
	  }
	else
	  {
	    action = action_save;
	  }
	  
      }
    else if(!strcmp(argv[1], "--load"))
      {
	if(argc == 3)
	  {
	    if(!strcmp(argv[2], "--save"))
	      {
		action = action_save_load;
	      }
	  }
	else
	  {
	    action = action_load;
	  }
      }
    else
      {
	fprintf(stderr, "Unrecognized switch.\n");
	
	return -1;
      }
  }

  char *path = getenv("HOME");
  strcat(path, "/.rlg327/");
  strcat(path, "dungeon");
  
  if(action == action_save)
    {
      if(!(f=fopen(path, "w")))
	{
	  fprintf(stderr, "Failed to open %s.\n", path);

	  return -1;
	}
    }
  if(action == action_load ||action == action_save_load)
    {
      if(!(f=fopen(path, "r")))
	{
	  fprintf(stderr, "Failed to open %s.\n", path);

	  return -1;
	}
    }
  

  char version[12] = {'R', 'L', 'G', '3', '2', '7', '-', 'S', '2', '0', '1', '7'};
  char character_buffer[12];
  switch (action) {
  case action_default:
    rooms = malloc(sizeof(struct room)*10);
    genHardness(h);
    genRooms(rooms, h);
    break;
  case action_load:
    {
      fseek(f, 0, SEEK_SET);
      fread(character_buffer, sizeof(char), 12, f);
      for(int i = 0; i < 12; i++)
	{
	  if(version[i] != character_buffer[i])
	    {
	      fprintf(stderr, "File type does not match.\n");
	      
	      return -1;
	    }
	}
      unsigned int version_buffer;
      fseek(f, 12, SEEK_SET);
      fread(&version_buffer, sizeof(unsigned int), 1, f);
      if(version_buffer != 0)
	{
	  fprintf(stderr, "File version does not match.\n");
	  
	  return -1;
	}
      unsigned int file_size;
      fseek(f, 16, SEEK_SET);
      fread(&file_size, sizeof(unsigned int), 1 ,f);
      file_size = be32toh(file_size);
      unsigned char hardness_buffer[160][105];
      fseek(f, 20, SEEK_SET);
      fread(hardness_buffer, sizeof(unsigned char), 16800, f);
      for(int j = 0; j < 105; j++)
	{
	  for(int i = 0; i < 160; i++)
	    {
	      
	      h[i][j] = (int)hardness_buffer[i][j];
	    }
	  printf("\n");
	}
      int roomcount = (file_size-16820)/4;
      unsigned char roominfo[roomcount*4];
      fseek(f, 16820, SEEK_SET);
      fread(roominfo, sizeof(unsigned char), roomcount, f);
      rooms = malloc(sizeof(struct room)*roomcount);
      int n = 0;
      for(int i = 0; i < roomcount; i ++)
	{
	  struct room newroom;
	  newroom.x_pos = (int)roominfo[n];
	  n++;
	  newroom.y_pos = (int)roominfo[n];
	  n++;
	  newroom.x_size = (int)roominfo[n];
	  n++;
	  newroom.y_size = (int)roominfo[n];
	  n++;
	  rooms[i] = newroom;
	}
      break;
    }
  case action_save:
    rooms = malloc(sizeof(struct room)*10);
    genHardness(h);
    genRooms(rooms, h);
    break;
  case action_save_load:
    break;
  }
  
  /*
   * Generate paths and display dungeon
   */
  char tile[160][105];
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
	  tile[i][j] = ' ';
	}
    }
  for(int m = 0; m < (sizeof(rooms)/sizeof(struct room)); m++)
    {
      for(int j = rooms[m].y_pos; j <= (rooms[m].y_pos + rooms[m].y_size); j++)
	{
	  for(int i = rooms[m].x_pos; i <= (rooms[m].x_pos + rooms[m].x_size); i++)
	    {
	      tile[i][j] = '.';
	      h[i][j] = 0;
	    }
	}
    }
  
  srand(123);
  /*
   * Generate paths between rooms
   */
  for(int i = 1; i < (sizeof(rooms)/sizeof(struct room)); i++)
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
	  if(tile[xx][yy] == ' ')
	    {
	      tile[xx][yy] = '#';
	      h[xx][yy] = 0;
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
	  if(tile[xx][yy] == ' ')
	    {
	      tile[xx][yy] = '#';
	      h[xx][yy] = 0;
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
	  printf("%c", tile[i][j]);
	}
      printf("\n");
    }
  free(rooms);
  
  return 0;
}
