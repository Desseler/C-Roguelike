#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>

#include "io.h"
#include "move.h"
#include "path.h"
#include "pc.h"
#include "utils.h"
#include "dungeon.h"
#include "object.h"
#include "npc.h"

/* Same ugly hack we did in path.c */
static dungeon_t *dungeon;

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/* Possible offsets:
 *   ( 0,  0), (20,  0), (40,  0), (60,  0), (80,  0) *
 *   ( 0,  7), (20,  7), (40,  7), (60,  7), (80,  7) *
 *   ( 0, 14), (20, 14), (40, 14), (60, 14), (80, 14) *
 *   ( 0, 21), (20, 21), (40, 21), (60, 21), (80, 21) *
 *   ( 0, 28), (20, 28), (40, 28), (60, 28), (80, 28) *
 *   ( 0, 35), (20, 35), (40, 35), (60, 35), (80, 35) *
 *   ( 0, 42), (20, 42), (40, 42), (60, 42), (80, 42) *
 *   ( 0, 49), (20, 49), (40, 49), (60, 49), (80, 49) *
 *   ( 0, 56), (20, 56), (40, 56), (60, 56), (80, 56) *
 *   ( 0, 63), (20, 63), (40, 63), (60, 63), (80, 63) *
 *   ( 0, 70), (20, 70), (40, 70), (60, 70), (80, 70) *
 *   ( 0, 77), (20, 77), (40, 77), (60, 77), (80, 77) *
 *   ( 0, 84), (20, 84), (40, 84), (60, 84), (80, 84) */

void io_calculate_offset(dungeon_t *d)
{
  d->io_offset[dim_x] = ((character_get_x(d->PC) - 10) / 20) * 20;
  if (d->io_offset[dim_x] < 0) {
    d->io_offset[dim_x] = 0;
  }
  if (d->io_offset[dim_x] > 80) {
    d->io_offset[dim_x] = 80;
  }
  d->io_offset[dim_y] = ((character_get_y(d->PC) - 4) / 7) * 7;
  if (d->io_offset[dim_y] < 0) {
    d->io_offset[dim_y] = 0;
  }
  if (d->io_offset[dim_y] > 84) {
    d->io_offset[dim_y] = 84;
  }
}

void io_update_offset(dungeon_t *d)
{
  int32_t x, y;

  x = (40 + d->io_offset[dim_x]) - character_get_x(d->PC);
  y = (11 + d->io_offset[dim_y]) - character_get_y(d->PC);

  if (x >= 15 && d->io_offset[dim_x]) {
    d->io_offset[dim_x] -= 20;
  }
  if (x <= -15 && d->io_offset[dim_x] != 80) {
    d->io_offset[dim_x] += 20;
  }
  if (y >= 5 && d->io_offset[dim_y]) {
    d->io_offset[dim_y] -= 7;
  }
  if (y <= -5 && d->io_offset[dim_y] != 84) {
    d->io_offset[dim_y] += 7;
  }
}

void io_display_tunnel(dungeon_t *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      mvaddch(y + 1, x, '0' + (d->pc_tunnel[y][x] % 10));
    }
  }
  refresh();
}

void io_display_distance(dungeon_t *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      mvaddch(y + 1, x, '0' + (d->pc_distance[y][x] % 10));
    }
  }
  refresh();
}

static char hardness_to_char[] =
  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

void io_display_hardness(dungeon_t *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      /* Maximum hardness is 255.  We have 62 values to display it, but *
       * we only want one zero value, so we need to cover [1,255] with  *
       * 61 values, which gives us a divisor of 254 / 61 = 4.164.       *
       * Generally, we want to avoid floating point math, but this is   *
       * not gameplay, so we'll make an exception here to get maximal   *
       * hardness display resolution.                                   */
      mvaddch(y + 1, x, (d->hardness[y][x]                             ?
                         hardness_to_char[1 + (d->hardness[y][x] / 5)] :
                         '0'));
    }
  }
  refresh();
}

