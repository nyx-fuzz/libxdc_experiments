#!/usr/bin/env python2
"""
Copyright (C) 2020 Sergej Schumilo

This file is part of kAFL Fuzzer (kAFL).

QEMU-PT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

QEMU-PT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QEMU-PT.  If not, see <http://www.gnu.org/licenses/>.
"""

import msgpack
import os
import sys
import struct
import os.path
import hashlib


def to_elf(pages, x86=False):
    dw = "h"
    dd = "i"
    dq = "Q"

    eip = 0x400000
    page_size = 0x1000
    ehsize = 0x40
    phdrsize =0x38
    phdr_offset = ehsize

    num_phdr= len(pages)

    elf = "\x7fELF"
    if(x86):
      elf += "\x01" #ELFCLASS32 
    else:
      elf += "\x02" #ELFCLASS64
    elf += "\x01\x01\x00"
    elf += "\x00"*8
    elf += struct.pack(dw, 2) #e_type
    if(x86):
      elf += struct.pack(dw, 0x3) #e_machine EM_386
    else:
      elf += struct.pack(dw, 0x3e) #e_machine c86_64
    elf += struct.pack(dd, 1) #current_version
    elf += struct.pack(dq, eip) #eip
    elf += struct.pack(dq, phdr_offset) #phdr_offset
    elf += struct.pack(dq, 0) #e_shoff no section headers
    elf += struct.pack(dd, 0) #e_flags no flags
    elf += struct.pack(dw, ehsize) #e_hsize b
    elf += struct.pack(dw, phdrsize) #phdrsize
    elf += struct.pack(dw, num_phdr) 
    elf += struct.pack(dw, 0) #e_shentsize
    elf += struct.pack(dw, 0) #e_shnum
    elf += struct.pack(dw, 0) #e_shstrndx

    for (counter,(vaddr, _)) in enumerate(pages):
        #program header
        elf += struct.pack(dd, 1) #loadable
        elf += struct.pack(dd, 5) #flags rx
        elf += struct.pack(dq, (num_phdr)*phdrsize+ehsize+(counter)*page_size) #p_offset
        elf += struct.pack(dq, vaddr) 
        elf += struct.pack(dq, vaddr) 
        elf += struct.pack(dq, page_size) #filesize
        elf += struct.pack(dq, page_size) #mem size
        elf += struct.pack(dq, page_size) #align



    for(_, content) in pages:
        content = content + "\x00"*(page_size-len(content))
        elf += content

    return elf

def export_to_dump(filename_addr, filename_dump, dest_filename, start_addr, end_addr):
  #pages = []
  offset = 0
  pages_map = {}
  f = open(filename_addr, "rb")
  f_dump = open(filename_dump, 'rb')

  min_addr = 0
  max_addr = 0
  addr = f.read(8)
  while addr != "":
    if min_addr == 0 or min_addr > struct.unpack("<Q", addr)[0]:
      min_addr = struct.unpack("<Q", addr)[0]
    if max_addr == 0 or max_addr < struct.unpack("<Q", addr)[0]:
      max_addr = struct.unpack("<Q", addr)[0]

    pages_map[struct.unpack("<Q", addr)[0]] = f_dump.read(0x1000)
    offset += 0x1000
    addr = f.read(8)

  print(pages_map.keys())
  for k in pages_map.keys():
    print(hex(k))

  print(hex(min_addr))
  print(hex(max_addr))

  print("---------")
  f_dump = open(dest_filename, 'wb')
  i = start_addr
  while i <= end_addr:
    if i in pages_map:
      print("%s found in cache"%(hex(i)))
      f_dump.write(pages_map[i])
    else:
      print("padding at %s"%(hex(i)))

      f_dump.write('\x00' * 0x1000)

    i += 0x1000
  

def export_to_elf(filename_addr, filename_dump, dest_filename, x86=False):
  pages = []
  f = open(filename_addr, "rb")
  f_dump = open(filename_dump, 'rb')
  addr = f.read(8)
  while addr != "":
    pages.append((struct.unpack("<Q", addr)[0], f_dump.read(0x1000)))
    addr = f.read(8)
 
  elf_data = to_elf(pages, x86=x86)

  f_elf = open(dest_filename, 'wb')
  f_elf.write(elf_data)
  f_elf.close()


  mode = "x86-64"
  if x86:
    mode = "x86"

  print("[!] Successfully exported " + str(len(elf_data)) + " bytes to ELF " + mode + " file (" + str(dest_filename) + ")!")


def print_page_cache_info(page_cache):
  print("Page Cache: " + str(page_cache))
  print("Pages:\t" + str(os.path.getsize(page_cache + ".addr")/8))
  print("Size:\t" +  hex(os.path.getsize(page_cache + ".dump")))
  print("\n")

def print_addr(addr, offset):
  print(hex(addr) + "\t" + hex(offset)) 

