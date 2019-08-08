/*
  Copyright (c) 2019
  Author: veelion@gmail.com
  All rights reserved.
*/

#ifndef _UTILITY_H
#define _UTILITY_H


#include <string.h>
#include <string>
#include <map>
#include <vector>

#include "utf8.h"

inline void map_counter(std::map<std::string, int> &amap, const std::string &key, int count=1) {
  auto itr = amap.find(key);
  if (itr == amap.end()) {
    amap[key] = count;
  } else {
    amap[key] += count;
  }
}

int split_sentence(const char* doc, std::vector<std::string> &sentences) {
  if (!doc) return 0;
  sentences.clear();
  const char* s_begin = NULL;
  const char* p = doc;
  unsigned int c = 0;
  int i = 0;
  while(*p) {
    c = utf8_decode(p, &i);
    if ((48 <= c && c <= 57) ||
        (65 <= c && c <= 90) ||
        (97 <= c && c <= 122) ||
        (0x4e00 <= c && c <= 0x9fa5)) {
      if (!s_begin) s_begin = p;
    } else {
      if (s_begin) {
        // std::cout << "p: " << p << std::endl;
        int len = p - s_begin;
        if (len > 6) {
          sentences.push_back(std::string(s_begin, len));
        }
        s_begin = NULL;
      }
    }
    p += i;
  }
  if (s_begin && p-s_begin > 6) {
    sentences.push_back(std::string(s_begin));
  }
  return sentences.size();
}


std::string ucs_reverse(const std::string &s) {
  //std::string r (s.size(), 'v');
  char buf[s.size()+1];
  char* x = buf + s.size();
  const char* p = s.c_str();
  while(*p) {
    int i = u8_seqlen(p);
    x -= i;
    memcpy(x, p, i);
    p += i;
  }
  return std::string(buf, s.size());
}
#endif
