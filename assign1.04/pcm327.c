#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "dungeon.h"
#include "path.h"

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n",
          name);

  exit(-1);
}

/*
 * Start Trevor's code
 */

static int32_t monster_cmp(const void *key, const void *with) {
  if(((monster_t *) key)->turn > ((monster_t *) with)->turn) {
    return 1;
  } else if(((monster_t *) key)->turn < ((monster_t *) with)->turn) {
    return -1;
  } else if(((monster_t *) key)->tie > ((monster_t *) with)->tie) {
    return 1;
  } else if(((monster_t *) key)->tie < ((monster_t *) with)->tie) {
    return -1;
  }
  return 0;
}

int line_of_sight(dungeon_t *d, int y, int x)
{
  int room_count = sizeof(d->rooms)/sizeof(d->rooms[0]);
  for(int i = 0; i < room_count; i++)
    {
      if(d->rooms[i].position[dim_y]                           <= d->pc.position[dim_y] &&
	 d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y] >= d->pc.position[dim_y] &&
	 d->rooms[i].position[dim_x]                           <= d->pc.position[dim_x] &&
	 d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x] >= d->pc.position[dim_x])
	{
	  if(d->rooms[i].position[dim_y]                           <= y &&
	     d->rooms[i].position[dim_y] + d->rooms[i].size[dim_y] >= y &&
	     d->rooms[i].position[dim_x]                           <= y &&
	     d->rooms[i].position[dim_x] + d->rooms[i].size[dim_x] >= y)
	    {
	      return 1;
	    }
	  else
	    {
	      return 0;
	    }
	}
    }
  return 0;
}

