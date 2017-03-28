#ifndef PARSER_H
# define PARSER_H

# include <stdint.h>
# include <iostream>
# include <ifstream>

class parser {
  ifstream f;
 public:
  void open_file(string filename);
  
}

#endif
