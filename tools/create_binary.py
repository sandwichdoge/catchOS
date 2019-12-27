data = bytearray([0xff, 0xff, 0xff, 0xff])

fd = open('mod.so', 'wb')
fd.write(data)
fd.close()