static void io_redisplay_visible_monsters(dungeon_t *d)
{
  /* This was initially supposed to only redisplay visible monsters.  After *
   * implementing that (comparitivly simple) functionality and testing, I   *
   * discovered that it resulted to dead monsters being displayed beyond    *
   * their lifetimes.  So it became necessary to implement the function for *
   * everything in the light radius.  In hindsight, it would be better to   *
   * keep a static array of the things in the light radius, generated in    *
   * io_display() and referenced here to accelerate this.  The whole point  *
   * of this is to accelerate the rendering of multi-colored monsters, and  *
   * it is *significantly* faster than that (it eliminates flickering       *
   * artifacts), but it's still significantly slower than it could be.  I   *
   * will revisit this in the future to add the acceleration matrix.        */
  pair_t pc_offset;
  pair_t pos;
  uint32_t color;
  uint32_t illuminated;

  pc_offset[dim_y] = d->PC->position[dim_y] - d->io_offset[dim_y] + 1;
  pc_offset[dim_x] = d->PC->position[dim_x] - d->io_offset[dim_x];
  

  for (pos[dim_y] = -PC_VISUAL_RANGE;
       pos[dim_y] <= PC_VISUAL_RANGE;
       pos[dim_y]++) {
    for (pos[dim_x] = -PC_VISUAL_RANGE;
         pos[dim_x] <= PC_VISUAL_RANGE;
         pos[dim_x]++) {
      if ((d->io_offset[dim_y] + pc_offset[dim_y] + pos[dim_y] < 0) ||
          (d->io_offset[dim_y] + pc_offset[dim_y] + pos[dim_y] >= DUNGEON_Y) ||
          (d->io_offset[dim_x] + pc_offset[dim_x] + pos[dim_x] < 0) ||
          (d->io_offset[dim_x] + pc_offset[dim_x] + pos[dim_x] >= DUNGEON_X)) {
        continue;
      }
      if ((illuminated = is_illuminated(d->PC,
                                        d->io_offset[dim_y] +
                                        pc_offset[dim_y]    +
                                        pos[dim_y],
                                        d->io_offset[dim_x] +
                                        pc_offset[dim_x]    +
                                        pos[dim_x]))) {
        attron(A_BOLD);
      }
      if (d->character_map[d->io_offset[dim_y] +
                           pc_offset[dim_y]    +
                           pos[dim_y]]
                          [d->io_offset[dim_x] +
                           pc_offset[dim_x]    +
                           pos[dim_x]]           &&
          can_see(d,
                  d->PC->position,
                  d->character_map[d->io_offset[dim_y] +
                                   pc_offset[dim_y] +
                                   pos[dim_y]]
                                  [d->io_offset[dim_x] +
                                   pc_offset[dim_x] +
                                   pos[dim_x]]->position,
                  1, 0)) {
        attron(COLOR_PAIR((color =
                           d->character_map[d->io_offset[dim_y] +
                                            pc_offset[dim_y] +
                                            pos[dim_y]]
                                           [d->io_offset[dim_x] +
                                            pc_offset[dim_x] +
                                            pos[dim_x]]->get_color())));
        mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                pc_offset[dim_x] + pos[dim_x],
                character_get_symbol(d->character_map[d->io_offset[dim_y] +
                                                      pc_offset[dim_y] +
                                                      pos[dim_y]]
                                                     [d->io_offset[dim_x] +
                                                      pc_offset[dim_x] +
                                                      pos[dim_x]]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[d->io_offset[dim_y] +
                           pc_offset[dim_y]    +
                           pos[dim_y]]
                          [d->io_offset[dim_x] +
                           pc_offset[dim_x]    +
                           pos[dim_x]]                           &&
                 (can_see(d,
                          d->PC->position,
                          d->objmap[d->io_offset[dim_y] +
                                    pc_offset[dim_y] +
                                    pos[dim_y]]
                                   [d->io_offset[dim_x] +
                                    pc_offset[dim_x] +
                                    pos[dim_x]]->get_position(),
                          1, 0)                                  ||
                 d->objmap[d->io_offset[dim_y] +
                                    pc_offset[dim_y] +
                                    pos[dim_y]]
                                   [d->io_offset[dim_x] +
                                    pc_offset[dim_x] +
                                    pos[dim_x]]->have_seen())) {
        attron(COLOR_PAIR(d->objmap[d->io_offset[dim_y] +
                                    pc_offset[dim_y]    +
                                    pos[dim_y]]
                                   [d->io_offset[dim_x] +
                                    pc_offset[dim_x]    +
                                    pos[dim_x]]->get_color()));
        mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                pc_offset[dim_x] + pos[dim_x],
                d->objmap[d->io_offset[dim_y] + pc_offset[dim_y] + pos[dim_y]]
                         [d->io_offset[dim_x] +
                          pc_offset[dim_x] +
                          pos[dim_x]]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[d->io_offset[dim_y] +
                                     pc_offset[dim_y]    +
                                     pos[dim_y]]
                                    [d->io_offset[dim_x] +
                                     pc_offset[dim_x]    +
                                     pos[dim_x]]->get_color()));
      } else {
        switch (pc_learned_terrain(d->PC,
                                   d->io_offset[dim_y] +
                                   pc_offset[dim_y] + pos[dim_y],
                                   d->io_offset[dim_x] + pc_offset[dim_x] +
                                   pos[dim_x])) {
        case ter_wall:
        case ter_wall_immutable:
        case ter_unknown:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '.');
          break;
        case ter_floor_hall:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '#');
          break;
        case ter_debug:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '*');
          break;
        case ter_stairs_up:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '<');
          break;
        case ter_stairs_down:
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '>');
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          mvaddch(pc_offset[dim_y] + pos[dim_y] + 1,
                  pc_offset[dim_x] + pos[dim_x], '0');
        }
      }
      attroff(A_BOLD);
    }
  }

  refresh();
}

