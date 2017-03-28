#ifndef PARSER_H
# define PARSER_H

# include <iostream>
# include <ifstream>
# include <string>

class parser {
 private:
  ifstream f;
 public:
  void parse_monster_file(string filename);
}

#endif
