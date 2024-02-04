import random

# genrate random meaning for each word in filename

filename = input("enter filename :")
words = []

with open(filename, "r") as file:
    words = file.readlines()

words_arr = [word.strip() for word in words]
for i in range(len(words)):
    num = random.randint(1, 5)
    meaning = " ".join(words_arr[i:i+num] + words_arr[i-num:i])
    print(meaning)
    
