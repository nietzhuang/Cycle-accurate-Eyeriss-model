import os
import shutil

import numpy as np

# Set parameters
pattern_seed    = 0
channels        = 1
filter_height   = 11
filter_width    = 11
ifmap_height    = 80
ifmap_width     = 80


os.chdir('Patterns')
filter = np.random.randint(0, 9, size=(channels, filter_height, filter_width)) # channel, height, width
image = np.random.randint(0, 9, size=(channels, ifmap_height, ifmap_width))

filename_filter = "filter_random" + str(pattern_seed) + ".dat"
filename_filter_C = "filter_rand" + str(pattern_seed) + "_C++.dat"
filename_image  = "image_random" + str(pattern_seed) + ".dat"
filename_image_C = "image_rand" + str(pattern_seed) + "_C++.dat"


# Write random array data
with open(filename_filter, 'w') as f:
    for char in filter:
        f.write(str(char))
f.close()
with open(filename_image, 'w') as im:
    for line in image:
        for char in line:
            im.write(str(char))
im.close()

# Convert array data to convenient data for C++ pattern
with open(filename_filter, 'r') as f:
    for line in f.readlines():
        line = line.replace('][', ' ')
        line = line.replace('[[', '[')
        line = line.replace(']]', ']')
        line = line.replace(' [', ' ')
        line = line.replace(']', '')
        line = line.replace('[', '\n')
        line = line.replace(' ', '\n')

        with open(filename_filter_C, 'a') as w:
            if line[0] == '\n':
                line = line[1:]
            w.write(line)

# Check whether the number of lines is correct
num_line = 0
with open(filename_filter_C, 'r') as im:
    for line in im.readlines():
        num_line += 1
    if (num_line != filter_height*filter_width*channels):
        print('Warning! This random pattern may not match the format. Please run the script again.')
f.close()
w.close()

with open(filename_image, 'r') as im:
    for line in im.readlines():
        line = line.replace('][', ' ')
        line = line.replace('[[', '[')
        line = line.replace(']]', ']')
        line = line.replace(' [', '')
        line = line.replace(']', '')
        line = line.replace('[', '\n')
        line = line.replace(' ', '\n')

        with open(filename_image_C, 'a') as w:
            if line[0] == '\n':
                line = line[1:]
            w.write(line)

# Check whether the number of lines is correct
num_line = 0
with open(filename_image_C, 'r') as im:
    for line in im.readlines():
        num_line += 1
    if (num_line != ifmap_height*ifmap_width*channels):
        print('Warning! This random pattern may not match the format. Please run the script again.')
im.close()
w.close()


# Arrange data path
try:
    shutil.move("./"+ filename_filter_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_filter_C, "./Convert2C++/" + filename_filter_C)
try:
    shutil.move("./"+ filename_image_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_image_C, "./Convert2C++/" + filename_image_C)