void io_display(dungeon_t *d)
{
  pair_t pos;
  uint32_t illuminated;
  uint32_t color;

  clear();
  for (pos[dim_y] = 0; pos[dim_y] < 21; pos[dim_y]++) {
    for (pos[dim_x] = 0; pos[dim_x] < 80; pos[dim_x]++) {
      if ((illuminated = is_illuminated(d->PC,
                                        d->io_offset[dim_y] + pos[dim_y],
                                        d->io_offset[dim_x] + pos[dim_x]))) {
        attron(A_BOLD);
      }
      if (d->io_offset[dim_y] + pos[dim_y] == d->cursor[dim_y] &&
	  d->io_offset[dim_x] + pos[dim_x] == d->cursor[dim_x]) {
	if(d->cursor[dim_y] != 0) {
	  attron(A_STANDOUT);
	}
      }
      if (d->character_map[d->io_offset[dim_y] + pos[dim_y]]
                          [d->io_offset[dim_x] + pos[dim_x]] &&
          can_see(d,
                  character_get_pos(d->PC),
                  character_get_pos(d->character_map[d->io_offset[dim_y] +
                                                     pos[dim_y]]
                                                    [d->io_offset[dim_x] +
                                                     pos[dim_x]]), 1, 0)) {

        attron(COLOR_PAIR((color =
                           d->character_map[d->io_offset[dim_y] + pos[dim_y]]
                                           [d->io_offset[dim_x] +
                                            pos[dim_x]]->get_color())));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                character_get_symbol(d->character_map[d->io_offset[dim_y] +
                                                      pos[dim_y]]
                                                     [d->io_offset[dim_x] +
                                                      pos[dim_x]]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[d->io_offset[dim_y] + pos[dim_y]]
                          [d->io_offset[dim_x] + pos[dim_x]] &&
                 (d->objmap[d->io_offset[dim_y] + pos[dim_y]]
                           [d->io_offset[dim_x] + pos[dim_x]]->have_seen() ||
                  can_see(d, character_get_pos(d->PC), pos, 1, 0))) {
        attron(COLOR_PAIR(d->objmap[d->io_offset[dim_y] + pos[dim_y]]
                                   [d->io_offset[dim_x] +
                                    pos[dim_x]]->get_color()));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                d->objmap[d->io_offset[dim_y] + pos[dim_y]]
                         [d->io_offset[dim_x] + pos[dim_x]]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[d->io_offset[dim_y] + pos[dim_y]]
                                    [d->io_offset[dim_x] +
                                     pos[dim_x]]->get_color()));
      } else {
        switch (pc_learned_terrain(d->PC,
                                   d->io_offset[dim_y] + pos[dim_y],
                                   d->io_offset[dim_x] + pos[dim_x])) {
        case ter_wall:
        case ter_wall_immutable:
        case ter_unknown:
          mvaddch(pos[dim_y] + 1, pos[dim_x], ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '.');
          break;
        case ter_floor_hall:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '#');
          break;
        case ter_debug:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '*');
          break;
        case ter_stairs_up:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '<');
          break;
        case ter_stairs_down:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '>');
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          mvaddch(pos[dim_y] + 1, pos[dim_x], '0');
        }
      }
      if (illuminated) {
        attroff(A_BOLD);
      }
      if (d->io_offset[dim_y] + pos[dim_y] == d->cursor[dim_y] &&
	  d->io_offset[dim_x] + pos[dim_x] == d->cursor[dim_x]) {
	if(d->cursor[dim_y] != 0) {
	  attroff(A_STANDOUT);
	}
      }
    }
  }
  /* Print PC Health */
  mvprintw(23, 0, "PC Health: ");
  attron(COLOR_PAIR(COLOR_YELLOW));
  mvprintw(23, 11, "%i", d->PC->hp);
  attroff(COLOR_PAIR(COLOR_YELLOW));
  
  /* Print PC meter */
  uint32_t i, scaled_meter;
  
  attron(COLOR_PAIR(COLOR_BLUE));
  mvprintw(23, 18, "EX: |     |     |     |");
  if(d->PC->meter == 300) {
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(23, 18, "CA: |     |     |     |");
    attroff(COLOR_PAIR(COLOR_CYAN));
  }
  attroff(COLOR_PAIR(COLOR_BLUE));

  scaled_meter = (d->PC->meter /20);
  if(scaled_meter > 5) {
    scaled_meter++;
  }
  if(scaled_meter > 11) {
    scaled_meter++;
  }
  for(i = 0; i < scaled_meter; i++) {
    if(i == 5 || i == 11) {
      i++;
    }
    attron(COLOR_PAIR(COLOR_BLUE));
    if(d->PC->meter == 300) {
      attron(COLOR_PAIR(COLOR_CYAN));
    }
    attron(A_STANDOUT);
    mvprintw(23, 23 + i, " ");
    if(d->PC->meter == 300) {
      attroff(COLOR_PAIR(COLOR_CYAN));
    }
    attroff(A_STANDOUT);
    attroff(COLOR_PAIR(COLOR_BLUE));
  }
  
  
  mvprintw(24, 0, "PC position is (%3d,%2d); offset is (%3d,%2d).",
           character_get_x(d->PC), character_get_y(d->PC),
           d->io_offset[dim_x], d->io_offset[dim_y]);

  io_print_message_queue(0, 0);

  refresh();
}

