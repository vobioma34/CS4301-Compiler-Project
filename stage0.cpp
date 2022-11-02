//Esai Barron and Victor Obioma
//CS 4301
//Stage 0

#ifndef STAGE0_H
#define STAGE0_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

const char END_OF_FILE = '$'; // arbitrary choice

enum storeTypes {INTEGER, BOOLEAN, PROG_NAME};
enum modes {VARIABLE, CONSTANT};
enum allocation {YES, NO};

class SymbolTableEntry 
{
public:
  // Public functions
  SymbolTableEntry(string in, storeTypes st, modes m,
                   string v, allocation a, int u)
  {
    setInternalName(in);
    setDataType(st);
    setValue(v);
    setAlloc(a);
    setUnits(u);
  }
  string getInternalName() const {
    return internalName;
  }
  storeTypes getDataType() const {
    return dataType;
  }
  modes getMode() const {
    return mode;
  }
  string getValue() const {
    return value;
  }
  allocation getAlloc() const {
    return alloc;
  }
  int getUnits() const {
    return units;
  }
  void setInternalName(string s) {
    internalName = s;
  }
  void setDataType(storeTypes st) {
    dataType = st;
  }
  void setMode(modes m) {
    mode = m;
  }
  void setValue(string s) {
    value = s;
  }
  void setAlloc(allocation a) {
    alloc = a;
  }
  void setUnits(int i) {
    units = i;
  }
  // End of the public field
private:
  // Private members
  string internalName;
  storeTypes dataType;
  modes mode;
  string value;
  allocation alloc;
  int units;
  // End of the private field
};
#endif
// Editor: Victor Obioma
// edited 11/1/2022 1:26pm
// Next is the creation of the Compiler class below...
