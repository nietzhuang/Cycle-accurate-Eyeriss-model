import os
import numpy as np

def conv3D(filter, image, stride):
    channel_number, filter_height, filter_width = filter.shape
    channel_number, image_height, image_width = image.shape
    ofmap_height = image_height - filter_height + stride
    ofmap_width = image_width - filter_width + stride

    psum = np.zeros((channel_number, ofmap_height, ofmap_width))
    for ch in range(channel_number):
        for i in range(ofmap_height):
            for j in range(ofmap_width):
                ofpsum = np.multiply(filter[ch, :, :], image[ch, 0+i:filter_height+i, 0+j:filter_width+j])
                psum[ch ,i, j] =  sum(sum(ofpsum))
                ofmap = sum(psum)

    return ofmap, psum

# Set parameters
pattern_name    = 'test'
channels        = 3
filter_height   = 5
filter_width    = 5
ifmap_height    = 34
ifmap_width     = 34
ofmap_height    = ifmap_height - filter_height + 1
ofmap_width     = ifmap_width - filter_width + 1


os.chdir('Patterns')
filter = np.random.randint(0, 2, size=(channels, filter_height, filter_width)) # channel, height, width
image = np.random.randint(0, 2, size=(channels, ifmap_height, ifmap_width))
ofmap, psum = conv3D(filter, image, stride=1)

filename_filter = "filter_random" + str(pattern_name) + ".dat"
filename_filter_C = "filter_rand" + str(pattern_name) + "_C++.dat"
filename_image  = "image_random" + str(pattern_name) + ".dat"
filename_image_C = "image_rand" + str(pattern_name) + "_C++.dat"
filename_ofmap = "ofmap_random" + str(pattern_name) + ".dat"
filename_ofmap_C = "ofmap_rand" + str(pattern_name) + "_C++.dat"
# filename_psum = "psum_random" + str(pattern_name) + ".dat"
filename_psum_C = "psum_rand" + str(pattern_name) + "_C++.dat"

# Write random array data
filter = np.reshape(filter, (channels, filter_height*filter_width))
filter = np.rot90(filter, 3)
filter = np.fliplr(filter)
with open(filename_filter_C, 'w') as f:
    np.savetxt(f, filter, fmt='%d')
f.close()
"""
np.savetxt(filename_filter, filter, fmt='%d')  # Save as pattern in rows of channel.
with open(filename_filter_C, 'w') as f:
    for row in filter:
        np.savetxt(f, row, fmt='%d')  # Save as SystemC pattern as flattern data.
f.close()
"""


image = np.reshape(image, (channels, ifmap_height*ifmap_width))
image = np.rot90(image, 3)
image = np.fliplr(image)
with open(filename_image_C, 'w') as im:
    np.savetxt(im, image, fmt='%d')
im.close()
"""
np.savetxt(filename_image, image, fmt='%d')
with open(filename_image_C, 'w') as im:
    for row in image:
        np.savetxt(im, row, fmt='%d')
im.close()
"""

ofmap = np.reshape(ofmap, (ofmap_height*ofmap_width))
np.savetxt(filename_ofmap, ofmap, fmt='%d')
np.savetxt(filename_ofmap_C, ofmap, fmt='%d')  # channel of ofmap is always only one.

psum = np.reshape(psum, (channels, ofmap_height*ofmap_width))
psum = np.rot90(psum, 3)
psum = np.fliplr(psum)
with open(filename_psum_C, 'w') as p:
    np.savetxt(p, psum, fmt='%d')  # Save psum in terms of psum and channel in row versus column.
p.close()


# Arrange data path
try:
    shutil.move("./"+ filename_filter_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_filter_C, "./Convert2C++/" + filename_filter_C)
try:
    shutil.move("./"+ filename_image_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_image_C, "./Convert2C++/" + filename_image_C)
try:
    shutil.move("./"+ filename_ofmap_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_ofmap_C, "./Convert2C++/" + filename_ofmap_C)
try:
    shutil.move("./"+ filename_psum_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_psum_C, "./Convert2C++/" + filename_psum_C)
