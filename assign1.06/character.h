#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>

# include "dims.h"

typedef struct dungeon dungeon_t;
typedef struct dice_t dice_t;

typedef enum kill_type {
  kill_direct,
  kill_avenged,
  num_kill_types
} kill_type_t;

#ifdef __cplusplus

class character_t {
 public:
  char symbol;
  pair_t position;
  int32_t speed;
  uint32_t alive;
  /* Characters use to have a next_turn for the move queue.  Now that it is *
   * an event queue, there's no need for that here.  Instead it's in the    *
   * event.  Similarly, sequence_number was introduced in order to ensure   *
   * that the queue remains stable.  Also no longer necessary here, but in  *
   * this case, we'll keep it, because it provides a bit of interesting     *
   * metadata: locally, how old is this character; and globally, how many   *
   * characters have been created by the game.                              */
  uint32_t sequence_number;
  uint32_t kills[num_kill_types];


  character_t();
  character_t(char sym, pair_t pos, int32_t spd, uint32_t seq);

  char getSymbol();
  int16_t getPositionX();
  int16_t getPositionY();
  int32_t getSpeed();
  uint32_t getAlive();
  uint32_t getSequenceNumber();
  uint32_t getKills(kill_type_t num);

  void setPositionX(int16_t pos);
  void setPositionY(int16_t pos);
  void kill();
  void increaseKill(kill_type_t num);
  void setKill(kill_type_t num, uint32_t amount);

  ~character_t();
};
#else
typedef void character_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2);
uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist);
void character_delete(character_t *c);


char getCharSymbol(character_t *ch);
int16_t getCharPositionX(character_t *ch);
int16_t getCharPositionY(character_t *ch);  
int32_t getCharSpeed(character_t *ch);
uint32_t getCharAlive(character_t *ch);
uint32_t getCharSequenceNumber(character_t *ch);
uint32_t getCharKills(character_t *ch, kill_type_t num);

void setCharPositionX(character_t *ch, int16_t pos);
void setCharPositionY(character_t *ch, int16_t pos);
void killChar(character_t *ch);
void increaseCharKills(character_t *ch, kill_type_t num);
void setCharKills(character_t *ch, kill_type_t num, uint32_t amount);
  

#ifdef __cplusplus
}
#endif
 

#endif
