#include "trie_seg.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 3) {
        const char* wf = argv[1];
        const char* sf = argv[2];
        make_dict(wf, sf);
    }else if(argc == 4) {
        const char* dict_f = argv[1];
        const char* opt = argv[2];
        if(strcmp(opt, "-f") != 0) {
            cout << "invalid option:" << opt << endl;
            return -1;
        }
        TrieSegger segger(dict_f);
        const char* text_f = argv[3];
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
        struct timeval b,e;
        string out;
        gettimeofday(&b, 0);
        out = segger.seg(buf, 1);
        gettimeofday(&e, 0);
        double timecost = (e.tv_sec - b.tv_sec)*1000 + (e.tv_usec - b.tv_usec)/1000;
        cout << "time cost: "<< timecost << endl;
        double perf = size/1024.0/1024.0/(timecost/1000);
        cout << "performance: " << perf << " MB/sec" << endl;
        FILE *fo = fopen("z-test-out.txt", "w");
        fwrite(out.c_str(), 1, out.size(), fo);
        fclose(fo);
        // 2. _core:
        gettimeofday(&b, 0);
        vector<string> wout;
        segger._core(buf, wout, segger.kMatchMaximum);
        gettimeofday(&e, 0);
        timecost = (e.tv_sec - b.tv_sec)*1000 + (e.tv_usec - b.tv_usec)/1000;
        cout << "_core() time cost: "<< timecost << endl;
        perf = size/1024.0/1024.0/(timecost/1000);
        cout << "_core() performance: " << perf << " MB/sec" << endl;


    }else if(argc == 2) {
        TrieSegger segger(argv[1]);
        const char* s = "了设计开发完了就离开家水电费就lsd附件";
        string out = segger.seg(s, 1);
        cout << s << " -> " << out << endl;
        s = "中共中央政治局常委纪检监察";
        out = segger.seg(s, 1);
        cout << s << " -> " << out << endl;
        out = segger.seg(s, 2);
        cout << s << " -> " << out << endl;
        out = segger.seg(s, 3);
        cout << s << " -> " << out << endl;
        s = "he is cOme to liFe goood oneee";
        out = segger.seg(s, 1);
        cout << s << " -> " << out << endl;
        out = segger.seg(s, 2);
        cout << s << " -> " << out << endl;
        out = segger.seg(s, 3);
        cout << s << " -> " << out << endl;
    }else {
        cout << "usage:" << endl;
        cout << "\tto make dictionary: " << argv[0] << " words-file save-file" << endl;
        cout << "\tto seg string in CMD: " << argv[0] << " dict-file" << endl;
        cout << "\tto seg file : " << argv[0] << " dict-file -f file-name" << endl;
    }
    return 0;
}
