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
            line_infos[idx][flag & 1023] = p1 | (p2 << 4) | (n1 << 8) | (n2 << 12)
    bits = [1 << i for i in range(10)]
    bit_counts = [len([j for j in bits if i & j])for i in range(1024)]
    distance_infos = [[(19 << 10 | 1023)]*128 for i in range(91)]
    for src in range(90):
        sy, sx = divmod(src, 9)
        for dst in range(90):
            if dst == src:
                continue
            dy, dx = divmod(dst, 9)
            if dx == sx:
                idx = dx+10
                mask = sum(bits[i] for i in set(range(dy+1, sy)+range(sy+1, dy)) if i in range(10))
                distance_infos[src][dst] = (idx << 10) | mask
            elif dy == sy:
                idx = dy
                mask = sum(bits[i] for i in set(range(dx+1, sx)+range(sx+1, dx)) if i in range(10))
                distance_infos[src][dst] = (idx << 10) | mask
    
    dict = {}
    dict['infos'] = d2a_str(line_infos, u32)
    dict['counts'] = d1a_str(bit_counts, u32)
    dict['distance'] = d2a_str(distance_infos, u32)
    template = open(os.path.join(template_path, 'bitboard_data.cpp.tmpl'), 'rb').read()
    template = string.Template(template)
    path = os.path.join(folium_path, 'bitboard_data.cpp')
    open(path, 'wb').write(str(template.safe_substitute(dict)))

if __name__ == "__main__":
    main()
