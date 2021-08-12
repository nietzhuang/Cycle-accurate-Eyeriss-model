import numpy as np
import string

with open("./Patterns/image_random0.dat", 'r') as f:
    for line in f.readlines():
        line = line.replace('][', ' ')
        print(line)