void io_look_mode(dungeon_t *d)
{
  int32_t key;

  do {
    if ((key = getch()) == 27 /* ESC */) {
      io_calculate_offset(d);
      io_display(d);
      return;
    }
    
    switch (key) {
    case '1':
    case 'b':
    case KEY_END:
    case '2':
    case 'j':
    case KEY_DOWN:
    case '3':
    case 'n':
    case KEY_NPAGE:
      if (d->io_offset[dim_y] != 84) {
        d->io_offset[dim_y] += 7;
      }
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
    case '5':
    case ' ':
    case KEY_B2:
    case '6':
    case 'l':
    case KEY_RIGHT:
      break;
    case '7':
    case 'y':
    case KEY_HOME:
    case '8':
    case 'k':
    case KEY_UP:
    case '9':
    case 'u':
    case KEY_PPAGE:
      if (d->io_offset[dim_y]) {
        d->io_offset[dim_y] -= 7;
      }
      break;
    }
    switch (key) {
    case '1':
    case 'b':
    case KEY_END:
    case '4':
    case 'h':
    case KEY_LEFT:
    case '7':
    case 'y':
    case KEY_HOME:
      if (d->io_offset[dim_x]) {
        d->io_offset[dim_x] -= 20;
      }
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
    case '5':
    case ' ':
    case KEY_B2:
    case '8':
    case 'k':
    case KEY_UP:
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
    case '6':
    case 'l':
    case KEY_RIGHT:
    case '9':
    case 'u':
    case KEY_PPAGE:
      if (d->io_offset[dim_x] != 80) {
        d->io_offset[dim_x] += 20;
      }
      break;
    }
    io_display(d);
  } while (1);
}

void io_display_monster_list(dungeon_t *d)
{
  mvprintw(11, 33, " HP:    XXXXX ");
  mvprintw(12, 33, " Speed: XXXXX ");
  mvprintw(14, 27, " Hit any key to continue. ");
  refresh();
  getch();
}

uint32_t io_teleport_pc(dungeon_t *d)
{
  /* Just for fun. */
  pair_t dest;

  do {
    dest[dim_x] = rand_range(1, DUNGEON_X - 2);
    dest[dim_y] = rand_range(1, DUNGEON_Y - 2);
  } while (charpair(dest));

  d->character_map[character_get_y(d->PC)][character_get_x(d->PC)] = NULL;
  d->character_map[dest[dim_y]][dest[dim_x]] = d->PC;

  character_set_y(d->PC, dest[dim_y]);
  character_set_x(d->PC, dest[dim_x]);

  if (mappair(dest) < ter_floor) {
    mappair(dest) = ter_floor;
  }

  pc_observe_terrain(d->PC, d);
  io_calculate_offset(d);
  dijkstra(d);
  dijkstra_tunnel(d);

  return 0;
}
/* Adjectives to describe our monsters */
static const char *adjectives[] = {
  "A menacing ",
  "A threatening ",
  "A horrifying ",
  "An intimidating ",
  "An aggressive ",
  "A frightening ",
  "A terrifying ",
  "A terrorizing ",
  "An alarming ",
  "A frightening ",
  "A dangerous ",
  "A glowering ",
  "A glaring ",
  "A scowling ",
  "A chilling ",
  "A scary ",
  "A creepy ",
  "An eerie ",
  "A spooky ",
  "A slobbering ",
  "A drooling ",
  " A horrendous ",
  "An unnerving ",
  "A cute little ",  /* Even though they're trying to kill you, */
  "A teeny-weenie ", /* they can still be cute!                 */
  "A fuzzy ",
  "A fluffy white ",
  "A kawaii ",       /* For our otaku */
  "Hao ke ai de "    /* And for our Chinese */
  /* And there's one special case (see below) */
};

