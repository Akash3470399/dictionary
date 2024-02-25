#! usr/bin/python3

import random

# genrate random meaning for each word in filename

# input is words file


filename = input()
words = []

with open(filename, "r") as file:
    words = file.readlines()

words_arr = [word.strip() for word in words]
for i in range(len(words)):
    num = random.randint(1, 5)
    meaning = " ".join(words_arr[i:i+num] + words_arr[i-num:i])

    print(meaning)
    
