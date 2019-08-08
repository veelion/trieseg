/*
   Copyright (c) 2019
Author: veelion@gmail.com
All rights reserved.
*/

#ifndef SRC_WORDFINDER_H_
#define SRC_WORDFINDER_H_


#include "cedarpp.h"
#include "trie_seg.h"


struct NewWord {
  NewWord(std::string _word, int _frequency, float _jointy, float _entropy)
      : word(_word), frequency(_frequency), jointy(_jointy), entropy(_entropy) {}
  std::string word;
  int frequency;
  float jointy;
  float entropy;
};


class WordFinder {
  public:
    typedef uint32_t trie_value_t;
    typedef cedar::da<trie_value_t> trie_t;

    explicit WordFinder(int ngram);
    virtual ~WordFinder();

    void LoadSeggerDict(const char* path);
    void Feed(const char* doc);
    // jointy: 凝固度
    void Find(int min_frequency, int min_jointy,
        int min_entropy, std::vector<NewWord>& newwords);

    void Find(int min_frequency, int min_jointy,
        int min_entropy, const char* save_to_path);

  private:
    int ngram_;
    trie_value_t totalcount_;
    TrieSegger * segger_;
    trie_t* trie_forword_;  // normal order string: ab中
    trie_t* trie_reverse_;  // reversed order string: 中ba
    static const int NUM_RESULT = 512;
    trie_t::result_triple_type triple_[NUM_RESULT];



    int Ngram(const char* sentence, int maxN, std::map<std::string, int> &tokens);
    uint32_t NextKey(std::string &key);  // traverse trie_forword_
    float CalculateJointy(const std::string& token, int frequency);
    float CalculateEntropy(const std::string& token);
    float CalculateEntropy(const std::map<std::string, int>& tf);
    void GenParts(
        const std::vector<std::string>& words,\
        std::vector<std::pair<std::string, std::string> >& parts);

};

#endif  // SRC_WORDFINDER_H_
