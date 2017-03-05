#include <stdlib.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "npc.h"


#include <ncurses.h>

#define esc 27

void pc_delete(pc_t *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof (d->pc));
  d->pc.symbol = '@';

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  d->pc.npc = NULL;
  d->pc.kills[kill_direct] = d->pc.kills[kill_avenged] = 0;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

/* Added n_draw method for ncurses */

void n_draw(dungeon_t *d, pair_t pos)
{
  int i;
  int j;
  pair_t p;
  p[dim_y] = pos[dim_y];
  p[dim_x] = pos[dim_x];
  char ter;
  clear();
  
  for(i = 0; i < 80; i++){
    mvprintw(0, i, " ");
  }
  for(j = 1; j < 22; j++, p[dim_y]++){
    p[dim_x] = pos[dim_x];
    for(i = 0; i < 80; i++, p[dim_x]++){
      if (charpair(p)) {
	ter = charpair(p)->symbol;
      } else {	
	switch (mappair(p)) {
	case ter_wall:
	case ter_wall_immutable:
	  ter = ' ';
	  break;
	case ter_floor:
	case ter_floor_room:
	  ter = '.';
	  break;
	case ter_floor_hall:
	  ter = '#';
	  break;
	case ter_debug:
	  ter = '*';
	  fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
	  break;
	case ter_stair_up:
	  ter = '<';
	  break;
	case ter_stair_down:
	  ter = '>';
	  break;
	}
      }
      mvprintw(j, i, "%c", ter);
    }
  }
  mvprintw(22, 0, "PC is at x, y coordinate: (%i, %i)", d->pc.position[dim_x], d->pc.position[dim_y]);
  mvprintw(22, 40, "Monsters killed: %i", d->pc.kills[kill_direct]);
  mvprintw(23, 0, "Dungeon view is in x, y sector: (%i, %i)", (pos[dim_x]/80), (pos[dim_y]/21));
  mvprintw(23, 40, "Avenged: %i", d->pc.kills[kill_avenged]);
 
  refresh();
}

/*
 * Modified pc_next_pos to ask for input for ncurses.
 */

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  pair_t p;
  p[dim_y] = 21 * (d->pc.position[dim_y] / 21);
  p[dim_x] = 80 * (d->pc.position[dim_x] / 80);  
  char in;
  dir[dim_y] = dir[dim_x] = 0;
  int control = 1;
  int moved = 0;
  while(moved == 0){
    n_draw(d, p);
    in = getch();
    switch(in){
    case '7':
    case 'y':
      if(control == 1){	
	if(mapxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] - 1) >= ter_floor){
	  dir[dim_x] = -1;
	  dir[dim_y] = -1;
	  moved = 1;
	} 
      }
      break;
    case '8':
    case 'k':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x], d->pc.position[dim_y] - 1) >= ter_floor){
	  dir[dim_y] = -1;
	  moved = 1;
	}
      } else {
	p[dim_y] = p[dim_y] / 21;
	if(p[dim_y] > 0){
	  p[dim_y] -= 1;
	}
	p[dim_y] = p[dim_y] * 21;
      }
      break;
    case '9':
    case 'u':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] - 1) >= ter_floor){
	  dir[dim_x] = 1;
	  dir[dim_y] = -1;
	  moved = 1;
	}
      }      
      break;
    case '6':
    case 'l':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y]) >= ter_floor){
	  dir[dim_x] = 1;
	  moved = 1;
	}
      } else {
	if(p[dim_x] == 0){
	  p[dim_x] = 80;
	}
      }      
      break;
    case '3':
    case 'n':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] + 1) >= ter_floor){
	  dir[dim_x] = 1;
	  dir[dim_y] = 1;
	  moved = 1;
	}
      }     
      break;
    case '2':
    case 'j':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x], d->pc.position[dim_y] + 1) >= ter_floor){
	  dir[dim_y] = 1;
	  moved = 1;
	}
      } else {
	p[dim_y] = p[dim_y] / 21;
	if(p[dim_y] < 4){
	  p[dim_y] += 1;
	}
	p[dim_y] = p[dim_y] * 21;
      }
      break;
    case '1':
    case 'b':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] + 1) >= ter_floor){
	  dir[dim_x] = -1;
	  dir[dim_y] = 1;
	  moved = 1;
	}
      } 
      break;
    case '4':
    case 'h':
      if(control == 1){
	if(mapxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y]) >= ter_floor){
	  dir[dim_x] = -1;
	  moved = 1;
	}
      } else {
	if(p[dim_x] == 80){
	  p[dim_x] = 0;
	} 
      }
      break;
    case '5':
    case ' ':
      if(control == 1){
	moved = 1;
      }      
      break;
    case '>':
      if(mappair(d->pc.position) == ter_stair_down){
	d->regen = 1;
	moved = 1;
      }
      break;
    case '<':
      if(mappair(d->pc.position) == ter_stair_up){
	d->regen = 1;
	moved = 1;
      }
      break;
    case 'L':
      control = 0;
      break;
    case esc:
      control = 1;
      p[dim_y] = 21 * (d->pc.position[dim_y] / 21);
      p[dim_x] = 80 * (d->pc.position[dim_x] / 80);
      break;
    case 'Q':
      endwin();

      pc_delete(d->pc.pc);
      delete_dungeon(d);

      exit(EXIT_SUCCESS);
      break;
    }
  }
  
  return 0;
}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc.position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc.position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc.position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc.position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }
  
  return 0;
}
