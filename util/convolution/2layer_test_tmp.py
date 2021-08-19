import os
import numpy as np

def conv3D(filter, image, stride):
    channel_number, filter_height, filter_width = filter.shape
    channel_number, image_height, image_width = image.shape
    ofmap_height = int((image_height - filter_height + stride) / stride)
    ofmap_width = int((image_width - filter_width + stride) / stride)

    psum = np.zeros((channel_number, ofmap_height, ofmap_width))
    for ch in range(channel_number):
        for i in range(ofmap_height):
            for j in range(ofmap_width):
                ofpsum = np.multiply(filter[ch, :, :], image[ch, 0+i*stride:filter_height+i*stride, 0+j*stride:filter_width+j*stride])
                psum[ch ,i, j] =  sum(sum(ofpsum))
                ofmap = sum(psum)

    return ofmap, psum

# Set parameters
pattern_name    = '2layer_test'
channels1       = 3
filter1_height  = 5
filter1_width   = 5
ifmap_height    = 81
ifmap_width     = 81
stride1         = 2
ofmap1_height   = int((ifmap_height - filter1_height + stride1) / stride1)  # 39
ofmap1_width    = int((ifmap_width - filter1_width + stride1) / stride1)

channels2       = 1
filter2_height  = 5
filter2_width   = 5
stride2         = 2
ofmap2_height   = int((ofmap1_height - filter2_height + stride2) / stride2)  # 18
ofmap2_width    = int((ofmap1_width - filter2_width + stride2) / stride2)


Path = os.getcwd()
os.chdir('Patterns')
try:
    os.mkdir(pattern_name)
except:
    pass
os.chdir(pattern_name)
filter1 = np.random.randint(0, 2, size=(channels1, filter1_height, filter1_width)) # channel, height, width
filter2 = np.random.randint(0, 2, size=(channels2, filter2_height, filter2_width))
image = np.random.randint(0, 2, size=(channels1, ifmap_height, ifmap_width))


# Create 2 layers
ofmap1, psum1 = conv3D(filter1, image, stride=stride1)
ofmap1 = ofmap1[np.newaxis, :, :]
ofmap2, psum2 = conv3D(filter2, ofmap1, stride=stride2)


filename_filter1    = "filter1_" + str(pattern_name) + ".dat"
filename_filter2    = "filter2_" + str(pattern_name) + ".dat"
filename_filter1_C  = "filter1_" + str(pattern_name) + "_C++.dat"
filename_filter2_C  = "filter2_" + str(pattern_name) + "_C++.dat"
filename_image      = "image_" + str(pattern_name) + ".dat"
filename_image_C    = "image_" + str(pattern_name) + "_C++.dat"
filename_ofmap1     = "ofmap1_" + str(pattern_name) + ".dat"
filename_ofmap2     = "ofmap2_" + str(pattern_name) + ".dat"
filename_ofmap1_C   = "ofmap1_" + str(pattern_name) + "_C++.dat"
filename_ofmap2_C   = "ofmap2_" + str(pattern_name) + "_C++.dat"
filename_psum1      = "psum1_" + str(pattern_name) + ".dat"
filename_psum2      = "psum2_" + str(pattern_name) + ".dat"
filename_psum1_C    = "psum1_" + str(pattern_name) + "_C++.dat"
filename_psum2_C    = "psum2_" + str(pattern_name) + "_C++.dat"


# Write data in shape
with open(filename_filter1, 'w') as f:
    f.write("Channel: 0\n")
    f.write("\n\n\nChannel: \n".join(map(str, filter1)))
f.close()
with open(filename_filter2, 'w') as f:
    f.write("Channel: 0\n")
    f.write("\n\n\nChannel: \n".join(map(str, filter2)))
f.close()

with open(filename_image, 'w') as im:
    im.write("Channel\n")
    im.write("\n\n\nChannel\n".join(map(str, image)))
im.close()

