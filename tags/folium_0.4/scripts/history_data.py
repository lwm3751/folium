#!/usr/bin/env python
#coding=utf-8
import os
import string
from consts import *
from xq_data import *

def capture_scores():
    capture_scores = [[0]*32 for i in range(33)]
    e = [10000, 1041, 1040, 2000, 1088, 1096,  1020]
    m = [1000, 41, 40, 200, 88, 96, 20]
    def level(src_type, dst_type): return levels[src_type][dst_type]
    def color(piece): return PC[piece]
    def type(piece):
        t = PT[piece]
        if t >= 7:
            t -= 7
        return t
    for src_piece in range(32):
        for dst_piece in range(32):
            if color(src_piece) != color(dst_piece):
                src_type = type(src_piece)
                dst_type = type(dst_piece)
                capture_scores[dst_piece][src_piece] =  e[dst_type] - m[src_type] + 1 << 17
    return capture_scores
capture_scores = capture_scores()

def main():
    dict = {}
    dict['scores'] = d2a_str(capture_scores, u32)

    template = open(os.path.join(template_path, 'history_data.cpp.tmpl'), 'rb').read()
    template = string.Template(template)
    path = os.path.join(folium_path, 'history_data.cpp')
    open(path, 'wb').write(str(template.safe_substitute(dict)))
if __name__ == "__main__":
    main()