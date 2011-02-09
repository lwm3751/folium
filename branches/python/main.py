#!/usr/bin/env python
#coding=utf-8

def main():
    import sys
    if len(sys.argv) == 1:
        import snake.protocol
        engine = snake.protocol.Engine()
        engine.run()

if __name__ == "__main__":
    main()