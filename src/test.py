#!/home/poulter/.local/bin/python3

import sys
import math

numbers = []
while True :
  data = sys.stdin.readline()
  if data == '':
    break
  if data == '\n':
    if numbers[0]*numbers[0] != numbers[1] :
      for x in numbers:
        print(x)
      print(numbers[0]*numbers[0])
      break
    print("Verified")
    numbers = []
    continue
  numbers.append(int(data))
print("Done!")
