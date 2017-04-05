#include <stdint.h>
#include <vector>
#include <string>

#include "utils.h"
#include "dice.h"
#include "descriptions.h"
#include "item.h"
#include "dungeon.h"

/*
item::item()
{
}

item::item(std::string name, std::string description, object_type_t type,
	   dice hit, dice damage, dice dodge, dice defence, dice weight,
	   dice speed, dice attribute, dice value, uint32_t color)
{
  this->color = color;
  this->hit = hit.roll();
  this->dodge = dodge.roll();
  this->defence = defence.roll();
  this->weight = weight.roll();
  this->speed = speed.roll();
  this->attribute = attribute.roll();
  this->value = value.roll();
  this->damage = damage;
  this->type = type;
  this->name = name;
  this->description = description;
}
*/

char item::symbol()
{
  switch(type){
  case 0:
    return '*';
  case 1:
    return '|';
  case 2:
    return ')';
  case 3:
    return '}';
  case 4:
    return '[';
  case 5:
    return ']';
  case 6:
    return '(';
  case 7:
    return '{';
  case 8:
    return '\\';
  case 9:
    return '=';
  case 10:
    return '\"';
  case 11:
    return '_';
  case 12:
    return '~';
  case 13:
    return '?';
  case 14:
    return '!';
  case 15:
    return '$';
  case 16:
    return '/';
  case 17:
    return ',';
  case 18:
    return '-';
  case 19:
    return '%';
  default:
    return '*';
  }
}

void gen_items(dungeon_t *d)
{
  uint32_t i;
  item *o;
  uint32_t room;
  pair_t p;
  object_description obj;

  for (i = 0; i < 50; i++) {
    obj = d->object_descriptions[rand_range(0, d->object_descriptions.size() - 1)];
    o = new item();
    memset(o, 0, sizeof (*o));
    o = obj.create_item();

    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
			    (d->rooms[room].position[dim_y] +
			     d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
			    (d->rooms[room].position[dim_x] +
			     d->rooms[room].size[dim_x] - 1));
    } while (d->item_map[p[dim_y]][p[dim_x]]);
    //o->position[dim_y] = p[dim_y];
    //o->position[dim_x] = p[dim_x];
    d->item_map[p[dim_y]][p[dim_x]] = o;

    /*
    o->color = obj.color;
    o->hit = obj.hit.roll();
    o->dodge = obj.dodge.roll();
    o->defence = obj.defence.roll();
    o->weight = obj.weight.roll();
    o->speed = obj.speed.roll();
    o->attribute = obj.attribute.roll();
    o->value = obj.value.roll();
    o->damage = obj.damage;
    o->type = obj.type;
    o->name = obj.name;
    o->description = obj.description;   
    
    d->item_map[p[dim_y]][p[dim_x]] = o;
    */
  }
}

void delete_items(dungeon_t *d)
{
  int x;
  int y;
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if(itemxy(x, y)) {
	delete itemxy(x, y);
	itemxy(x, y) = NULL;
      }
    }
  }
}
