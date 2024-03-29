#!/usr/bin/env python3
# coding:utf-8
# Author: veelion


import os
import trieseg

def find(path):
    print('finding....')
    wf = trieseg.WordFinder(9)
    wf.load_segger_dict('./dict.txt')
    print('load_segger_dict done')
    doc = open(path, encoding='utf8').read()
    wf.feed(doc)

    wf.find(1, 100, 1, 'z.txt')


if __name__ == '__main__':
    from sys import argv
    fp = argv[1]
    find(fp)
