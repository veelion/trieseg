/*
   Copyright (c) 2019
Author: veelion@gmail.com
All rights reserved.
*/

#include "trie_seg.h"

#include <cstring>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

using namespace std;


TrieSegger::TrieSegger() {
  cout << CLI_RED << "Don't forget to load words" << CLI_NOR << endl;
  trie_ = new trie_t;
}

TrieSegger::TrieSegger(const char* dict_file) {
  trie_ = new trie_t;
  cout << "open new dict: " << dict_file << endl;
  if (trie_->open(dict_file)) {
    delete trie_;
    trie_ = NULL;
    cout << CLI_RED << "Cannot open trie file: " << dict_file << CLI_NOR << endl;
    cout << CLI_RED << "Is it really there?!!!" << CLI_NOR << endl;
    throw runtime_error(string("failed to open dictionary: ") + dict_file);
  }
  std::fprintf (stderr, "#keys: %ld\n", trie_->num_keys ());
  std::fprintf (stderr, "size: %ld\n", trie_->size ());
  std::fprintf (stderr, "nonzero_size: %ld\n", trie_->nonzero_size ());
}

TrieSegger::~TrieSegger() {
  if(trie_) {
    delete trie_;
    trie_ = NULL;
  }
}

void TrieSegger::LoadCustomeWord(const char* path) {
  ifstream ifs(path);
  if (!ifs) {
    cout << CLI_RED << "Can't open word file: " << path << CLI_NOR << endl;
    return;
  }
  string line;
  int no_value_cnt = 0;
  string word;
  while (!ifs.eof()){
    getline(ifs, line);
    if(line[0] == '#') {
      cout << "comment:" << line << endl;
      continue;
    }
    int n = parse_line(line.c_str(), word);
    if (n == 0) {
      no_value_cnt++;
      if(no_value_cnt < 10){
        cout << "no word's value line: " << line << endl;
        cout << "\t" << word << ", " << n << endl;
      }
    }
    if (word.size() < 3 || word.size() > WORD_MAX_LENGTH) {
      cout << "=== too short or too long word :" << word << " : " << word.size() << endl;
    } else {
      n = sqrt(n);
      if (n > 65535) n = 65535;
      trie_->update(word.c_str(), word.size(), n);
    }
  }
  ifs.close();
  //
  std::fprintf (stderr, "#keys: %ld\n", trie_->num_keys ());
  std::fprintf (stderr, "size: %ld\n", trie_->size ());
  std::fprintf (stderr, "nonzero_size: %ld\n", trie_->nonzero_size ());
}


int TrieSegger::dict_insert(const char* word, trie_value_t value) {
  if (!word) {
    return -1;
  }
  trie_->update(word, strlen(word), value);
  return 0;
}

int TrieSegger::get_token_len(const char* p,
    trie_t::result_pair_type *result) {
  int count = trie_->commonPrefixSearch(p, result, NUM_RESULT, min(WORD_MAX_LENGTH, strlen(p)));
  int len = 0;
  if(count) {
    len = result[count-1].length;
  }else{
    len = u8_seqlen(p);
  }
  return len;
}

int TrieSegger::prefix(const char*p, vector<string> &words) {
  // find words with prefix
  // trie_t::result_pair_type result[NUM_RESULT];
  int count = trie_->commonPrefixSearch(p, result_pair_, NUM_RESULT, min(WORD_MAX_LENGTH, strlen(p)));
  if(count) {
    if (count > NUM_RESULT) {
      cout << CLI_RED << "NUM_RESULT is small for: " << string(p, 30) << endl;
    }
    words.clear();
    for(int i = 0; i < count; i++) {
      string w = string(p, result_pair_[i].length);
      words.push_back(w);
    }
  }else{
    // not find
  }
  return count;
}

int TrieSegger::get_dict_value(const string &word) {
  size_t n = trie_->commonPrefixSearch(
      word.c_str(), result_pair_, NUM_RESULT, word.size());
  int v = 0;
  for(size_t i = 0; i < n; i++) {
    if(result_pair_[i].length == word.size()){
      v = result_pair_[i].value;
      break;
    }
  }
  // if(v < 1) v = 1;
  return v;
}

bool TrieSegger::is_in_dict(const char* word){
  size_t n = trie_->commonPrefixSearch(
      word, result_pair_, NUM_RESULT);
  bool in = false;
  if(n && result_pair_[n-1].length == strlen(word)) {
    in = true;
  }
  return in;
}

int TrieSegger::is_jointer(const char* p) {
  // return length of jointer, otherwise return 0;
  if(*p == '+' ||
      *p == '_' ||
      //*p == '.' ||
      *p == '-' ||
      *p == '%') return 1;
  if(strncmp(p, "·", 2) == 0) return 2;
  return 0;
}

