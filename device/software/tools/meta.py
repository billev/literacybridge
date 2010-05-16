# -*- coding: utf-8 -*-
import os
import sys
import struct

#print sys.argv[1:]
categories = ["Other", "Agriculture", "Health", "Education", "Stories", "Business", "Governance", "Music", "Diary"]

if len(sys.argv) == 1:
  print "usage: python " + sys.argv[0] + " .a18 file names\n" 
  print "  for example: python " + sys.argv[0] + " f1.a18 f2.a18 ...\n"

for fn in sys.argv[1:]:
  print ' '
  f = os.open(fn, os.O_RDONLY)
  alen = os.read(f, 4)
  value = struct.unpack('<i4', alen)
  pos = os.lseek(f, value[0] + 4, os.SEEK_SET)
  print fn + "has " + str(value[0]) + " audio bytes"
#  print pos
  metaver = alen = os.read(f, 4)
  value = struct.unpack('<i4', metaver)
  print "  metedata version=" + str(value[0])
  numf = os.read(f, 4)
  value = struct.unpack('<i4', numf)
  numfields = value[0]
  print "  num fields=" + str(numfields)
  
  myfieldIdx = 0
  while myfieldIdx < numfields:
    fid = os.read(f, 2)
    value = struct.unpack('<h2', fid)
    field_id = value[0]
    print "\n    field id=" + str(field_id)
    
    flen = os.read(f, 4)
    field_len = struct.unpack('<i4', flen)[0]
    print "      field length=" + str(field_len)
    
    nfv = os.read(f, 1)
    num_f_values = struct.unpack('<b', nfv)
    num_field_values = num_f_values[0]
    print "      num field values=" + str(num_field_values)
    
    myvalueIdx = 0
    while myvalueIdx < num_field_values:
      fln = os.read(f, 2)
      value = struct.unpack('<h2', fln)
      fvalue_len = value[0]
      print "      field value length=" + str(fvalue_len)
      fvalue = os.read(f, fvalue_len)
      print    "      field value[" + str(myvalueIdx) + ']=' + str(fvalue)
      myvalueIdx = myvalueIdx + 1
      
  
    myfieldIdx = myfieldIdx + 1
    
  os.close(f)
    