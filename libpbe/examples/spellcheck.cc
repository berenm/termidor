#include <fstream>
#include <iostream>
#include <string>
#include <tr1/unordered_set>

#include "bloom_filter.hh"


using namespace std;


static size_t hash1(string key, int k) {
  string s(1,k);
  s += key;
  return std::tr1::hash<string>()(s);
}

struct hash_t {
  typedef std::tr1::array<size_t,9> hashes_t;
  hashes_t operator()(string key) {
    hashes_t h;
    for (int i=0; i<9; ++i) {
      h[i] = hash1(key,i);
    }
    return h;
  }
};


int main()
{

  typedef bloom_filter< std::string, 512*1024, 9, hash_t > spellcheck_t;

  spellcheck_t spellcheck;

  ifstream words("/usr/share/dict/words");
  while (!words.fail()) {
    string w;
    getline(words,w);
    spellcheck.insert(w);
  }

  while (1) {
    cout << "word: " << flush;
    string w;
    getline(cin,w);
    if (spellcheck.check(w)) {
      cout << "OK\n";
    } else {
      cout << "Wrong\n";
    }
  }
}

