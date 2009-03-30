#!/usr/bin/env python
#coding=utf-8

def main():
    import sys
    if len(sys.argv) == 1:
        import pyfolium.protocol
        engine = pyfolium.protocol.Engine()
        engine.run()