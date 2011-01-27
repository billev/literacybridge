# -*- coding: utf-8 -*-
from Tkinter import *
from tkFileDialog import askopenfilename
from metaclass import *
import os
import string

metaFIDnames = { 0: " (Categories)", 1: "(Title)", 2: " (Creator)", 3: " (Subject)", 4: " (Description)", 5: " (Publisher)", 6: " (Contributor)", 7: " (Date)", 8: " (Type)", 9: " (Format)", 10: " (UniqueID)", 11: " (Source)", 12: " (Language)", 13: " (Relation)", 14: " (Coverage)", 15 : " (Rights)", 16: " (Revision)", 17: " (Rev. Date)", 18: " (Rev. Desc.)", 19: " (Copy count)", 20: " (Play Count)", 21: " (Rating)"}
metaCATnames = { 1: " (Agriculture)", 2: " (Health)", 3: " (Education)", 4: " (Stories)", 5: " (Business)", 6: " (Governance)", 7: " (Music)", 8: " (Diary)", 0: " (Other)" }
bNext = 0
curitem = 0
tblist = []
curlistframe = []

def init(F):
    global filenames
    global tblist
    if len(filenames) == 0:
      filenames = askopenfilename(multiple=1,filetypes=[("TB audio files","*.a18")])
#    print type(filename)
    
    if os.name == 'nt':
      # BUG ?? windows returns a string, not a tuple
      name = string.replace(filenames, "/", "\\\\")
      name = name.encode('ascii','ignore')
      filenames = string.split(name, ' ')
    for name in filenames:
      tbc = TB_metadata(name)
      if tbc != None:
	tblist.append(tbc)
    fmetalbl = Frame(F, relief="sunken", border=1)
    l11 = Label(fmetalbl, text="Field ID", width=25)
    l11.grid(row=1, column=0)
    l22 = Label(fmetalbl, text="Value(s)", width=50)
    l22.grid(row=1, column=1)
#    Label(fmetalbl, text="Field ID", relief=RIDGE,  width=25).grid(row=r, column=0)
#    Label(fmetalbl, text="Value(s)",relief=RIDGE, width=50).grid(row=r, column=1)
    fmetalbl.grid(row=1)
    if len(tblist) == 1:
      bNext.configure(state=DISABLED)
      bPrev.configure(state=DISABLED)
    else:
      bNext.configure(state=NORMAL)
      bPrev.configure(state=NORMAL)

#	for item in tblist:
def dispItem(F, num):
    r = 2
    global curitem
    global tblist
    global curlistframe
          
    if num >= len(tblist):
      num = 0
    item = tblist[num]
    x = os.path.split(item.getFileName())
    root.title(x[1])
    md = item.getMetaData()
    if md == None:
 #     print "No metadata"
      return
    for m in md:
      mid = m[0]
      mvalue = m[1]
#	 	print md[0][1][0]
#      print mid
#      print mvalue
      
      try:
	m1 = metaFIDnames[mid]
	m = str(mid) + m1
      except:
	m = mid
      fmeta = Frame(F, relief="sunken", border=1)
      l1 = Label(fmeta, text=m, relief=SUNKEN, width=25)
      l1.grid(row=0, column=0)
      m22 = mvalue[0]
      if mid == 0:  # categories
	if mvalue[0] >= '0' and mvalue[0] < '9':
	  m22 = m22 + metaCATnames[int(mvalue[0])]
      l2 = Label(fmeta, justify=LEFT, text=m22,relief=SUNKEN, width=50)
      l2.grid(row=0, column=1)
      fmeta.grid(row=r)
      r = r + 1
      curlistframe.append(fmeta)
#    print dir(root)
#    print 'maxsize ' + str(root.maxsize())
#    print 'minsize ' + str(root.minsize())
       
def evNext():
  global curitem
  global curlistframe
  for item in curlistframe:
    item.grid_forget()
  curlistframe = []
  curitem = curitem + 1
  if curitem >= len(tblist):
    curitem = 0
  dispItem(F, curitem)


def evPrev():
  global curitem
  global curlistframe
  for item in curlistframe:
    item.grid_forget()
  curlistframe = []
  curitem = curitem - 1
  if curitem < 0:
    curitem = len(tblist) - 1
  dispItem(F, curitem)

def evOpen():
  global tblist
  global filenames
  global curitem
  filenames = askopenfilename(multiple=1,filetypes=[("TB audio files","*.a18")])
  tblist = []
  init(F)
  curitem = 0
  dispItem(F, 0)
    
root=Tk() 
F = Frame(root)
#F.grid(expand="true")
w=root.winfo_screenwidth()
h=root.winfo_screenheight()
root.geometry("600x200+%d+%d" % ( (w-600)/2, (h-200)/2 ) )

fButtons = Frame(F, relief="sunken", border=3)
bOpen = Button(fButtons, text="Open", command=evOpen)
bOpen.grid(row=0, column=0)
bNext = Button(fButtons, text="Next", command=evNext)
bNext.grid(row=0, column=1)
bPrev = Button(fButtons, text="Prev", command=evPrev)
bPrev.grid(row=0, column=2)
bQuit = Button(fButtons, text="Quit", command=F.quit)
bQuit.grid(row=0, column=3)
fButtons.grid(row=0, columnspan=2)

#print os.name
if len(sys.argv) >= 2:
  filenames = sys.argv[1:]
  
init(F)
dispItem(F, curitem)
F.grid()
F.mainloop()
