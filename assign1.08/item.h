#ifndef ITEM_H
# define ITEM_H



# include <stdint.h>
# include <vector>
# include <string>
# include "dice.h"
# include "descriptions.h"

typedef struct dungeon dungeon_t;


class item {
 public:
  std::string name, description;
  object_type_t type;
  uint32_t color;
  int32_t hit, dodge, defence, weight, speed, attribute, value;
  dice damage;

  //item();
  /*
  item(std::string name, std::string description, object_type_t type,
		  dice hit, dice damage, dice dodge, dice defence, dice weight,
                  dice speed, dice attribute, dice value, uint32_t color);
  */
  char symbol();
};

void gen_items(dungeon_t *d);
void delete_items(dungeon_t *d);

#endif
