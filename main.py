#!/usr/bin/env python
#coding=utf-8

def ucci():
    import snake.protocol
    engine = snake.protocol.Engine()
    engine.run()

def perft():
    import _xq
    def _():
        _xq.writeline(str(xq))
        for index, count in enumerate(perfts):
            t1 = _xq.currenttime()
            assert(xq.perft(index) == count)
            t2 = _xq.currenttime()
            if t2 - t1 > 0.1:
                _xq.writeline("ply:%d\tcount:%d\tnode:%d"%(index+1, count, int(count/(t2-t1))))

    xq = _xq.XQ("r1ba1a3/4kn3/2n1b4/pNp1p1p1p/4c4/6P2/P1P2R2P/1CcC5/9/2BAKAB2 r")
    perfts = [38, 1128, 43929, 1339047, 53112976]
    _()
    xq = _xq.XQ("r2akab1r/3n5/4b3n/p1p1pRp1p/9/2P3P2/P3P3c/N2C3C1/4A4/1RBAK1B2 r")
    perfts = [58, 1651, 90744, 2605437, 140822416]
    _()
    xq = _xq.XQ("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r")
    perfts = [44, 1920, 79666, 3290240, 133312995, 5392831844]
    #_()