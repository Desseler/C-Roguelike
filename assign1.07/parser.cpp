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
  bool NameB, DescB, ColorB, SpeedB, AbilB, HpB, DamB, SymbB;
  
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
      NameB = DescB = ColorB = SpeedB = AbilB = HpB = DamB = SymbB = false;
      for(;;) {
	getline(f, buffer);
	tmpbuffer = buffer.substr(0, 2);

	if (       tmpbuffer == "NA") {
	  NAME = buffer.substr(5, buffer.length() - 5);
	  NameB = true;

	} else if (tmpbuffer == "DE") {
	  DescB = true;
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
	  ColorB = true;
	  
	} else if (tmpbuffer == "SP") {
	  SPEED = buffer.substr(6, buffer.length() - 6);
	  SpeedB = true;
	  
	} else if (tmpbuffer == "AB") {
	  ABIL = buffer.substr(5, buffer.length() - 5);
	  AbilB = true;
	  
	} else if (tmpbuffer == "HP") {
	  HP = buffer.substr(3, buffer.length() - 3);
	  HpB = true;
	  
	} else if (tmpbuffer == "DA") {
	  DAM = buffer.substr(4, buffer.length() - 4);
	  DamB = true;
	  
	} else if (tmpbuffer == "SY") {
	  SYMB = buffer.substr(5, buffer.length() - 5);
	  SymbB = true;
	  
	} else if (tmpbuffer == "EN") {
	  break;
	}
      }
      if(NameB && DescB && SymbB && ColorB && SpeedB && AbilB && HpB && DamB) {	
	cout << NAME + "\n";
	cout << DESC;
	cout << SYMB + "\n";
	cout << COLOR + "\n";
	cout << SPEED + "\n";
	cout << ABIL + "\n";
	cout << HP + "\n";
	cout << DAM + "\n\n";
      } else {
	cout << "**ERROR: Monster description not complete. Skipping Monster.**\n\n";
      }
    }
  }
}
