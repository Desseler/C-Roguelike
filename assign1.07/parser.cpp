#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#include "parser.h"

using namespace std;

void parser::parse_monster_file()
{  
  string buffer, tmpbuffer, NAME, DESC, COLOR, SPEED, ABIL, HP, DAM, SYMB;
  
  chdir(getenv("HOME"));
  chdir("./.rlg327");  
  ifstream f("monster_desc.txt", ios::in);

  getline(f, buffer);
  if (buffer != "RLG327 MONSTER DESCRIPTION 1") {
    cerr << "Incorrect File Version \n";

    exit(0);
  }
  
  getline(f, buffer, '\n');

   while (f) {
    getline(f, buffer, '\n');
    
    if(buffer == "BEGIN MONSTER") {
      NAME = DESC = COLOR = SPEED = ABIL = HP = DAM = SYMB = "";
      for(;;) {
	getline(f, buffer);
	tmpbuffer = buffer.substr(0, 2);

	if (       tmpbuffer == "NA") {
	  NAME = buffer.substr(5, buffer.length() - 5);

	} else if (tmpbuffer == "DE") {
	  for(;;) {
	    getline(f, buffer);
	    if(buffer != ".") { 
	      DESC += buffer + "\n";
	    } else {
	      break;
	    }
	  }

	} else if (tmpbuffer == "CO") {
	  COLOR = buffer.substr(6, buffer.length() - 6);
	 
	} else if (tmpbuffer == "SP") {
	  SPEED = buffer.substr(6, buffer.length() - 6);
	  
	} else if (tmpbuffer == "AB") {
	  ABIL = buffer.substr(5, buffer.length() - 5);
	  
	} else if (tmpbuffer == "HP") {
	  HP = buffer.substr(3, buffer.length() - 3);
	  
	} else if (tmpbuffer == "DA") {
	  DAM = buffer.substr(4, buffer.length() - 4);
	  
	} else if (tmpbuffer == "SY") {
	  SYMB = buffer.substr(5, buffer.length() - 5);
	  
	} else if (tmpbuffer == "EN") {
	  break;
	}
      }
      cout << NAME + "\n";
      cout << DESC;
      cout << SYMB + "\n";
      cout << COLOR + "\n";
      cout << SPEED + "\n";
      cout << ABIL + "\n";
      cout << HP + "\n";
      cout << DAM + "\n\n"; 
    }
  }
}
