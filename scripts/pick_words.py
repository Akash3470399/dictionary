#! /usr/bin/python3

import random
filename = "../data/words"
n = 30 

words = []
with open(filename) as f:
    words = f.readlines()
    words = list(map(lambda w : w.strip(), words))
    wlen = len(words)

    while n > 0: 
        r1 = random.randint(1, wlen)
        print(words[r1])
        n -= 1
     