static void io_scroll_monster_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static bool is_vowel(const char c)
{
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
          c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

static void io_list_monsters_display(dungeon_t *d,
                                     character **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */
  (void) adjectives;

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d monsters:", count);
  mvprintw(4, 19, " %-40s ", s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%3s%s (%c): %2d %s by %2d %s",
             (is_vowel(character_get_name(c[i])[0]) ? "An " : "A "),
             character_get_name(c[i]),
             character_get_symbol(c[i]),
             abs(character_get_y(c[i]) - character_get_y(d->PC)),
             ((character_get_y(c[i]) - character_get_y(d->PC)) <= 0 ?
              "North" : "South"),
             abs(character_get_x(c[i]) - character_get_x(d->PC)),
             ((character_get_x(c[i]) - character_get_x(d->PC)) <= 0 ?
              "East" : "West"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_monster_list(s, count);
  }

  free(s);
}

static int compare_monster_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (character **) v1;
  const character *const *c2 = (character **) v2;

  return (dungeon->pc_distance[character_get_y(*c1)][character_get_x(*c1)] -
          dungeon->pc_distance[character_get_y(*c2)][character_get_x(*c2)]);
}

static void io_list_monsters(dungeon_t *d)
{
  character **c;
  uint32_t x, y, count;

  c = (character **) malloc(d->num_monsters * sizeof (*c));

  /* Get a linear list of monsters */
  for (count = 0, y = 1; y < DUNGEON_Y - 1; y++) {
    for (x = 1; x < DUNGEON_X - 1; x++) {
      if (d->character_map[y][x] && d->character_map[y][x] != d->PC) {
        c[count++] = d->character_map[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  dungeon = d;
  qsort(c, count, sizeof (*c), compare_monster_distance);

  /* Display it */
  io_list_monsters_display(d, c, count);
  free(c);

  /* And redraw the dungeon */
  io_display(d);
}

void io_display_ch(dungeon_t *d)
{
  mvprintw(11, 33, " HP:    %5d ", d->PC->hp);
  mvprintw(12, 33, " Speed: %5d ", d->PC->speed);
  mvprintw(14, 27, " Hit any key to continue. ");
  refresh();
  getch();
  io_display(d);
}

void io_object_to_string(object *o, char *s, uint32_t size)
{
  if (o) {
    snprintf(s, size, "%s (sp: %d, dmg: %d+%dd%d)",
             o->get_name(), o->get_speed(), o->get_damage_base(),
             o->get_damage_number(), o->get_damage_sides());
  } else {
    *s = '\0';
  }
}

uint32_t io_wear_eq(dungeon_t *d)
{
  uint32_t i, key;
  char s[61];

  for (i = 0; i < MAX_INVENTORY; i++) {
    /* We'll write 12 lines, 10 of inventory, 1 blank, and 1 prompt. *
     * We'll limit width to 60 characters, so very long object names *
     * will be truncated.  In an 80x24 terminal, this gives offsets  *
     * at 10 x and 6 y to start printing things.  Same principal in  *
     * other functions, below.                                       */
    io_object_to_string(d->PC->in[i], s, 61);
    mvprintw(i + 6, 10, " %c) %-55s ", '0' + i, s);
  }
  mvprintw(16, 10, " %-58s ", "");
  mvprintw(17, 10, " %-58s ", "Wear which item (ESC to cancel)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key < '0' || key > '9') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter 0-9 or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter 0-9 or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->in[key - '0']) {
      mvprintw(18, 10, " %-58s ", "Empty inventory slot.  Try again.");
      continue;
    }

    if (!d->PC->wear_in(key - '0')) {
      return 0;
    }

    snprintf(s, 61, "Can't wear %s.  Try again.",
             d->PC->in[key - '0']->get_name());
    mvprintw(18, 10, " %-58s ", s);
    refresh();
  }

  return 1;
}

void io_display_in(dungeon_t *d)
{
  uint32_t i;
  char s[61];

  for (i = 0; i < MAX_INVENTORY; i++) {
    io_object_to_string(d->PC->in[i], s, 61);
    mvprintw(i + 7, 10, " %c) %-55s ", '0' + i, s);
  }

  mvprintw(17, 10, " %-58s ", "");
  mvprintw(18, 10, " %-58s ", "Hit any key to continue.");

  refresh();

  getch();

  io_display(d);
}

uint32_t io_remove_eq(dungeon_t *d)
{
  uint32_t i, key;
  char s[61], t[61];

  for (i = 0; i < num_eq_slots; i++) {
    sprintf(s, "[%s]", eq_slot_name[i]);
    io_object_to_string(d->PC->eq[i], t, 61);
    mvprintw(i + 5, 10, " %c %-9s) %-45s ", 'a' + i, s, t);
  }
  mvprintw(17, 10, " %-58s ", "");
  mvprintw(18, 10, " %-58s ", "Take off which item (ESC to cancel)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key < 'a' || key > 'l') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter a-l or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter a-l or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->eq[key - 'a']) {
      mvprintw(18, 10, " %-58s ", "Empty equipment slot.  Try again.");
      continue;
    }

    if (!d->PC->remove_eq(key - 'a')) {
      return 0;
    }

    snprintf(s, 61, "Can't take off %s.  Try again.",
             d->PC->eq[key - 'a']->get_name());
    mvprintw(19, 10, " %-58s ", s);
  }

  return 1;
}

void io_display_eq(dungeon_t *d)
{
  uint32_t i;
  char s[61], t[61];

  for (i = 0; i < num_eq_slots; i++) {
    sprintf(s, "[%s]", eq_slot_name[i]);
    io_object_to_string(d->PC->eq[i], t, 61);
    mvprintw(i + 5, 10, " %c %-9s) %-45s ", 'a' + i, s, t);
  }
  mvprintw(17, 10, " %-58s ", "");
  mvprintw(18, 10, " %-58s ", "Hit any key to continue.");

  refresh();

  getch();

  io_display(d);
}

uint32_t io_drop_in(dungeon_t *d)
{
  uint32_t i, key;
  char s[61];

  for (i = 0; i < MAX_INVENTORY; i++) {
      mvprintw(i + 6, 10, " %c) %-55s ", '0' + i,
               d->PC->in[i] ? d->PC->in[i]->get_name() : "");
  }
  mvprintw(16, 10, " %-58s ", "");
  mvprintw(17, 10, " %-58s ", "Drop which item (ESC to cancel)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key < '0' || key > '9') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter 0-9 or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter 0-9 or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->in[key - '0']) {
      mvprintw(18, 10, " %-58s ", "Empty inventory slot.  Try again.");
      continue;
    }

    if (!d->PC->drop_in(d, key - '0')) {
      return 0;
    }

    snprintf(s, 61, "Can't drop %s.  Try again.",
             d->PC->in[key - '0']->get_name());
    mvprintw(18, 10, " %-58s ", s);
    refresh();
  }

  return 1;
}

static uint32_t io_display_obj_info(object *o)
{
  char s[80];
  uint32_t i, l;
  uint32_t n;

  for (i = 0; i < 79; i++) {
    s[i] = ' ';
  }
  s[79] = '\0';

  l = strlen(o->get_description());
  for (i = n = 0; i < l; i++) {
    if (o->get_description()[i] == '\n') {
      n++;
    }
  }

  for (i = 0; i < n + 4; i++) {
    mvprintw(i, 0, s);
  }

  io_object_to_string(o, s, 80);
  mvprintw(1, 0, s);
  mvprintw(3, 0, o->get_description());

  mvprintw(n + 5, 0, "Hit any key to continue.");

  refresh();
  getch();

  return 0;  
}

static uint32_t io_inspect_eq(dungeon_t *d);

static uint32_t io_inspect_in(dungeon_t *d)
{
  uint32_t i, key;
  char s[61];

  for (i = 0; i < MAX_INVENTORY; i++) {
    io_object_to_string(d->PC->in[i], s, 61);
    mvprintw(i + 6, 10, " %c) %-55s ", '0' + i,
             d->PC->in[i] ? d->PC->in[i]->get_name() : "");
  }
  mvprintw(16, 10, " %-58s ", "");
  mvprintw(17, 10, " %-58s ", "Inspect which item (ESC to cancel, '/' for equipment)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key == '/') {
      io_display(d);
      io_inspect_eq(d);
      return 1;
    }

    if (key < '0' || key > '9') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter 0-9 or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter 0-9 or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->in[key - '0']) {
      mvprintw(18, 10, " %-58s ", "Empty inventory slot.  Try again.");
      refresh();
      continue;
    }

    io_display(d);
    io_display_obj_info(d->PC->in[key - '0']);
    io_display(d);
    return 1;
  }

  return 1;
}

