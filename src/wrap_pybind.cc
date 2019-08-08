/*
   Copyright (c) 2019
Author: veelion
All rights reserved.
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "trie_seg.h"
#include "wordfinder.h"

namespace py = pybind11;
PYBIND11_MAKE_OPAQUE(std::vector<std::string, std::allocator<std::string>>);
using StringVector = std::vector<std::string, std::allocator<std::string>>;

PYBIND11_MODULE(trieseg, m) {
  // This will enable user-defined docstrings and python signatures,
  // while disabling the C++ signatures
  m.doc() = "[trieseg] module is created by veelion.\n"
    "It's a Chinese word segmentation Class implemented in C++\n"
    "and exported as Python module using pybind11";
  m.def(
      "make_dict", &make_dict,
      "Function: make a dictionary of many words\n"
      "Definition: make_dict(word_str, save_file, jointer)\n"
      "PARAM-1: [word_file] is a file of words per line\n"
      "PARAM-2: [save_file] is name of dict to be saved\n"
      "example:\n"
      "    dictseg.make_dict('words.txt', 'z.dict')\n"
      "RETURN : None, no return\n"
     );
  py::class_<StringVector>(m, "StringVector")
    .def(py::init<>())
    .def("pop_back", &StringVector::pop_back)
    .def("push_back", (void (StringVector::*)(const std::string &)) &StringVector::push_back)
    .def("back", (std::string &(StringVector::*)()) &StringVector::back)
    .def("__len__", [](const StringVector &v) { return v.size();  })
    .def("__iter__", [](StringVector &v) {
        return py::make_iterator(v.begin(), v.end());
    }, py::keep_alive<0, 1>());

  py::class_<TrieSegger>(m, "TrieSegger", "to create an TrieSegger instance:\nsegger = dictseg.TrieSegger(dict-file-name)")
    .def(py::init<const char*>())
    .def(py::init<>())
    // methods for Chinese word segmentation
    .def_readonly("MATCH_MINIMUM", &TrieSegger::kMatchMinimum)
    .def_readonly("MATCH_MAXIMUM", &TrieSegger::kMatchMaximum)
    .def_readonly("MATCH_ALL", &TrieSegger::kMatchAll)
    .def(
        "load_custome_word", &TrieSegger::LoadCustomeWord,
        "Function: load custome words to segmentation dictionary\n"
        "PARAM-1: [path] path of word file with one word one line:\n"
        "         中国 300 # word and it's frequency\n"
        "RETURN: None"
        )
    .def(
        "get_dict_value", &TrieSegger::get_dict_value,
        "Function: get the value of a word stored in the dict\n"
        "Definition: get_dict_value(word)\n"
        "PARAM-1: [word] string of the word\n"
        "RETURN: the value, 0 if word is not in the dict\n"
        )
    .def(
        "is_in_dict", &TrieSegger::is_in_dict,
        "Function: check a word in the dict or not\n"
        "Definition: is_in_dict(word)\n"
        "PARAM-1: [word] string of the word\n"
        "RETURN: true if it is in the seg dict\n"
        )
    .def(
        "prefix", &TrieSegger::prefix,
        "Function: find all words with the same prefix\n"
        "Definition: prefix(prefix)\n"
        "PARAM-1: [prefix] string to prefix\n"
        "PARAM-2: [words] vector of string for words found\n"
        "RETURN: count of words found\n"
        )
    .def(
        "dict_insert", &TrieSegger::dict_insert,
        "Function: insert new word to the dictionary\n"
        "Definition: dict_insert(word-string, word-value)\n"
        "PARAM-1: [word-string] string of the new word\n"
        "PARAM-2: [word-value] a int value of the word\n"
        "RETURN: 0 if succeed, -1 if word-string is empty\n"
        )
    .def(
        "seg", &TrieSegger::seg,
        "Function: seg string to get a result of string\n"
        "Definition: seg(str, match_type)\n"
        "PARAM-1: [str] is what you want to seg\n"
        "PARAM-2: [match_type] is which match way you want to choose\n"
        "           self.MATCH_MINIMUM: minimum matching;\n"
        "           self.MATCH_MAXIMUM: maximum matching;\n"
        "           self.MATCH_ALL: all-words matching;\n"
        "RETURN : a string of segged words jointed by ' '\n"
        )
    .def(
        "seg2vec", &TrieSegger::seg2vec,
        "Function: seg string and put words into vector\n"
        "Definition: seg2vec(string, words-vector, match_type)\n"
        "PARAM-1: [str] is what you want to seg\n"
        "PARAM-2: [words-vector] a stdd:vector of string to save words\n"
        "PARAM-3: [match_type] is which match way you want to choose\n"
        "           self.MATCH_MINIMUM: minimum matching;\n"
        "           self.MATCH_MAXIMUM: maximum matching;\n"
        "           self.MATCH_ALL: all-words matching;\n"
        "RETURN: no return\n"
        );
  py::class_<WordFinder>(m, "WordFinder", "find new words from texts")
    .def(py::init<int>())
    .def("load_segger_dict", &WordFinder::LoadSeggerDict)
    .def("feed", &WordFinder::Feed)
    .def("find", py::overload_cast<int, int, int, const char*>(&WordFinder::Find));
}

