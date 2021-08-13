with open("ofmap.dat", 'r') as i:
    str = i.read()
    str = str.replace(".", "\n")

    with open("ofmap.txt", 'w') as out:
        out.write(str)