static uint32_t io_inspect_eq(dungeon_t *d)
{
  uint32_t i, key;
  char s[61], t[61];

  for (i = 0; i < num_eq_slots; i++) {
    sprintf(s, "[%s]", eq_slot_name[i]);
    io_object_to_string(d->PC->eq[i], t, 61);
    mvprintw(i + 5, 10, " %c %-9s) %-45s ", 'a' + i, s, t);
  }
  mvprintw(17, 10, " %-58s ", "");
  mvprintw(18, 10, " %-58s ", "Inspect which item (ESC to cancel, '/' for inventory)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key == '/') {
      io_display(d);
      io_inspect_in(d);
      return 1;
    }

    if (key < 'a' || key > 'l') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter a-l or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter a-l or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->eq[key - 'a']) {
      mvprintw(18, 10, " %-58s ", "Empty equipment slot.  Try again.");
      continue;
    }

    io_display(d);
    io_display_obj_info(d->PC->eq[key - 'a']);
    io_display(d);
    return 1;
  }

  return 1;
}

uint32_t io_expunge_in(dungeon_t *d)
{
  uint32_t i, key;
  char s[61];

  for (i = 0; i < MAX_INVENTORY; i++) {
    /* We'll write 12 lines, 10 of inventory, 1 blank, and 1 prompt. *
     * We'll limit width to 60 characters, so very long object names *
     * will be truncated.  In an 80x24 terminal, this gives offsets  *
     * at 10 x and 6 y to start printing things.                     */
      mvprintw(i + 6, 10, " %c) %-55s ", '0' + i,
               d->PC->in[i] ? d->PC->in[i]->get_name() : "");
  }
  mvprintw(16, 10, " %-58s ", "");
  mvprintw(17, 10, " %-58s ", "Destroy which item (ESC to cancel)?");
  refresh();

  while (1) {
    if ((key = getch()) == 27 /* ESC */) {
      io_display(d);
      return 1;
    }

    if (key < '0' || key > '9') {
      if (isprint(key)) {
        snprintf(s, 61, "Invalid input: '%c'.  Enter 0-9 or ESC to cancel.",
                 key);
        mvprintw(18, 10, " %-58s ", s);
      } else {
        mvprintw(18, 10, " %-58s ",
                 "Invalid input.  Enter 0-9 or ESC to cancel.");
      }
      refresh();
      continue;
    }

    if (!d->PC->in[key - '0']) {
      mvprintw(18, 10, " %-58s ", "Empty inventory slot.  Try again.");
      continue;
    }

    if (!d->PC->destroy_in(key - '0')) {
      io_display(d);

      return 1;
    }

    snprintf(s, 61, "Can't destroy %s.  Try again.",
             d->PC->in[key - '0']->get_name());
    mvprintw(18, 10, " %-58s ", s);
    refresh();
  }

  return 1;
}

