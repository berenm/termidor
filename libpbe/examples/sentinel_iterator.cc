#include "sentinel_iterator.hh"

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace pbe;


int main()
{
  const char* raw = "now is the time for all good men to come to the aid of the party";

  typedef const_sentinel_iterator<const char*,'\0'> cstr_iter;

  cstr_iter s(raw);
  cstr_iter e;

  string a(s,e);
  cout << "a='" << a << "'\n";

  cstr_iter y = find(s,e,'y');
  string b(s,y);
  cout << "b='" << b << "'\n";

  cstr_iter z = find(s,e,'z');
  string c(s,z);
  cout << "c='" << c << "'\n";

  return 0;
}

