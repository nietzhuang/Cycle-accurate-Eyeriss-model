import os
import numpy as np

def conv3D(filter, image, stride, padding):
    channel_number, filter_height, filter_width = filter.shape
    channel_number, image_height, image_width = image.shape
    ofmap_height = int((image_height - filter_height + padding*2 + stride) / stride)
    ofmap_width = int((image_width - filter_width + padding*2 + stride) / stride)

    if padding > 0:
        image_pad = np.zeros((channel_number, image_height+padding*2, image_width+padding*2))
        image_pad[:, padding:-padding, padding:-padding] = image

    psum = np.zeros((channel_number, ofmap_height, ofmap_width))
    for ch in range(channel_number):
        for i in range(ofmap_height):
            for j in range(ofmap_width):
                if padding > 0:
                    ofpsum = np.multiply(filter[ch, :, :], image_pad[ch, 0+i*stride:filter_height+i*stride, 0+j*stride:filter_width+j*stride])
                else:
                    ofpsum = np.multiply(filter[ch, :, :], image[ch, 0+i*stride:filter_height+i*stride, 0+j*stride:filter_width+j*stride])
                psum[ch ,i, j] =  sum(sum(ofpsum))
    ofmap = sum(psum)

    return ofmap, psum

# Set parameters
pattern_name    = 'filter3x3x3_ifmap5x5x3_stride2'
channels        = 3
filter_num      = 1
filter_height   = 3
filter_width    = 3
ifmap_height    = 5
ifmap_width     = 5
stride          = 2
padding         = 0
ofmap_height    = int((ifmap_height - filter_height + padding*2 + stride) / stride)
ofmap_width     = int((ifmap_width - filter_width + padding*2 + stride) / stride)
ofmap_channels  = filter_num
dataflow        = 'RS'
layer           = 'CONV'

# Define filename
filename_filter = "filter_" + str(pattern_name) + ".dat"
filename_filter_C = "filter_" + str(pattern_name) + "_C++.dat"
filename_image  = "image_" + str(pattern_name) + ".dat"
filename_image_C = "image_" + str(pattern_name) + "_C++.dat"
filename_ofmap = "ofmap_" + str(pattern_name) + ".dat"
filename_ofmap_C = "ofmap_" + str(pattern_name) + "_C++.dat"
filename_psum = "psum_" + str(pattern_name) + ".dat"
filename_psum_C = "psum_" + str(pattern_name) + "_C++.dat"
filename_config = "config_" + str(pattern_name) + ".dat"


os.chdir('Patterns')
filter = np.random.randint(0, 2, size=(channels, filter_height, filter_width)) # channel, height, width
image = np.random.randint(0, 2, size=(channels, ifmap_height, ifmap_width))
ofmap, psum = conv3D(filter, image, stride=stride, padding=padding)


# Write configuration bits
layer_bw            = 2
dataflow_bw         = 2
padding_bw          = 3
stride_bw           = 3
filter_num_bw       = 10
channels_bw         = 10
ifmap_width_bw      = 10
ifmap_height_bw     = 10
filter_width_bw     = 4
filter_height_bw    = 4
Dataflow            = {'WS': 0, 'IS': 1, 'OS': 2, 'RS': 3}
LayerType           = {'CONV': 0, 'MAX': 1, 'FC': 2}

config_reg =  '0' * (64 - layer_bw - dataflow_bw - \
              padding_bw - stride_bw - filter_num_bw - \
              channels_bw - ifmap_width_bw - ifmap_height_bw - \
              filter_width_bw - filter_height_bw) + \
              '{0:02b}'.format(LayerType[layer]) + \
              '{0:02b}'.format(Dataflow[dataflow]) + \
              '{0:03b}'.format(padding) + \
              '{0:03b}'.format(stride) + \
              '{0:010b}'.format(filter_num) + \
              '{0:010b}'.format(channels) + \
              '{0:010b}'.format(ifmap_width) + \
              '{0:010b}'.format(ifmap_height) + \
              '{0:04b}'.format(filter_width) + \
              '{0:04b}'.format(filter_height)

conf = open(filename_config, 'w')
for bit in config_reg:
    conf.write(bit +"\n")
conf.close()


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
    #om.write("Channel\n")
    #om.write("\n\n\nChannel\n".join(map(str, ofmap)))
    om.write(str(ofmap))
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
try:
    shutil.move("./"+ filename_config, "./Convert2C++/.")
except:
    os.replace("./"+ filename_config, "./Convert2C++/" + filename_config)