void io_mon_to_string(character *o, char *s, uint32_t size)
{
  if (o) {
    snprintf(s, size, "%s (sp: %d, dmg: %d+%dd%d)",
	     o->get_name(), o->get_speed(), o->get_damage_base(),
	     o->get_damage_number(), o->get_damage_sides());
  } else {
    *s = '\0';
  }
}

static uint32_t io_display_mon_info(character *o)
{
  char s[80];
  uint32_t i, l;
  uint32_t n;
  npc *mon;
  mon = (npc *) o;
  
  for (i = 0; i < 79; i++) {
    s[i] = ' ';
  }
  s[79] = '\0';

  l = strlen(mon->get_description());
  for (i = n = 0; i < l; i++) {
    if (mon->get_description()[i] == '\n') {
      n++;
    }
  }

  for (i = 0; i < n + 4; i++) {
    mvprintw(i, 0, s);
  }

  io_mon_to_string(o, s, 80);
  mvprintw(1, 0, s);
  mvprintw(3, 0, mon->get_description());

  mvprintw(n + 5, 0, "Hit any key to continue.");

  refresh();
  getch();

  return 0;
}

uint32_t io_select_tile(dungeon_t*d)
{
  int32_t key;
  pair_t cursor_vision;
  uint32_t fail_code;

  
  do {
    if ((key = getch()) == 27 /* ESC */) {
      d->cursor[dim_y] = 0;
      d->cursor[dim_x] = 0;
      io_calculate_offset(d);
      io_display(d);
      return 1;
    }

    cursor_vision[dim_y] = d->cursor[dim_y];
    cursor_vision[dim_x] = d->cursor[dim_x];
    
    switch (key) {
    case '7':
    case 'y':
    case KEY_HOME:
      cursor_vision[dim_y]--;
      cursor_vision[dim_x]--;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]--;
	d->cursor[dim_x]--;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '8':
    case 'k':
    case KEY_UP:
      cursor_vision[dim_y]--;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]--;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      cursor_vision[dim_y]--;
      cursor_vision[dim_x]++;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]--;
	d->cursor[dim_x]++;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      cursor_vision[dim_x]++;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_x]++;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      cursor_vision[dim_y]++;
      cursor_vision[dim_x]++;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]++;
	d->cursor[dim_x]++;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      cursor_vision[dim_y]++;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]++;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '1':
    case 'b':
    case KEY_END:
      cursor_vision[dim_y]++;
      cursor_vision[dim_x]--;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_y]++;
	d->cursor[dim_x]--;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      cursor_vision[dim_x]--;
      if (can_see(d, d->PC->position, cursor_vision, 1, 0)) {
	d->cursor[dim_x]--;
      } else {
	io_queue_message("Can't select outside PC line of sight.");
      }
      fail_code = 1;
      break;
    case 'r':
      if(charpair(d->cursor) && charpair(d->cursor) != d->PC) {
	if (d->PC->eq[eq_slot_ranged]) {
	  do_ranged_combat(d, d->PC, charpair(d->cursor));
	  fail_code = 0;
	} else {
	  io_queue_message("You have no ranged weapon equipped.");
	  fail_code = 1;
	}
      } else {
	io_queue_message("No monster to attack.");
	fail_code = 1;
      }
      break;
    case 'z':
      if(charpair(d->cursor) && charpair(d->cursor) != d->PC) {
	do_special_combat(d, d->PC, charpair(d->cursor), 1);
	fail_code = 0;
      } else {
	io_queue_message("No monster to attack.");
	fail_code = 1;
      }
      break;
    case 'i':
    case 'I':
      if(charpair(d->cursor) && charpair(d->cursor) != d->PC) {
	io_display_mon_info(charpair(d->cursor));
      } else {
	io_queue_message("No monster to inspect.");
      }
      fail_code = 1;
      break;
    case 'x':	
      if(charpair(d->cursor) && charpair(d->cursor) != d->PC) {
	if(d->PC->meter >= 100) { 
	  do_special_combat(d, d->PC, charpair(d->cursor), 2);
	} else {
	  do_special_combat(d, d->PC, charpair(d->cursor), 1);
	}
	fail_code = 0;
      } else {
	io_queue_message("No monster to attack.");
	fail_code = 1;
      }
      break;
    case 'c':
      if(charpair(d->cursor) && charpair(d->cursor) != d->PC) {
	if(d->PC->meter == 300) {
	  uint32_t i;
	  pair_t order[25] = {
	    {  0,  0 },
	    { -1,  0 },
	    { -1,  1 },
	    {  0, -1 },
	    { -1, -1 },
	    {  0,  1 },
	    {  1, -1 },
	    {  1,  0 },
	    {  1,  1 },
	    { -2, -2 },
	    { -2, -1 },
	    { -2,  0 },
	    { -2,  1 },
	    { -2,  2 },
	    { -1,  2 },
	    {  0,  2 },
	    {  1,  2 },
	    {  2,  2 },
	    {  2,  1 },
	    {  2,  0 },
	    {  2, -1 },
	    {  2, -2 },
	    {  1, -2 },
	    {  0, -2 },
	    { -1, -2 },
	  };
	  for (i = 0; i < 25; i++) {
	    if(i == 0) {
	      do_special_combat(d, d->PC, charpair(d->cursor), 3);
	    }
	    if(i > 0 && i < 9) {
	      if(charxy(d->cursor[dim_x] + order[i][dim_x],
			d->cursor[dim_y] + order[i][dim_y]) &&
		 charxy(d->cursor[dim_x] + order[i][dim_x],
			d->cursor[dim_y] + order[i][dim_y]) != d->PC) {
		do_special_combat(d, d->PC, charxy(d->cursor[dim_x] + order[i][dim_x],
						   d->cursor[dim_y] + order[i][dim_y]), 4);
	      }
	    }
	    if(i > 9) {
	      if(charxy(d->cursor[dim_x] + order[i][dim_x],
			d->cursor[dim_y] + order[i][dim_y]) &&
		 charxy(d->cursor[dim_x] + order[i][dim_x],
			d->cursor[dim_y] + order[i][dim_y]) != d->PC) {
		do_special_combat(d, d->PC, charxy(d->cursor[dim_x] + order[i][dim_x],
						   d->cursor[dim_y] + order[i][dim_y]), 5);
	      }
	    }
	  }
	} else {
	  do_special_combat(d, d->PC, charpair(d->cursor), 1);
	}
	fail_code = 0;
      } else {
	io_queue_message("No monster to attack.");
	fail_code = 1;
      }
      break;
    default:
      fail_code = 1;
      break;
    }
    io_display(d);
  } while (fail_code);
  d->cursor[dim_y] = 0;
  d->cursor[dim_x] = 0;
  
  return 0;
}

