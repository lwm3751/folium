#!/usr/bin/env python
#coding=utf-8
import os
import string
from consts import *

def main():
    line_infos = [[0]*1024 for i in range(10)]
    for idx in range(10):
        for flag in range(1024):
            flag |= 0xFC00
            p1 = (idx and [idx-1] or [0xF])[0]
            while not flag & (1 << p1):
                p1 = (p1 and [p1-1] or [0xF])[0]
            p2 = (p1 and [p1-1] or [0xF])[0]
            while not flag & (1 << p2):
                p2 = (p2 and [p2-1] or [0xF])[0]
            n1 = idx + 1
            while not flag & (1 << n1):
                n1 += 1
            n2 = n1 + 1
            while not flag & (1 << n2):
                n2 += 1
            line_infos[idx][flag & 1023] = p1 | (p2 << 8) | (n1 << 16) | (n2 << 24)
    dict = {}
    dict['infos'] = d2a_str(line_infos, u32)
    template = open(os.path.join(template_path, 'bitlines_data.cpp.tmpl'), 'rb').read()
    template = string.Template(template)
    path = os.path.join(folium_path, 'bitlines_data.cpp')
    open(path, 'wb').write(str(template.safe_substitute(dict)))

if __name__ == "__main__":
    main()