int move(dungeon_t *d, monster_t *m)
{
  int x = 0;
  int y = 0;
  uint16_t temp = 65535;
  int move = 0;
  pc_t pctemp;
  pair_t p;
  p[dim_y] = m->pos[dim_y];
  p[dim_x] = m->pos[dim_x];
  switch (d->m_map[p[dim_y]][p[dim_x]]->prop)
    {
    case '@': //PC
      y = 0;
      x = 0;
      move = 0;
      while(move == 0)
	{
	  y = d->pc.position[dim_y] + (rand() % ((d->pc.position[dim_y] + 1) + 1 - (d->pc.position[dim_y] - 1)) + (d->pc.position[dim_y] - 1));
	  x = d->pc.position[dim_x] + (rand() % ((d->pc.position[dim_x] + 1) + 1 - (d->pc.position[dim_x] - 1)) + (d->pc.position[dim_x] - 1));
	  if (mapxy(x, y) >= ter_floor)
	    {
	      move = 1;
	    }
	}
      if(d->m_map[y][x] != NULL)
	{
	  d->m_map[y][x]->hn = NULL;
	}
      d->m_map[p[dim_y]][p[dim_x]] = NULL;
      d->pc.position[dim_y] = y;
      d->pc.position[dim_x] = x;
      m->pos[dim_y] = y;
      m->pos[dim_x] = x;
      d->m_map[y][x] = m;
      
      return 1;
      
      break;
      
    case '0': //No Traits.
      if(line_of_sight(d, p[dim_y], p[dim_x]) == 1)
	{
	  y = 0;
	  x = 0;
	  if(d->pc.position[dim_y] > p[dim_y])
	    {
	      y = 1;
	    }
	  else
	    {
	      if(d->pc.position[dim_y] < p[dim_y])
		{
		  y = -1;
		}
	      else
		{
		  y = 0;
		}
	    }
	  if(d->pc.position[dim_x] > p[dim_x])
	    {
	      x = 1;
	    }
	  else
	    {
	      if(d->pc.position[dim_x] < p[dim_x])
		{
		  x = -1;
		}
	      else
		{
		  x = 0;
		}
	    }
	  if(mapxy(x, y) >= ter_floor)
	    {
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      return 0;
		    }
		}
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	      return 1;
	    }
	  else
	    {
	      if(mapxy(x, p[dim_y]) >= ter_floor)
		{
		  if(d->m_map[p[dim_y]][x] != NULL)
		{
		  d->m_map[p[dim_y]][x]->hn = NULL;
		  if(d->m_map[p[dim_y]][x]->prop == '@')
		    {
		      return 0;
		    }
		}
		
		d->m_map[p[dim_y]][p[dim_x]] = NULL;
		m->pos[dim_x] = x;
		d->m_map[p[dim_y]][x] = m;
		
		return 1;
		}
	      else
		{
		  if(mapxy(p[dim_x], y) >= ter_floor)
		    {
		      if(d->m_map[y][p[dim_x]] != NULL)
			{
			  d->m_map[y][p[dim_x]]->hn = NULL;
			  if(d->m_map[y][p[dim_x]]->prop == '@')
			    {
			      return 0;
			    }
			}
		      
		      d->m_map[p[dim_y]][p[dim_x]] = NULL;
		      m->pos[dim_y] = y;
		      d->m_map[y][p[dim_x]] = m;

		      return 1;
		    }
		  return 1;
		}
	    }
	}
      else
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) >= ter_floor)
		{
		  move = 1;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      break;
      
    case '1': //Intelligence
      if(line_of_sight(d, p[dim_y], p[dim_x]) == 1)
	{
	  m->pc.position[dim_y] = d->pc.position[dim_y];
	  m->pc.position[dim_x] = d->pc.position[dim_x];
	  dijkstra(d);
	  x = 0;
	  y = 0;
	  temp = 65535;
	  for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
	    {
	      for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
		{
		  if(d->pc_distance[j][i] < temp)
		    {
		      temp = d->pc_distance[j][i];
		      y = j;
		      x = i;
		    }
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      else
	{
	  if(m->pc.position[dim_y] != 0)
	    {
	      pctemp = d->pc;
	      d->pc = m->pc;
	      dijkstra(d);
	      d->pc = pctemp;
	      x = 0;
	      y = 0;
	      temp = 65535;
	      for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
		{
		  for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
		    {
		      if(d->pc_distance[j][i] < temp)
			{
			  temp = d->pc_distance[j][i];
			  y = j;
			  x = i;
			}
		    }
		}
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	    }
	  else
	    {
	      y = 0;
	      x = 0;
	      move = 0;
	      while(move == 0)
		{
		  y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
		  x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
		  if (mapxy(x, y) >= ter_floor)
		    {
		      move = 1;
		    }
		}
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;

	      return 1;
	    }
	}
      break;
      
    case '2': //Telepathy
      y = 0;
      x = 0;
      if(d->pc.position[dim_y] > p[dim_y])
	{
	  y = 1;
	}
      else
	{
	  if(d->pc.position[dim_y] < p[dim_y])
	    {
	      y = -1;
	    }
	  else
	    {
	      y = 0;
	    }
	}
      if(d->pc.position[dim_x] > p[dim_x])
	{
	  x = 1;
	}
      else
	{
	  if(d->pc.position[dim_x] < p[dim_x])
	    {
	      x = -1;
	    }
	  else
	    {
	      x = 0;
	    }
	}
      if(mapxy(x, y) >= ter_floor)
	{
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      else
	{
	  if(mapxy(x, p[dim_y]) >= ter_floor)
	    {
	      if(d->m_map[p[dim_y]][x] != NULL)
		{
		  d->m_map[p[dim_y]][x]->hn = NULL;
		  if(d->m_map[p[dim_y]][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_x] = x;
	      d->m_map[p[dim_y]][x] = m;
	      
	      return 1;
	    }
	  else
	    {
	      if(mapxy(p[dim_x], y) >= ter_floor)
		{
		  if(d->m_map[y][p[dim_x]] != NULL)
		    {
		      d->m_map[y][p[dim_x]]->hn = NULL;
		      if(d->m_map[y][p[dim_x]]->prop == '@')
			{
			  //END GAME***
			  return 0;
			}
		    }
		  
		  d->m_map[p[dim_y]][p[dim_x]] = NULL;
		  m->pos[dim_y] = y;
		  d->m_map[y][p[dim_x]] = m;
		  
		}
	      return 1;
	    }
	}
      break;
      
    case '3': //Telepathy & Intelligence
      dijkstra(d);
      x = 0;
      y = 0;
      temp = 65535;
      for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
	{
	  for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
	    {
	      if(d->pc_distance[j][i] < temp)
		{
		  temp = d->pc_distance[j][i];
		  y = j;
		  x = i;
		}
	    }
	}
      if(d->m_map[y][x] != NULL)
	{
	  d->m_map[y][x]->hn = NULL;
	  if(d->m_map[y][x]->prop == '@')
	    {
	      //END GAME***
	      return 0;
	    }
	}
      
      d->m_map[p[dim_y]][p[dim_x]] = NULL;
      m->pos[dim_y] = y;
      m->pos[dim_x] = x;
      d->m_map[y][x] = m;

      return 1;
      
      break;
      
    case '4': //Tunneling
      if(line_of_sight(d, p[dim_y], p[dim_x]) == 1)
	{
	  y = 0;
	  x = 0;
	  if(d->pc.position[dim_y] > p[dim_y])
	    {
	      y = 1;
	    }
	  else
	    {
	      if(d->pc.position[dim_y] < p[dim_y])
		{
		  y = -1;
		}
	      else
		{
		  y = 0;
		}
	    }
	  if(d->pc.position[dim_x] > p[dim_x])
	    {
	      x = 1;
	    }
	  else
	    {
	      if(d->pc.position[dim_x] < p[dim_x])
		{
		  x = -1;
		}
	      else
		{
		  x = 0;
		}
	    }
	  if(mapxy(x, y) >= ter_floor)
	    {
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	      
	      return 1;
	    }
	  else
	    {
	      if(mapxy(x, p[dim_y]) >= ter_floor)
		{
		  if(d->m_map[p[dim_y]][x] != NULL)
		    {
		      d->m_map[p[dim_y]][x]->hn = NULL;
		      if(d->m_map[p[dim_y]][x]->prop == '@')
			{
			  //END GAME***
			  return 0;
			}
		    }
		  
		  d->m_map[p[dim_y]][p[dim_x]] = NULL;
		  m->pos[dim_x] = x;
		  d->m_map[p[dim_y]][x] = m;
		  
		  
		  return 1;
		}
	      else
		{
		  if(mapxy(p[dim_x], y) >= ter_floor)
		    {
		      if(d->m_map[y][p[dim_x]] != NULL)
			{
			  d->m_map[y][p[dim_x]]->hn = NULL;
			  if(d->m_map[y][p[dim_x]]->prop == '@')
			    {
			      //END GAME***
			      return 0;
			    }
			}
		      
		      d->m_map[p[dim_y]][p[dim_x]] = NULL;
		      m->pos[dim_y] = y;
		      d->m_map[y][p[dim_x]] = m;
		      return 1;
		    }
		  return 1;
		}
	    }
	}
      else
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) != ter_wall_immutable)
		{
		  move = 1;
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      break;
      
    case '5': //Tunneling & Intelligence
      if(line_of_sight(d, p[dim_y], p[dim_x]) == 1)
	{
	  m->pc.position[dim_y] = d->pc.position[dim_y];
	  m->pc.position[dim_x] = d->pc.position[dim_x];
	  dijkstra_tunnel(d);
	  x = 0;
	  y = 0;
	  temp = 65535;
	  for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
	    {
	      for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
		{
		  if(d->pc_tunnel[j][i] < temp)
		    {
		      temp = d->pc_tunnel[j][i];
		      y = j;
		      x = i;
		    }
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      else
	{
	  if(m->pc.position[dim_y] != 0)
	    {
	      pctemp = d->pc;
	      d->pc = m->pc;
	      dijkstra_tunnel(d);
	      d->pc = pctemp;
	      x = 0;
	      y = 0;
	      temp = 65535;
	      for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
		{
		  for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
		    {
		      if(d->pc_tunnel[j][i] < temp)
			{
			  temp = d->pc_tunnel[j][i];
			  y = j;
			  x = i;
			}
		    }
		}
	      if(mapxy(x, y) == ter_wall)
		{
		  if(hardnessxy(x, y) > 85)
		    {
		      hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		      
		      return 1;
		    }
		  else
		    {
		      hardnessxy(x, y) = 0;
		      mapxy(x, y) = ter_floor_hall;
		    }
		}
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	    }
	  else
	    {
	      y = 0;
	      x = 0;
	      move = 0;
	      while(move == 0)
		{
		  y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
		  x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
		  if (mapxy(x, y) != ter_wall_immutable)
		    {
		      move = 1;
		    }
		}
	      if(mapxy(x, y) == ter_wall)
		{
		  if(hardnessxy(x, y) > 85)
		    {
		      hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		      
		      return 1;
		    }
		  else
		    {
		      hardnessxy(x, y) = 0;
		      mapxy(x, y) = ter_floor_hall;
		    }
		}
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	      return 1;
	    }
	}
      break;
      
    case '6': //Tunneling & Telepathy
      y = 0;
      x = 0;
      if(d->pc.position[dim_y] > p[dim_y])
	{
	  y = 1;
	}
      else
	{
	  if(d->pc.position[dim_y] < p[dim_y])
	    {
	      y = -1;
	    }
	  else
	    {
	      y = 0;
	    }
	}
      if(d->pc.position[dim_x] > p[dim_x])
	{
	  x = 1;
	}
      else
	{
	  if(d->pc.position[dim_x] < p[dim_x])
	    {
	      x = -1;
	    }
	  else
	    {
	      x = 0;
	    }
	}
      if(mapxy(x, y) == ter_wall)
	{
	  if(hardnessxy(x, y) > 85)
	    {
	      hardnessxy(x, y) = (hardnessxy(x, y) - 85);

	      return 1;
	    }
	  else
	    {
	      hardnessxy(x, y) = 0;
	      mapxy(x, y) = ter_floor_hall;
	    }
	}
      if(d->m_map[y][x] != NULL)
	{
	  d->m_map[y][x]->hn = NULL;
	  if(d->m_map[y][x]->prop == '@')
	    {
	      //END GAME***
	      return 0;
	    }
	}
      
      d->m_map[p[dim_y]][p[dim_x]] = NULL;
      m->pos[dim_y] = y;
      m->pos[dim_x] = x;
      d->m_map[y][x] = m;
      
      return 1;
	  
      break;
      
    case '7': //Tunneling & Telepathy & Intelligence
      dijkstra_tunnel(d);
      x = 0;
      y = 0;
      temp = 65535;
      for(int j = p[dim_y] - 1; j < p[dim_y] + 2; j++)
	{
	  for(int i = p[dim_x] - 1; i < p[dim_x] + 2; i++)
	    {
	      if(d->pc_tunnel[j][i] < temp)
		{
		  temp = d->pc_tunnel[j][i];
		  y = j;
		  x = i;
		}
	    }
	}
      if(mapxy(x, y) == ter_wall)
	{
	  if(hardnessxy(x, y) > 85)
	    {
	      hardnessxy(x, y) = (hardnessxy(x, y) - 85);

	      return 1;
	    }
	  else
	    {
	      hardnessxy(x, y) = 0;
	      mapxy(x, y) = ter_floor_hall;
	    }
	}
      if(d->m_map[y][x] != NULL)
	{
	  d->m_map[y][x]->hn = NULL;
	  if(d->m_map[y][x]->prop == '@')
	    {
	      //END GAME***
	      return 0;
	    }
	}
      
      d->m_map[p[dim_y]][p[dim_x]] = NULL;
      m->pos[dim_y] = y;
      m->pos[dim_x] = x;
      d->m_map[y][x] = m;

      return 1;
      break;
      
    case '8': //Erratic
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) >= ter_floor)
		{
		  move = 1;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	}
      else
	{
	  if(line_of_sight(d, p[dim_y], p[dim_x]) == 1)
	    {
	      y = 0;
	      x = 0;
	      if(d->pc.position[dim_y] > p[dim_y])
		{
		  y = 1;
		}
	      else
		{
		  if(d->pc.position[dim_y] < p[dim_y])
		    {
		      y = -1;
		    }
		  else
		    {
		      y = 0;
		    }
		}
	      if(d->pc.position[dim_x] > p[dim_x])
		{
		  x = 1;
		}
	      else
		{
		  if(d->pc.position[dim_x] < p[dim_x])
		    {
		      x = -1;
		    }
		  else
		    {
		      x = 0;
		    }
		}
	      if(mapxy(x, y) >= ter_floor)
		{
		  if(d->m_map[y][x] != NULL)
		    {
		      d->m_map[y][x]->hn = NULL;
		      if(d->m_map[y][x]->prop == '@')
			{
			  //END GAME***
			  return 0;
			}
		    }
		  
		  d->m_map[p[dim_y]][p[dim_x]] = NULL;
		  m->pos[dim_y] = y;
		  m->pos[dim_x] = x;
		  d->m_map[y][x] = m;
		  
		  return 1;
		}
	      else
		{
		  if(mapxy(x, p[dim_y]) >= ter_floor)
		    {
		      if(d->m_map[p[dim_y]][x] != NULL)
			{
			  d->m_map[p[dim_y]][x]->hn = NULL;
			  if(d->m_map[p[dim_y]][x]->prop == '@')
			    {
			      //END GAME***
			      return 0;
			    }
			}
		      
		      d->m_map[p[dim_y]][p[dim_x]] = NULL;
		      m->pos[dim_x] = x;
		      d->m_map[p[dim_y]][x] = m;
		      
		      return 1;
		    }
		  else
		    {
		      if(mapxy(p[dim_x], y) >= ter_floor)
			{
			  if(d->m_map[y][p[dim_x]] != NULL)
			    {
			      d->m_map[y][p[dim_x]]->hn = NULL;
			      if(d->m_map[y][p[dim_x]]->prop == '@')
				{
				  //END GAME***
				  return 0;
				}
			    }
			  
			  d->m_map[p[dim_y]][p[dim_x]] = NULL;
			  m->pos[dim_y] = y;
			  d->m_map[y][p[dim_x]] = m;

			  return 1;
			}
		      return 1;
		    }
		}
	    }
	  else
	    {
	      y = 0;
	      x = 0;
	      move = 0;
	      while(move == 0)
		{
		  y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
		  x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
		  if (mapxy(x, y) >= ter_floor)
		    {
		      move = 1;
		    }
		}
	      if(d->m_map[y][x] != NULL)
		{
		  d->m_map[y][x]->hn = NULL;
		  if(d->m_map[y][x]->prop == '@')
		    {
		      //END GAME***
		      return 0;
		    }
		}
	      
	      d->m_map[p[dim_y]][p[dim_x]] = NULL;
	      m->pos[dim_y] = y;
	      m->pos[dim_x] = x;
	      d->m_map[y][x] = m;
	      
	      return 1;
	    }
	}
      break;
      
    case '9': //Erratic & Intelligence
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) >= ter_floor)
		{
		  move = 1;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	} else {
	//TODO***
      }
      break;
      
    case 'a': //Erratic & Telepathy
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) >= ter_floor)
		{
		  move = 1;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;

	  return 1;
	} else {
	//TODO***
      }
      break;
      
    case 'b': //Erratic & Telepathy & Intelligence
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) >= ter_floor)
		{
		  move = 1;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;

	  return 1;
	} else {
	//TODO***
      }
      break;
      
    case 'c': //Erratic & Tunneling
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) != ter_wall_immutable)
		{
		  move = 1;
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;

	  return 1;
	} else {
	//TODO***
      }
      break;
      
    case 'd': //Erratic & Tunneling & Intelligence
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) != ter_wall_immutable)
		{
		  move = 1;
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;

	  return 1;
	} else {
	//TODO***
      }
      
      break;
      
    case 'e': //Erratic & Tunneling & Telepathy
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) != ter_wall_immutable)
		{
		  move = 1;
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;

	  return 1;
	} else {
	//TODO***
	return 0;
      }
      
      break;
      
    case 'f': //Erratic & Tunneling & Telepathy & Intelligence
      if(rand() % 2 == 0)
	{
	  y = 0;
	  x = 0;
	  move = 0;
	  while(move == 0)
	    {
	      y = p[dim_y] + (rand() % ((p[dim_y] + 1) + 1 - (p[dim_y] - 1)) + (p[dim_y] - 1));
	      x = p[dim_x] + (rand() % ((p[dim_x] + 1) + 1 - (p[dim_x] - 1)) + (p[dim_x] - 1));
	      if (mapxy(x, y) != ter_wall_immutable)
		{
		  move = 1;
		}
	    }
	  if(mapxy(x, y) == ter_wall)
	    {
	      if(hardnessxy(x, y) > 85)
		{
		  hardnessxy(x, y) = (hardnessxy(x, y) - 85);
		  
		  return 1;
		}
	      else
		{
		  hardnessxy(x, y) = 0;
		  mapxy(x, y) = ter_floor_hall;
		}
	    }
	  if(d->m_map[y][x] != NULL)
	    {
	      d->m_map[y][x]->hn = NULL;
	      if(d->m_map[y][x]->prop == '@')
		{
		  //END GAME***
		  return 0;
		}
	    }
	  
	  d->m_map[p[dim_y]][p[dim_x]] = NULL;
	  m->pos[dim_y] = y;
	  m->pos[dim_x] = x;
	  d->m_map[y][x] = m;
	  
	  return 1;
	} else {
	//TODO***
      }
      
      break;  
    }
  return 1; 
}

