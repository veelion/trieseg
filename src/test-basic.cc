
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "utf8.h"
#include "utility.h"
#include "cedarpp.h"
#include "trie_seg.h"

using namespace std;

int main(int argc, char* argv[]) {
  // test cedar
  cedar::da<int> trie;
  vector<string> kk = {
    "a",
    "ab",
    "abc",
    "abd",
    "ad",
    "ae",
  };
  for (auto k : kk) {
    trie.update(k.c_str(), k.size(), 1);
  }
  for (auto k : kk) {
    int v = trie.exactMatchSearch<int>(k.c_str(), k.size());
    cout << k << " : " << v << endl;
  }
  cedar::da<int>::result_triple_type triple[32];
  int count = trie.commonPrefixPredict("ab", triple, 32, 2);
  char buf[32] ;
  for (int i = 0; i < count; i++) {
    trie.suffix(buf, triple[i].length, triple[i].id);
    cout << buf << ", " << triple[i].length << endl;
  }

  vector<string> chars = {
    "A",
    "中"
  };
  int i = 0;
  for (auto c : chars) {
    unsigned int uc = utf8_decode(c.c_str(), &i);
    cout << c << " -> " << uc << ", length: " << i << endl;
  }

  vector<string> sentences;
  vector<string> docs = {
    "  1234abc,   分句子 .\n",
    ",,,刻录机。？～！“《》jk90g水电费。123abcd",
  };

  for (auto doc : docs) {
    int c = split_sentence(doc.c_str(), sentences);
    cout << doc << " -> " << endl;
    for (auto c : sentences) {
      cout << c << ", length: " << c.size() << endl;
    }
    cout << "===========" << endl;
  }

  vector<string> ss = {
    "abc",
    "a12",
    "字符abc",
  };
  for (auto s : ss) {
    string r = ucs_reverse(s);
    cout << s << " -> " << r << endl;
  }

  TrieSegger segger = TrieSegger();
  segger.LoadCustomeWord("./dict.txt");

  vector<string> words;
  const char* zz = "人关注了该圆桌";
  segger.seg2vec(zz, words, 2);
  cout << zz << " ==> " << endl;
  for (auto w : words) {
    cout << "\t" << w << ": " << w.size() << endl;
  }


  return 0;
}
