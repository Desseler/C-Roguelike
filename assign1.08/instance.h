#ifndef INSTANCE_H
# define INSTANCE_H

# include <stdint.h>
# include <vector>
# include <string>
# include "dice.h"
# include "descriptions.h"

class object_instance {
 private:
  std::string name, description;
  object_type_t type;
  uint32_t color, hit, dodge, defence, weight, speed, attribute, value;
  dice damage;
 public:
  object_instance();
  object_instance(std::string name, std::string description, object_type_t type,
		  dice hit, dice damage, dice dodge, dice defence, dice weight,
		  dice speed, dice attribute, dice value);
  void set(const std::string &name,
	   const std::string &description,
	   const object_type_t type,
	   const uint32_t color,
	   const uint32_t hit,
	   const dice &damage,
	   const uint32_t dodge,
	   const uint32_t defence,
	   const uint32_t weight,
	   const uint32_t speed,
	   const uint32_t attribute,
	   const uint32_t value);

  /*Copied accessors from description.h*/
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const object_type_t get_type() const { return type; }
  inline const uint32_t get_color() const { return color; }
  inline const uint32_t get_hit() const { return hit; }
  inline const dice &get_damage() const { return damage; }
  inline const uint32_t get_dodge() const { return dodge; }
  inline const uint32_t get_defence() const { return defence; }
  inline const uint32_t get_weight() const { return weight; }
  inline const uint32_t get_speed() const { return speed; }
  inline const uint32_t get_attribute() const { return attribute; }
  inline const uint32_t get_value() const { return value; }
}

#endif