void add_pc(dungeon_t *d, pair_t pc, heap_t *h){
  heap_init(h, monster_cmp, NULL);
  monster_t * monster = (monster_t *) malloc(sizeof(monster_t));
  monster->pos[dim_y] = pc[dim_y];
  monster->pos[dim_x] = pc[dim_x];
  monster->prop = '@';
  monster->tie = 0;
  monster->speed = 10;
  monster->hn = heap_insert(h, monster);
  d->m_map[pc[dim_y]][pc[dim_x]] = monster;
  d->pc.position[dim_y] = pc[dim_y];
  d->pc.position[dim_x] = pc[dim_x];
}

void add_monsters(dungeon_t *d, int monster_count, heap_t *h)
{
  //heap_init(&h, monster_cmp, NULL);
  for(int i = 0; i < monster_count; i++)
    {
      int x = 0;
      int y = 0;
      while(mapxy(x, y) < ter_floor)
	{
	  x = rand() % (DUNGEON_X - 1) + 1;
	  y = rand() % (DUNGEON_Y - 1) + 1;
	}
      int prop = rand() % 16;
      char property;
      switch (prop)
	{
	case 0:
	  property = '0';
	  break;

	case 1:
	  property = '1';
	  break;
	  
	case 2:
	  property = '2';
	  break;

	case 3:
	  property = '3';
	  break;

	case 4:
	  property = '4';
	  break;

	case 5:
	  property = '5';
	  break;

	case 6:
	  property = '6';
	  break;

	case 7:
	  property = '7';
	  break;

	case 8:
	  property = '8';
	  break;

	case 9:
	  property = '9';
	  break;
	  
	case 10:
	  property = 'a';
	  break;

	case 11:
	  property = 'b';
	  break;

	case 12:
	  property = 'c';
	  break;

	case 13:
	  property = 'd';
	  break;

	case 14:
	  property = 'e';
	  break;

	case 15:
	  property = 'f';
	  break;

	default:
	  break;
	}
      monster_t * monster = (monster_t *) malloc(sizeof(monster_t));
      monster->pos[dim_y] = y;
      monster->pos[dim_x] = x;
      monster->prop = property;
      monster->tie = i + 1;
      monster->speed = rand() % (20 + 1 - 5) + 5;
      monster->hn = heap_insert(h, monster);
      d->m_map[y][x] = monster;
    }
}

