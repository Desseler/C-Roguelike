#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"

#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"
#include "io.h"

void do_combat(dungeon_t *d, character_t *atk, character_t *def)
{
  
  killChar(def);
  if (def != &d->pc) {
    d->num_monsters--;
  }
  increaseCharKills(atk, kill_direct);
  setCharKills(atk, kill_avenged, (getCharKills(atk, kill_avenged) + (getCharKills(def, kill_direct) +
								      getCharKills(def, kill_avenged))));
		 

  if (atk == &d->pc) {
    io_queue_message("You smite the %c", getCharSymbol(def));
  }
}

void move_character(dungeon_t *d, character_t *c, pair_t next)
{
  if (charpair(next) &&
      ((next[dim_y] != getCharPositionY(c)) ||
       (next[dim_x] != getCharPositionX(c)))) {
    do_combat(d, c, charpair(next));
  } else {
    /* No character in new position. */

    d->character[getCharPositionY(c)][getCharPositionX(c)] = NULL;
    setCharPositionY(c, next[dim_y]);    
    setCharPositionX(c, next[dim_x]);
    d->character[getCharPositionY(c)][getCharPositionX(c)] = c;
  }
}

void do_moves(dungeon_t *d)
{
  pair_t next;
  character_t *c;
  event_t *e;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    /* The PC always goes first one a tie, so we don't use new_event().  *
     * We generate one manually so that we can set the PC sequence       *
     * number to zero.                                                   */
    e = malloc(sizeof (*e));
    e->type = event_character_turn;
    /* The next line is buggy.  Monsters get first turn before PC.  *
     * Monster gen code always leaves PC in a monster-free room, so *
     * not a big issue, but it needs a better solution.             */
    e->time = d->time + (1000 / getCharSpeed(d->pc))
;
    e->sequence = 0;
    e->c = &d->pc;
    heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) &&
         (e = heap_remove_min(&d->events)) &&
         ((e->type != event_character_turn) || (e->c != &d->pc))) {
    d->time = e->time;
    if (e->type == event_character_turn) {
      c = e->c;
    }
    if (!getCharAlive(c)) {
      if (d->character[getCharPositionY(c)][getCharPositionX(c)] == c) {
        d->character[getCharPositionY(c)][getCharPositionX(c)] = NULL;
      }
      if (c != &d->pc) {
        event_delete(e);
      }
      continue;
    }

    npc_next_pos(d, c, next);
    move_character(d, c, next);

    heap_insert(&d->events, update_event(d, e, 1000 / getCharSpeed(c)));
  }

  io_display(d);
  if (pc_is_alive(d) && e->c == &d->pc) {
    c = e->c;
    d->time = e->time;
    /* Kind of kludgey, but because the PC is never in the queue when   *
     * we are outside of this function, the PC event has to get deleted *
     * and recreated every time we leave and re-enter this function.    */
    e->c = NULL;
    event_delete(e);
    /* Implementation of Fog of War */
    for(int j = getCharPositionY(d->pc) - 5; j < getCharPositionY(d->pc) + 6; j++){
      for(int i = getCharPositionX(d->pc) - 5; i < getCharPositionX(d->pc) + 6; i++){
	pcmapxy(i, j) = mapxy(i, j);
      }
    }
    io_handle_input(d);
  }
}

void dir_nearest_wall(dungeon_t *d, character_t *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (getCharPositionX(c) != 1 && getCharPositionX(c) != DUNGEON_X - 2) {
    dir[dim_x] = (getCharPositionX(c) > DUNGEON_X - getCharPositionX(c) ? 1 : -1);
  }
  if (getCharPositionY(c) != 1 && getCharPositionY(c) != DUNGEON_Y - 2) {
    dir[dim_y] = (getCharPositionY(c) > DUNGEON_Y - getCharPositionY(c) ? 1 : -1);
  }
}

uint32_t against_wall(dungeon_t *d, character_t *c)
{
  return ((mapxy(getCharPositionX(c) - 1,
                 getCharPositionY(c)    ) == ter_wall_immutable) ||
          (mapxy(getCharPositionX(c) + 1,
                 getCharPositionY(c)    ) == ter_wall_immutable) ||
          (mapxy(getCharPositionX(c)    ,
                 getCharPositionY(c) - 1) == ter_wall_immutable) ||
          (mapxy(getCharPositionX(c)    ,
                 getCharPositionY(c) + 1) == ter_wall_immutable));
}

uint32_t in_corner(dungeon_t *d, character_t *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(getCharPositionX(c) - 1,
                          getCharPositionY(c)    ) == ter_wall_immutable);
  num_immutable += (mapxy(getCharPositionX(c) + 1,
                          getCharPositionY(c)    ) == ter_wall_immutable);
  num_immutable += (mapxy(getCharPositionX(c)    ,
                          getCharPositionY(c) - 1) == ter_wall_immutable);
  num_immutable += (mapxy(getCharPositionX(c)    ,
                          getCharPositionY(c) + 1) == ter_wall_immutable);

  return num_immutable > 1;
}


static void new_dungeon_level(dungeon_t *d, uint32_t dir)
{
  /* Eventually up and down will be independantly meaningful. *
   * For now, simply generate a new dungeon.                  */

  switch (dir) {
  case '<':
  case '>':
    new_dungeon(d);
    break;
  default:
    break;
  }
}


uint32_t move_pc(dungeon_t *d, uint32_t dir)
{
  pair_t next;
  uint32_t was_stairs = 0;

  next[dim_y] = getCharPositionY(d->pc);
  next[dim_x] = getCharPositionX(d->pc);


  switch (dir) {
  case 1:
  case 2:
  case 3:
    next[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    next[dim_y]--;
    break;
  }
  switch (dir) {
  case 1:
  case 4:
  case 7:
    next[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    next[dim_x]++;
    break;
  case '<':
    if (mapxy(getCharPositionX(d->pc), getCharPositionY(d->pc)) == ter_stairs_up) {
      was_stairs = 1;
      new_dungeon_level(d, '<');
    }
    break;
  case '>':
    if (mapxy(getCharPositionX(d->pc), getCharPositionY(d->pc)) == ter_stairs_down) {
      was_stairs = 1;
      new_dungeon_level(d, '>');
    }
    break;
  }

  if (was_stairs) {
    return 0;
  }

  if ((dir != '>') && (dir != '<') && (mappair(next) >= ter_floor)) {
    move_character(d, &d->pc, next);
    io_update_offset(d);
    dijkstra(d);
    dijkstra_tunnel(d);

    return 0;
  }

  return 1;
}
