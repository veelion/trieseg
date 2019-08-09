/*
   Copyright (c) 2019
Author: veelion@gmail.com
All rights reserved.
*/

#include <math.h>
#include <iostream>
#include <fstream>
#include <map>
#include "utility.h"
#include "wordfinder.h"

using namespace std;

WordFinder::WordFinder(int ngram)
    : ngram_(ngram),
      totalcount_(0) {
  segger_ = new TrieSegger();
  trie_forword_ = new trie_t;
  trie_reverse_ = new trie_t;
}


WordFinder::~WordFinder() {
  delete segger_;
  delete trie_forword_;
  delete trie_reverse_;
}


void WordFinder::LoadSeggerDict(const char* path) {
  segger_->LoadCustomeWord(path);
}


int WordFinder::Ngram(const char* sentence, int maxN, map<string, int> &tokens) {
  vector<string> words;
  segger_->seg2vec(sentence, words, segger_->kMatchMaximum);
  tokens.clear();

  int tokens_count = 0;
  for (size_t i=0; i<words.size(); i++) {
    string token = words[i];
    map_counter(tokens, token);
    for (int j=1; j<maxN; j++) {
      if (i+j >= words.size()) break;
      token += words[i+j];
      map_counter(tokens, token);
      tokens_count += 1;
    }
  }
  return tokens_count;
}

void WordFinder::Feed(const char* doc) {
  vector<string> sentences;
  split_sentence(doc, sentences);
  map<string, int> tokens;
  for (auto& sentence : sentences) {
    int total = Ngram(sentence.c_str(), ngram_, tokens);
    totalcount_ += total;
    for (auto& t : tokens) {
      trie_forword_->update(t.first.c_str(), t.first.size(), t.second);
      string reversed = ucs_reverse(t.first);
      trie_reverse_->update(reversed.c_str(), reversed.size(), t.second);
    }
  }
}

// to iterate all keys
uint32_t WordFinder::NextKey(string& key) {
  static size_t from = 0, p = 0;
  union { int i; int x; } b;
  char buf[256] = {0};
  key.clear();
  if(from == 0) {
    b.i = trie_forword_->begin(from, p);
  }else{
    b.i = trie_forword_->next(from, p);
  }
  if (b.i == trie_t::CEDAR_NO_PATH) {
    key = "";
    from = 0;
    p = 0;
    return 0;
  }
  trie_forword_->suffix(buf, p, from);
  if (p > 255) {
    std::cout << "too long p: " << p << std::endl;
  }
  buf[255] = 0;
  key = buf;
  // b.x is the value
  return b.x;
}

void WordFinder::GenParts(const vector<string> &words, vector<pair<string, string> >& parts) {
  for(size_t i=1; i < words.size(); i++) {
    string a, b;
    for (size_t j = 0; j < i; j++) {
      a += words[j];
    }
    for (size_t j = i; j < words.size(); j++) {
      b += words[j];
    }
    parts.push_back(pair<string, string>(a, b));
  }
}

float WordFinder::CalculateJointy(const string& token, int frequency) {
  vector<string> words;
  segger_->seg2vec(token.c_str(), words, segger_->kMatchMaximum);
  vector<pair<string, string> > parts;
  GenParts(words, parts);
  int count_a, count_b;
  float jointy = 1000.0;
  for (auto& p : parts) {
    // cout << "token: " << token << ", part: " << p.first << " " << p.second << endl;
    count_a = trie_forword_->exactMatchSearch<trie_value_t>(p.first.c_str(), p.first.size());
    if (count_a == trie_t::CEDAR_NO_VALUE) {
      // cout << "no value for " << p.first << endl;
      count_a = 1;
    }
    count_b = trie_forword_->exactMatchSearch<trie_value_t>(p.second.c_str(), p.second.size());
    if (count_b == trie_t::CEDAR_NO_VALUE) {
      // cout << "no value for " << p.second << endl;
      count_b = 1;
    }
    // cout << "count_a: " << count_a << ", count_b: " << count_b << endl;
    //float j = (frequency / totalcount_) / (count_a/totalcount_ * count_b/totalcount_);
    float j = float(frequency) / count_a * (float(totalcount_) / count_b);
    if (j < jointy) jointy = j;
  }
  return jointy;
}