//void floor(dungeon_t *d, heap_t *h)
//{
//}
 
/*
 * End Trevor's code
 */

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_place_pc;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_place_pc = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            save_file = argv[++i];
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        case 'p':
          if ((!long_arg && argv[i][2])            ||
              (long_arg && strcmp(argv[i], "-pc")) ||
              argc <= i + 2                        ||
              argv[i + 1][0] == '-'                ||
              argv[i + 2][0] == '-') {
            usage(argv[0]);
          }
          do_place_pc = 1;
          if ((d.pc.position[dim_y] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_y] > DUNGEON_Y - 2         ||
              (d.pc.position[dim_x] = atoi(argv[++i])) < 1 ||
              d.pc.position[dim_x] > DUNGEON_X - 2) {
            fprintf(stderr, "Invalid PC position.\n");
            usage(argv[0]);
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_place_pc) {
    i = rand() % d.num_rooms;
    d.pc.position[dim_x] = (d.rooms[i].position[dim_x] +
                            (rand() % d.rooms[i].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[i].position[dim_y] +
                            (rand() % d.rooms[i].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  heap_t h;
  add_pc(&d, d.pc.position, &h);
  add_monsters(&d, 100, &h);
  
  render_dungeon(&d);

  printf("Made it past 'render_dungeon'");
  
  dijkstra(&d);
  dijkstra_tunnel(&d);
  render_distance_map(&d);
  render_tunnel_distance_map(&d);

  if (do_save) {
    write_dungeon(&d, save_file);
  }

  delete_dungeon(&d);

  return 0;
}
