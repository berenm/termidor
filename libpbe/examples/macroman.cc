#include "macroman.hh"

#include <iostream>

using namespace std;
using namespace pbe;


int main()
{
  string s;
  getline(cin,s);

  cout << utf8_to_macroman(s) << "\n";

  return 0;
}

