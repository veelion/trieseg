
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "wordfinder.h"

using namespace std;

int main(int argc, char* argv[]) {
  const char* doc = "吃葡萄不吐葡萄皮不吃葡萄倒吐葡萄皮";
  if (argc > 1) {
    const char* text_f = argv[1];
    FILE *fp = fopen(text_f, "r");
    if(!fp){
      cout << "can not open text file: " << text_f << endl;
      return -1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    cout << "test file size:" << size << endl;
    char* buf = new char[size+1];
    rewind(fp);
    size_t r = fread(buf, 1, size, fp);
    fclose(fp);
    cout << "read file:" << r << endl;
    buf[size] = 0;
    doc = buf;
  }
  WordFinder wf(8);
  wf.LoadSeggerDict("./z4");
  wf.Feed(doc);

  vector<NewWord> newwords;
  wf.FindCore(1, 1, 1, newwords);

  sort(newwords.begin(), newwords.end(), [](NewWord& a, NewWord& b) { return a.entropy > b.entropy; });
  cout << "newwords: " << newwords.size() << endl;
  for (auto& n : newwords) {
    cout << n.word << " " << n.frequency << " " << n.jointy << " " << n.entropy << endl;
  }

  wf.Find(1, 1, 1, "z-new-words.txt");


  return 0;
}
