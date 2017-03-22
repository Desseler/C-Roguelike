#include <stdlib.h>

#include "character.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "dungeon.h"

character_t::character_t()
{
  symbol = '0';
  speed = 10;
  alive = 1;
  sequence_number = 0;
}

character_t::character_t(char sym, pair_t pos, int32_t spd, uint32_t seq)
{
  symbol = sym;
  position[dim_y] = pos[dim_y];
  position[dim_x] = pos[dim_x];
  speed = spd;
  alive = 1;
  sequence_number = seq;
}

uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist)
{
  /* Application of Bresenham's Line Drawing Algorithm.  If we can draw *
   * a line from v to e without intersecting any walls, then v can see  *
   * e.  Unfortunately, Bresenham isn't symmetric, so line-of-sight     *
   * based on this approach is not reciprocal (Helmholtz Reciprocity).  *
   * This is a very real problem in roguelike games, and one we're      *
   * going to ignore for now.  Algorithms that are symmetrical are far  *
   * more expensive.                                                    */

  pair_t first, second;
  pair_t del, f;
  int16_t a, b, c, i;

  first[dim_x] = voyeur->position[dim_x];
  first[dim_y] = voyeur->position[dim_y];
  second[dim_x] = exhibitionist->position[dim_x];
  second[dim_y] = exhibitionist->position[dim_y];

  if ((abs(first[dim_x] - second[dim_x]) > VISUAL_RANGE) ||
      (abs(first[dim_y] - second[dim_y]) > VISUAL_RANGE)) {
    return 0;
  }

  /*
  mappair(first) = ter_debug;
  mappair(second) = ter_debug;
  */

  if (second[dim_x] > first[dim_x]) {
    del[dim_x] = second[dim_x] - first[dim_x];
    f[dim_x] = 1;
  } else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }

  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  } else {
    del[dim_y] = first[dim_y] - second[dim_y];
    f[dim_y] = -1;
  }

  if (del[dim_x] > del[dim_y]) {
    a = del[dim_y] + del[dim_y];
    c = a - del[dim_x];
    b = c - del[dim_x];
    for (i = 0; i <= del[dim_x]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_x])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_x] += f[dim_x];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    return 1;
  } else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_y])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_y] += f[dim_y];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    return 1;
  }

  return 1;
}

char character_t::getSymbol()
{
  return symbol;
}

int16_t character_t::getPositionX()
{
  return position[dim_x];
}

int16_t character_t::getPositionY()
{
  return position[dim_y];
}

int32_t character_t::getSpeed()
{
  return speed;
}

uint32_t character_t::getAlive()
{
  return alive;
}

uint32_t character_t::getSequenceNumber()
{
  return sequence_number;
}

uint32_t character_t::getKills(kill_type_t num)
{
  return kills[num];
}

void character_t::setPositionX(int16_t pos)
{
  position[dim_x] = pos;
}

void character_t::setPositionY(int16_t pos)
{
  position[dim_y] = pos;
}
  
void character_t::kill()
{
  alive = 0;
}

void character_t::increaseKill(kill_type_t num)
{
  kills[num]++;
}

void character_delete(character_t *c)
{
  if (c) {
    delete c;
  }
}

char getCharSymbol(character_t *ch)
{
  return ch->getSymbol();
}

int16_t getCharPositionX(character_t *ch)
{
  return ch->getPositionX();
}

int16_t getCharPositionY(character_t *ch)
{
  return ch->getPositionY();
}

int32_t getCharSpeed(character_t *ch)
{
  return ch->getSpeed();
}

uint32_t getCharAlive(character_t *ch)
{
  return ch->getAlive();
}

uint32_t getCharSequenceNumber(character_t *ch)
{
  return ch-> getSequenceNumber();
}

uint32_t getCharKills(character_t *ch, kill_type_t num)
{
  return ch->getKills(num);
}

void setCharPositionX(character_t *ch, int16_t pos)
{
  ch->setPositionX(pos);
}

void setCharPositionY(character_t *ch, int16_t pos)
{
  ch->setPositionY(pos);
}

void killChar(character_t *ch)
{
  ch->kill();
}

void increaseCharKill(character_t *ch, kill_type_t num)
{
  ch->increaseKill(num);
}



int main()
{
  return 0;
}
