import os
import sys
import dbhash
import cPickle

import book

class bsdbook(book.Book):
    def __init__(self):
        book.Book.__init__(self)
        bookfile = os.path.join(os.path.dirname(sys.argv[0]), 'snake.bsd')
        self.book = dbhash.open(bookfile, 'r')
    def readmoves(self, fen):
        _, moves = self.book.set_location(fen)
        if _ == fen:
            return cPickle.loads(moves)