float WordFinder::CalculateEntropy(const map<string, int>& tf) {
  float sum = 0.0;
  for (auto& x : tf) {
    //cout << "\t" << x.first << " " << x.second << endl;
    sum += x.second;
  }
  float entropy = 0.0;
  for (auto& x : tf) {
    float p = x.second / sum;
    //cout << "\tp: " << p << endl;
    entropy += (-p * log(p));
    //cout << "\tentropy: " << entropy << endl;
  }
  return entropy;
}

float WordFinder::CalculateEntropy(const string& token) {
  string reversed = ucs_reverse(token);
  //1. calculate left entropy
  int count = trie_reverse_->commonPrefixPredict(reversed.c_str(), triple_, NUM_RESULT, reversed.size());
  if (!count) return 0.0;
  if (count > NUM_RESULT) {
    cout << "trie_reverse_ CalculateEntropy() too small NUM_RESULT which less than " << count << " of " << token << endl;
    count = NUM_RESULT;
  }
  map<string, int> tf;
  int start = 0;
  char key[64] = {0};
  if (triple_[0].length == 0) start = 1;
  for (int i = start; i < count; i++) {
    trie_reverse_->suffix(key, triple_[i].length, triple_[i].id);
    int l = u8_seqlen(key);
    string ch = string(key, l);
    map_counter(tf, ch, triple_[i].value);
  }
  float left_entropy = CalculateEntropy(tf);
  //2. calculate right entropy
  count = trie_forword_->commonPrefixPredict(token.c_str(), triple_, NUM_RESULT, token.size());
  if (!count) return 0.0;
  if (count > NUM_RESULT) {
    cout << "trie_forword_ CalculateEntropy() too small NUM_RESULT which less than " << count  << " of " << token << endl;
    count = NUM_RESULT;
  }
  tf.clear();
  //cout << "trie_forword_ predict : " << token << ", count: " << count << endl;
  if (triple_[0].length == 0) start = 1;
  for (int i = start; i < count; i++) {
    trie_forword_->suffix(key, triple_[i].length, triple_[i].id);
    int l = u8_seqlen(key);
    string ch = string(key, l);
    map_counter(tf, ch, triple_[i].value);
  }
  float right_entropy = CalculateEntropy(tf);
  //cout << "left_entropy: " << left_entropy << ", right_entropy: " << right_entropy << endl;
  if (left_entropy < right_entropy) return left_entropy;
  return right_entropy;
}


void WordFinder::Find(int min_frequency, int min_jointy, int min_entropy,
    vector<NewWord>& newwords) {
  cout << "start finding new words..." << endl;
  cout << "totalcount tokens: " << totalcount_ << endl;
  cout << "number of tokens:  " << trie_forword_->num_keys() << endl;
  string token;
  //vector<NewWord> newwords;
  while (1) {
    int frequency = NextKey(token);
    if (token.empty()) break;
    if (token.size() < 4) {
      continue;
    }
    if (segger_->is_in_dict(token.c_str())) {
      //cout << "old word: " << token << endl;
      continue;
    }
    if (frequency < min_frequency) continue;
    float jointy = CalculateJointy(token, frequency);
    //cout << token << " jointy: " << jointy << endl;
    if (jointy < min_jointy) continue;
    float entropy = CalculateEntropy(token);
    //cout << token << " entropy: " << entropy << endl;
    if (entropy < min_entropy) continue;
    NewWord nw(token, frequency, jointy, entropy);
    newwords.push_back(nw);
  }
}

void WordFinder::Find(int min_frequency, int min_jointy, int min_entropy,
    const char* save_to_path) {
  vector<NewWord> newwords;
  Find(min_frequency, min_jointy, min_entropy, newwords);
  ofstream ofs(save_to_path);
  if (!ofs) {
    cout << "Can't open file: " << save_to_path << endl;
    return;
  }
  for (auto& w : newwords) {
    ofs << w.word << " " << w.frequency << " " << w.jointy << " " << w.entropy << endl;
  }
  ofs.close();
}
