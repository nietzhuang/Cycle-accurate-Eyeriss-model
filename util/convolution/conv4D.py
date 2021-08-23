import os
import numpy as np

def conv4D(filter, image, stride, padding):
    channel_number, filter_num, filter_height, filter_width = filter.shape
    channel_number, image_height, image_width = image.shape
    ofmap_height = int((image_height - filter_height + padding*2 + stride) / stride)
    ofmap_width = int((image_width - filter_width + padding*2 + stride) / stride)
    ofmap_num    = filter_num

    if padding > 0:
        image_pad = np.zeros((channel_number, image_height+padding*2, image_width+padding*2))
        image_pad[:, padding:-padding, padding:-padding] = image

    ofmap = np.zeros((ofmap_num, ofmap_height, ofmap_width))
    psum = np.zeros((ofmap_num, channel_number, ofmap_height, ofmap_width))
    for fil_num in range(filter_num):
        for ch in range(channel_number):
            for i in range(ofmap_height):
                for j in range(ofmap_width):
                    if padding > 0:
                        ofpsum = np.multiply(filter[ch, fil_num, :, :], image_pad[ch, 0+i*stride:filter_height+i*stride, 0+j*stride:filter_width+j*stride])
                    else:
                        ofpsum = np.multiply(filter[ch, fil_num, :, :], image[ch, 0+i*stride:filter_height+i*stride, 0+j*stride:filter_width+j*stride])
                    psum[fil_num, ch ,i, j] = sum(sum(ofpsum))
        ofmap[fil_num] = sum(psum[fil_num, :, :, :])

    return ofmap, psum

# Set parameters
pattern_name    = 'filter5x5x3x2_imagee81x81x3_stride2_pad2'
channels        = 3
filter_num      = 2
filter_height   = 5
filter_width    = 5
ifmap_height    = 81
ifmap_width     = 81
stride          = 2
padding         = 2
ofmap_height    = int((ifmap_height - filter_height + padding*2 + stride) / stride)
ofmap_width     = int((ifmap_width - filter_width + padding*2 + stride) / stride)
ofmap_channels  = filter_num

# Define filename
filename_filter = "filter_" + str(pattern_name) + ".dat"
filename_filter_C = "filter_" + str(pattern_name) + "_C++.dat"
filename_image  = "image_" + str(pattern_name) + ".dat"
filename_image_C = "image_" + str(pattern_name) + "_C++.dat"
filename_ofmap = "ofmap_" + str(pattern_name) + ".dat"
filename_ofmap_C = "ofmap_" + str(pattern_name) + "_C++.dat"
filename_psum = "psum_" + str(pattern_name) + ".dat"
filename_psum_C = "psum_" + str(pattern_name) + "_C++.dat"


os.chdir('Patterns')
filter = np.random.randint(0, 2, size=(channels, filter_num, filter_height, filter_width)) # channel, height, width
image = np.random.randint(0, 2, size=(channels, ifmap_height, ifmap_width))
ofmap, psum = conv4D(filter, image, stride=stride, padding=padding)


# Write data in shape
with open(filename_filter, 'w') as f:
    f.write("Channel: 0\n")
    f.write("\n\n\nChannel: \n".join(map(str, filter)))
f.close()

with open(filename_image, 'w') as im:
    im.write("Channel\n")
    im.write("\n\n\nChannel\n".join(map(str, image)))
im.close()

with open(filename_ofmap, 'w') as om:
    om.write("Channel\n")
    om.write("\n\n\nChannel\n".join(map(str, ofmap)))
om.close()

with open(filename_psum, 'w') as p:
    p.write("ofmap number: 0\n")
    p.write("\n\n\nofmap number: \n".join(map(str, psum)))
p.close()


# Write as SystemC pattern
filter = np.reshape(filter, (channels, filter_height*filter_width*filter_num))
filter = np.rot90(filter, 3)
filter = np.fliplr(filter)
f = open(filename_filter_C, 'w')
np.savetxt(f, filter, fmt='%d')
f.close()

image = np.reshape(image, (channels, ifmap_height*ifmap_width))
image = np.rot90(image, 3)
image = np.fliplr(image)
im = open(filename_image_C, 'w')
np.savetxt(im, image, fmt='%d')
im.close()

ofmap_tmp = np.zeros((ofmap_channels, ofmap_height*ofmap_width))
ofmap = np.reshape(ofmap, (ofmap_channels*ofmap_height*ofmap_width))
for ch in range(ofmap_channels):  # Arrange it in shape as next feature map
    ofmap_tmp[ch][:] = ofmap[ch*ofmap_height*ofmap_width : (ch+1)*ofmap_height*ofmap_width]
ofmap = np.rot90(ofmap_tmp, 3)
ofmap = np.fliplr(ofmap)
om = open(filename_ofmap_C, 'w')
np.savetxt(om, ofmap, fmt='%d')
om.close()

psum_tmp = np.zeros((ofmap_height*ofmap_width*filter_num, channels))
psum = np.reshape(psum, (filter_num, channels, ofmap_height*ofmap_width))
for fil_num in range(filter_num): # Arrange it as the shape row, coloumn in terms of psum data, channels
    psum_tmp[fil_num*ofmap_height*ofmap_width:(fil_num+1)*ofmap_height*ofmap_width][:] = psum[fil_num][:][:].T
p = open(filename_psum_C, 'w')
np.savetxt(p, psum_tmp, fmt='%d')  # Save psum in terms of psum and channel in row versus column.
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
