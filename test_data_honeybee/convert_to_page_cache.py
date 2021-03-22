import struct

def convert_to_page_cache(elf_path, start_addr, output):
  f = open(elf_path)

  f2 = open("%s.dump"%(output), "w")
  total = 0
  while True:
    data = f.read(0x1000)
    if data:
      f2.write(data)
      total = total + len(data)
    else:
      break

  f.close()
  f2.close()

  if total%0x1000:
    total = (total&0xFFFFFFFFFFFFF000) + 0x1000

  length = total
  start_addr = start_addr

  f = open("%s.addr"%(output), "w")
  for i in range(length/0x1000):
    f.write((struct.pack("<Q", start_addr+(i*0x1000))))
  f.close()

convert_to_page_cache("small/small.bin", 0x400000, "small/small")
convert_to_page_cache("medium/medium.bin", 0x400000, "medium/medium")
convert_to_page_cache("mirror/mirror.bin", 0x400000, "mirror/mirror")
convert_to_page_cache("html_fast_parse/html_fast_parse.bin", 0x555555554000, "html_fast_parse/html_fast_parse")
convert_to_page_cache("tar/tar.bin", 0x55555555d000, "tar/tar")
convert_to_page_cache("ssh/ssh.bin", 0x55555555e000, "ssh/ssh")
convert_to_page_cache("clang/clang.bin", 0x400000, "clang/clang")

