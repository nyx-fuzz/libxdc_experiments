import sys

# 2 GB
MAX_LEN = 2 << 30

if len(sys.argv) >= 3:

  f = open(sys.argv[1], 'rb')
  data = ""
  while True:
    tmp = f.read()
    if tmp:
      data += tmp
    else:
      break
  f.close()
  #print("SIZE: %d"%(len(data)))

  f = open(sys.argv[2], 'wb')
  size = 0
  while size <= MAX_LEN:
    f.write(data)
    size += len(data)
  f.close()
