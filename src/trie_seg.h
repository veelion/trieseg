/*
   Copyright (c) 2019
Author: veelion@gmail.com
All rights reserved.
*/

#ifndef SRC_TRIE_SEG_H_
#define SRC_TRIE_SEG_H_

#include <cstring>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include "base_const.h"
#include "cedarpp.h"
#include "utf8.h"


int parse_line(const char* line, std::string& word);
void make_dict(const char* word_file,
    const char* save_file);

class TrieSegger {
  public:
    typedef uint16_t trie_value_t;
    typedef cedar::da<trie_value_t> trie_t;
    const int kMatchMinimum = 1;
    const int kMatchMaximum = 2;
    const int kMatchAll = 3;

  public:
    TrieSegger();
    explicit TrieSegger(const char* dict_file);
    virtual ~TrieSegger();

    void LoadCustomeWord(const char* path);

    void set_spliter(const char* sp) {
      spliter_ = sp;
    }

    void conv2lower(const char* in);
    void _core(const char* in, std::vector<std::string> &out,
        int match_type);

    std::string seg(const char* in, int match_type);
    void seg2vec(const char* in, std::vector<std::string> &out, int match_type);
    int get_dict_value(const std::string &word);
    int prefix(const char* p, std::vector<std::string> &words);

    bool is_in_dict(const char* word);
    int dict_insert(const char* word, trie_value_t value=0);
    // ========= make dict =============
    static void make_dict(const char* word_file,
        const char* save_file);
    static std::string conv2lower(const std::string &word);

  private:
    trie_t* trie_;
    const char* spliter_ = "|";
    static const int NUM_RESULT = 32;
    const size_t WORD_MAX_LENGTH = 50;
    trie_t::result_pair_type result_pair_[NUM_RESULT];


  private:
    int get_token_len(const char* p,
        trie_t::result_pair_type *result);

    int is_jointer(const char* p);
};



#endif  // SRC_TRIE_SEG_H_