with open(filename_ofmap1, 'w') as om:
    om.write("Channel\n")
    om.write("\n\n\nChannel\n".join(map(str, ofmap1)))
om.close()
with open(filename_ofmap2, 'w') as om:
    om.write("Channel\n")
    om.write("\n\n\nChannel\n".join(map(str, ofmap2)))
om.close()

with open(filename_psum1, 'w') as p:
    p.write("ofmap number: 0\n")
    p.write("\n\n\nofmap number: \n".join(map(str, psum1)))
p.close()
with open(filename_psum2, 'w') as p:
    p.write("ofmap number: 0\n")
    p.write("\n\n\nofmap number: \n".join(map(str, psum2)))
p.close()


# Write random array data
filter1 = np.reshape(filter1, (channels1, filter1_height*filter1_width))
filter1 = np.rot90(filter1, 3)
filter1 = np.fliplr(filter1)
with open(filename_filter1_C, 'w') as f:
    np.savetxt(f, filter1, fmt='%d')
f.close()
filter2 = np.reshape(filter2, (channels2, filter2_height*filter2_width))
filter2 = np.rot90(filter2, 3)
filter2 = np.fliplr(filter2)
with open(filename_filter2_C, 'w') as f:
    np.savetxt(f, filter2, fmt='%d')
f.close()

image = np.reshape(image, (channels1, ifmap_height*ifmap_width))
image = np.rot90(image, 3)
image = np.fliplr(image)
with open(filename_image_C, 'w') as im:
    np.savetxt(im, image, fmt='%d')
im.close()

ofmap1 = np.reshape(ofmap1, (ofmap1_height*ofmap1_width))
np.savetxt(filename_ofmap1, ofmap1, fmt='%d')
np.savetxt(filename_ofmap1_C, ofmap1, fmt='%d')  # channel of ofmap is always only one.
ofmap2 = np.reshape(ofmap2, (ofmap2_height*ofmap2_width))
np.savetxt(filename_ofmap2, ofmap2, fmt='%d')
np.savetxt(filename_ofmap2_C, ofmap2, fmt='%d')  # channel of ofmap is always only one.

psum1 = np.reshape(psum1, (channels1, ofmap1_height*ofmap1_width))
psum1 = np.rot90(psum1, 3)
psum1 = np.fliplr(psum1)
with open(filename_psum1_C, 'w') as p:
    np.savetxt(p, psum1, fmt='%d')  # Save psum in terms of psum and channel in row versus column.
p.close()
psum2 = np.reshape(psum2, (channels2, ofmap2_height*ofmap2_width))
psum2 = np.rot90(psum2, 3)
psum2 = np.fliplr(psum2)
with open(filename_psum2_C, 'w') as p:
    np.savetxt(p, psum2, fmt='%d')  # Save psum in terms of psum and channel in row versus column.
p.close()






# Arrange data path
try:
    os.mkdir("Convert2C++")
except:
    pass
try:
    shutil.move("./"+ filename_filter1_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_filter1_C, "./Convert2C++/" + filename_filter1_C)
try:
    shutil.move("./"+ filename_filter2_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_filter2_C, "./Convert2C++/" + filename_filter2_C)
try:
    shutil.move("./"+ filename_image_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_image_C, "./Convert2C++/" + filename_image_C)
try:
    shutil.move("./"+ filename_ofmap1_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_ofmap1_C, "./Convert2C++/" + filename_ofmap1_C)
try:
    shutil.move("./"+ filename_ofmap2_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_ofmap2_C, "./Convert2C++/" + filename_ofmap2_C)
try:
    shutil.move("./"+ filename_psum1_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_psum1_C, "./Convert2C++/" + filename_psum1_C)
try:
    shutil.move("./"+ filename_psum1_C, "./Convert2C++/.")
except:
    os.replace("./"+ filename_psum2_C, "./Convert2C++/" + filename_psum2_C)
os.chdir(Path)