void TrieSegger::_core(const char* in, vector<string> &out, int match_type) {
  /* match_type:
   * 1 : minimum
   * 2 : maximum
   * 3 : all
   */
  const char* token_begin;
  const char* p = in;
  int len = 0;
  int count = 0;
  while(*p) {
    token_begin = p;
    count = trie_->commonPrefixSearch(p, result_pair_, NUM_RESULT, min(WORD_MAX_LENGTH, strlen(p)));
    if(!count){
      len = u8_seqlen(token_begin);
      if(len == 0) break;
      if(len > 1) {
        out.push_back(string(token_begin, len));
        p += len;
        continue;
      }
      // alpha, number, english punctuation
      count = 0;
      while(*p) {
        if(isalnum(*p)) {
          p++;
          count++;
          continue;
        }
        int c = is_jointer(p);
        if(c) {
          p += c;
          count += c;
          continue;
        }
        break;
      }
      if(count > 0){
        out.push_back(string(token_begin, count));
      }else{
        out.push_back(string(token_begin, 1));
        p++;
      }
      continue;
    }else{
      if (count > NUM_RESULT) {
        cout << CLI_RED << "NUM_RESULT is small for: " << string(p, 30) << endl;
      }
      if (kMatchMinimum == match_type) {
        // maximum match
        len = result_pair_[0].length;
        char end = *(token_begin + len - 1);
        char next = *(token_begin + len);
        while(isalpha(end) && isalpha(next)) {
          len++;
          next = *(token_begin + len);
        }
        out.push_back(string(token_begin, len));
        p += len;
        continue;
      }
      if(kMatchMaximum == match_type) {
        int idx = count - 1;
        if(count > NUM_RESULT) {
          idx = NUM_RESULT - 1;
        }
        len = result_pair_[idx].length;
        char end = *(token_begin + len - 1);
        char next = *(token_begin + len);
        while(isalpha(end) && isalpha(next)) {
          len++;
          next = *(token_begin + len);
        }
        out.push_back(string(token_begin, len));
        p += len;
        continue;
      }
      // output all matched words
      for(int i = 0; i < count; i++) {
        len = result_pair_[i].length;
        char end = *(token_begin + len - 1);
        char next = *(token_begin + len);
        while(isalpha(end) && isalpha(next)) {
          len++;
          next = *(token_begin + len);
        }
        out.push_back(string(token_begin, len));
      }
      p += result_pair_[0].length;
    }
  }
}

string TrieSegger::seg(const char* in, int match_type){
  vector<string> out;
  string sentence;

  seg2vec(in, out, match_type);
  // generate word string
  string os = "";
  if (!spliter_) spliter_="|";
  for (auto& word : out) {
    os += word + spliter_;
  }

  //int s = strlen(_spliter);
  //os.erase(os.size()-s);
  return os;
}

void TrieSegger::seg2vec(const char* in, vector<string> &out, int match_type) {
  _core(in, out, match_type);
}




// =========== create dict ==================


void TrieSegger::make_dict(const char* word_file,
    const char* save_file) {
  make_dict(word_file, save_file);
}

// global functions
/* returns length of next utf-8 sequence */


int parse_line(const char* line, std::string& word) {
  // strip space, \n, \t at both end
  int n = 1;
  const char* p = line;
  const char* word_begin = NULL;
  const char* digit_begin = NULL;
  // skip space of line head
  while(*p == ' ' || *p == '\t' ) {
      p++;
  }
  word_begin = p;
  while(*p != ' ' && *p != '\t' && *p != '\n') {
      p++;
  }
  word = string(word_begin, p-word_begin);
  while(*p == ' ' || *p == '\t' ) {
      p++;
  }
  if (isdigit(*p)) {
    digit_begin = p;
    p++;
    while(isdigit(*p)) p++;
    //p[0] = '\0';
    n = atol(digit_begin);
  } else {
    n = 0;
  }
  return n;
}

void make_dict(const char* word_file,
    const char* save_file) {
  TrieSegger::trie_value_t n = 1;
  TrieSegger::trie_t dict;
  ifstream ifs(word_file);
  int no_value_cnt = 0;
  string line, word;
  while (!ifs.eof()){
    if(line[0] == '#') {
      cout << "comment: " << line << endl;
      continue;
    }
    n = parse_line(line.c_str(), word);
    n = sqrt(n);
    if (n > 65535) n = 65535;
    if (n == 0) no_value_cnt++;
    if (no_value_cnt < 10){
      no_value_cnt++;
      cout << "no word's value line: " <<  line << endl;
    }
    if (word.size() > 0) {
      dict.update(word.c_str(), word.size(), n);
    }
  }
  ifs.close();
  if (dict.save(save_file) != 0){
    std::fprintf (stderr, "cannot save trie: %s\n", save_file);
    std::exit (1);
  }
  //
  std::fprintf (stderr, "#keys: %ld\n", dict.num_keys ());
  std::fprintf (stderr, "size: %ld\n", dict.size ());
  std::fprintf (stderr, "nonzero_size: %ld\n", dict.nonzero_size ());
}

