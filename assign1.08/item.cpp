#include <stdint.h>
#include <vector>
#include <string>
#include "dice.h"
#include "descriptions.h"
#include "item.h"

item::item()
{
  //this->damage = new dice();
  this->name = description = "";
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
}
