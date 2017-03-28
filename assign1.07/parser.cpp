#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#include "parser.h"

using namespace std;

void parser::parse_monster_file()
{
  chdir(getenv("HOME"));
  chdir("./.rlg327");
  
  ifstream f("monster_desc.txt", ios::in);

  string buffer;
  getline(f, buffer);
  if (buffer != "RLG327 MONSTER DESCRIPTION 1") {
    cerr << "Incorrect File Version";

    exit(0);
  }
  
  string NAME, DESC, COLOR, SPEED, ABIL, HP, DAM, SYMB;
  //getline(f, buffer);
  
  while (getline(f, buffer)) {
    NAME = DESC = COLOR = SPEED = ABIL = HP = DAM = SYMB = "";

    getline(f, buffer);
    if (buffer == "BEGIN MONSTER") {
      getline(f, buffer, ' ');
      while (buffer != "END") {
	
	if (       buffer == "NAME") {
	  getline(f, NAME, '\n');
	  
	} else if (buffer == "DESC") {
	  getline(f, buffer, '\n');
	  getline(f, buffer);
	  while (buffer != ".") {
	    DESC += buffer + "\n";
	    getline(f, buffer);
	  }

	} else if (buffer == "COLOR") {
	  getline(f, COLOR, '\n');

	} else if (buffer == "SPEED") {
	  getline(f, SPEED, '\n');

	} else if (buffer == "ABIL") {
	  getline(f, ABIL, '\n');
	  
	} else if (buffer == "HP") {
	  getline(f, HP, '\n');

	} else if (buffer == "DAM") {
	  getline(f, DAM, '\n');
	  
	} else if (buffer == "SYMB") {
	  getline(f, SYMB, '\n');
	  
	}
      }

      cout << NAME;
      cout << DESC;
      cout << SYMB;
      cout << COLOR;
      cout << SPEED;
      cout << ABIL;
      cout << HP;
      cout << DAM;

      //getline(f, buffer);
    }
  }
}