def print_file(filename, sort=False):
  print("Page\t\tOffset")
  print("---------------------------------------")
  offset = 0
  pages = []
  f = open(filename, "rb")
  addr = f.read(8)
  while addr != "":
    pages.append([struct.unpack("<Q", addr)[0], offset])
    offset += 0x1000
    addr = f.read(8)
  
  if sort:
    pages = sorted(pages,key=lambda x: x[0])

  for p in pages:
    print_addr(p[0], p[1])

def dump_page(filename_addr, filename_dump, dest_filename, page):
  f = open(filename_addr, "rb")
  f_dump = open(filename_dump, "rb")
  f_output = open(dest_filename, "wb")
  offset = 0
  addr = f.read(8)
  while addr != "":
    if(struct.unpack('Q', addr)[0] == page):
      f_dump.seek(offset)
      f_output.write(f_dump.read(0x1000))
      print("Page " + hex(page) + " successfully dumped to file " + str(dest_filename) + "!")
      return
    offset += 0x1000
    addr = f.read(8)
  print("Page " + hex(page) + " not found!")

def diff_page_cache_data(page_cache_a, page_cache_b, offset_a, offset_b):
  f1 = open(page_cache_a)
  f1.seek(offset_a)
  data_a = f1.read(0x1000)
  f1.close()

  f2 = open(page_cache_b)
  f2.seek(offset_b)
  data_b = f2.read(0x1000)
  f2.close()

  m1= hashlib.sha256()
  m1.update(data_a)
  result1 = m1.hexdigest()

  m2= hashlib.sha256()
  m2.update(data_b)
  result2 = m2.hexdigest()

  #print(str(result1) + " vs " + str(result2))

  if result1 == result2:
    return True
  return False
  

def diff_page_caches(page_cache_a, page_cache_b):
  pages_a = {}
  offset = 0
  f = open(page_cache_a + ".addr", "rb")
  addr = f.read(8)
  while addr != "":
    pages_a[struct.unpack("<Q", addr)[0]] = offset
    offset += 0x1000
    addr = f.read(8)

  pages_b = {}
  offset = 0
  f = open(page_cache_b + ".addr", "rb")
  addr = f.read(8)
  while addr != "":
    pages_b[struct.unpack("<Q", addr)[0]] = offset
    offset += 0x1000
    addr = f.read(8)  

  error_num = 0

  for i in pages_a.keys():
    if i not in pages_b:
      print("WARNING: Not found " + hex(i))
    else:
      if not diff_page_cache_data(page_cache_a + ".dump", page_cache_b + ".dump", pages_a[i], pages_b[i]):
        print("ERROR: Not matching " + hex(i))
        error_num += 1

  print("\nMatches: " + str(len(pages_a)-error_num) + "/" + str(len(pages_a)))


sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)) + "/../")

import argparse

parser = argparse.ArgumentParser(description='Page Dump Utility')

parser.add_argument('--print_addresses', action="store_true", default=False)
parser.add_argument('--sort_addresses', action="store_true", default=False)

parser.add_argument('--dump_page', action="store", type=lambda x: int(x,0))
parser.add_argument('--output_dump_file', action="store", type=str)

parser.add_argument('--export_to_elf', action="store", type=str)
parser.add_argument('--elf_type_32', action="store_true", default=False)

parser.add_argument('--diff', action="store", type=str)
parser.add_argument('--export_to_dump', action="store", type=str)


parser.add_argument('page_cache', action="store")

parser.add_argument('--start_addr', action="store", type=lambda x: int(x,0))
parser.add_argument('--end_addr', action="store", type=lambda x: int(x,0))


args = parser.parse_args()

if not os.path.isfile(args.page_cache + ".addr"):
  print("Error: " + args.page_cache + ".addr" + " not found")
  sys.exit(1)
if not os.path.isfile(args.page_cache + ".dump"):
  print("Error: " + args.page_cache + ".dump" + " not found")
  sys.exit(1)

print_page_cache_info(args.page_cache)

if(args.diff):
  if not os.path.isfile(args.diff + ".addr"):
    print("Error: " + args.diff + ".addr" + " not found")
    sys.exit(1)
  if not os.path.isfile(args.diff + ".dump"):
    print("Error: " + args.diff + ".dump" + " not found")
    sys.exit(1)
  diff_page_caches(args.page_cache, args.diff)

if(args.print_addresses):
  print_file(args.page_cache + ".addr", sort=args.sort_addresses)

if(args.export_to_elf is not None):
  export_to_elf(args.page_cache + ".addr",  args.page_cache + ".dump", args.export_to_elf, x86=args.elf_type_32)

if(args.export_to_dump is not None):
  if (args.start_addr is not None) and (args.end_addr is not None):
    export_to_dump(args.page_cache + ".addr",  args.page_cache + ".dump", args.export_to_dump, args.start_addr, args.end_addr)
  else:
     print("Error: --start_addr and --end_addr are not defined")


if(args.dump_page is not None):
  if(args.output_dump_file is None):
    print("Error: --output_dump_file not defined")
    sys.exit(1)
  dump_page(args.page_cache + ".addr",  args.page_cache + ".dump", args.output_dump_file, args.dump_page)