void io_handle_input(dungeon_t *d)
{
  uint32_t fail_code;
  int key;
  fd_set readfs;
  struct timeval tv;

  do {
    do{
      FD_ZERO(&readfs);
      FD_SET(STDIN_FILENO, &readfs);

      tv.tv_sec = 0;
      tv.tv_usec = 125000; /* An eigth of a second */

      io_redisplay_visible_monsters(d);
    } while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));

    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      fail_code = move_pc(d, 7);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      fail_code = move_pc(d, 8);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      fail_code = move_pc(d, 9);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      fail_code = move_pc(d, 6);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      fail_code = move_pc(d, 3);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      fail_code = move_pc(d, 2);
      break;
    case '1':
    case 'b':
    case KEY_END:
      fail_code = move_pc(d, 1);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      fail_code = move_pc(d, 4);
      break;
    case '5':
    case ' ':
    case KEY_B2:
      fail_code = 0;
      break;
    case '>':
      fail_code = move_pc(d, '>');
      break;
    case '<':
      fail_code = move_pc(d, '<');
      break;
    case 'Q':
      d->quit = 1;
      fail_code = 0;
      break;
#if 0
    case 'T':
      /* New command.  Display the distances for tunnelers.             */
      io_display_tunnel(d);
      fail_code = 1;
      break;
    case 'D':
      /* New command.  Display the distances for non-tunnelers.         */
      io_display_distance(d);
      fail_code = 1;
      break;
    case 'H':
      /* New command.  Display the hardnesses.                          */
      io_display_hardness(d);
      fail_code = 1;
      break;
    case 's':
      /* New command.  Return to normal display after displaying some   *
       * special screen.                                                */
      io_display(d);
      fail_code = 1;
      break;
#endif
    case 'L':
      io_look_mode(d);
      fail_code = 1;
      break;
    case 'r':
      d->cursor[dim_y] = d->PC->position[dim_y];
      d->cursor[dim_x] = d->PC->position[dim_x];
      fail_code = io_select_tile(d);
      break;
    case 'g':
      /* Teleport the PC to a random place in the dungeon.              */
      io_teleport_pc(d);
      fail_code = 0;
      break;
    case 'm':
      io_list_monsters(d);
      fail_code = 1;
      break;
      break;
    case 'w':
      fail_code = io_wear_eq(d);
      break;
    case 't':
      fail_code = io_remove_eq(d);
      break;
    case 'd':
      fail_code = io_drop_in(d);
      break;
    case 'x':
      fail_code = io_expunge_in(d);
      break;
    case 'i':
      io_display_in(d);
      fail_code = 1;
      break;
    case 'e':
      io_display_eq(d);
      fail_code = 1;
      break;
    case 'c':
      io_display_ch(d);
      fail_code = 1;
      break;
    case 'I':
      io_inspect_in(d);
      fail_code = 1;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matterrs, but using this command will  *
       * waste a turn.  Set fail_code to 1 and you should be able to *
       * figure out why I did it that way.                           */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      fail_code = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      fail_code = 1;
    }
  } while (fail_code);
}
