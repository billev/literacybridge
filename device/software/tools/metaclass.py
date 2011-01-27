# -*- coding: utf-8 -*-
import os
import sys
from stat import *


import struct

class TB_metadata:
  def __init__(self, filename):
    self.debug = 1
    self.filename = filename
    mode = os.stat(filename)[ST_MODE]
    if not S_ISREG(mode):
      return None
    fsize = os.path.getsize(filename)
    f = os.open(filename, os.O_RDONLY)
    alen = os.read(f, 4)
    value = struct.unpack('<i4', alen)
    pos = os.lseek(f, value[0] + 4, os.SEEK_SET)
    if self.debug != 0:
      print filename + " has " + str(value[0]) + " audio bytes"
    if value[0] > fsize:
      return None
    self.metadata_offset = value[0] + 4
    metaver = os.read(f, 4)
    print str(metaver)
    value = struct.unpack('<i4', metaver)
    self.metaversion = str(value[0])
    if self.debug != 0:
      print "  metadata version=" + str(value[0])
    numf = os.read(f, 4)
    value = struct.unpack('<i4', numf)
    self.numfields = value[0]
    if self.debug != 0:
      print "  num fields=" + str(self.numfields)
    self.metafields = []
    
    myfieldIdx = 0
    while myfieldIdx < self.numfields:
      fid = os.read(f, 2)
      value = struct.unpack('<h2', fid)
      field_id = value[0]
      if self.debug != 0:
	print "\n    field id=" + str(field_id)
      
      flen = os.read(f, 4)
      field_len = struct.unpack('<i4', flen)[0]
      if self.debug != 0:
	print "      field length=" + str(field_len)
      
      nfv = os.read(f, 1)
      num_f_values = struct.unpack('<b', nfv)
      num_field_values = num_f_values[0]
      if self.debug != 0:
	print "      num field values=" + str(num_field_values)
      
      myvalueIdx = 0
      vlist = []
      while myvalueIdx < num_field_values:
	fln = os.read(f, 2)
	value = struct.unpack('<h2', fln)
	fvalue_len = value[0]
	if self.debug != 0:
	  print "      field value length=" + str(fvalue_len)
	fvalue = os.read(f, fvalue_len)
	if self.debug != 0:
	  print    "      field value[" + str(myvalueIdx) + ']=' + str(fvalue)
	myvalueIdx = myvalueIdx + 1
	vlist.append(str(fvalue))
	
      newmetafield = field_id, vlist
      self.metafields.append(newmetafield)
      myfieldIdx = myfieldIdx + 1
    
    os.close(f)
    for f in self.metafields:
      if self.debug != 0:
	print f

  def getMetaData(self):
    try:
      return self.metafields
    except:
      return None
    
  def getFileName(self):
    return self.filename
    
  def setDebug(self, value):
    self.debug = value
    

