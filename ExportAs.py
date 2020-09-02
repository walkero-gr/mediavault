#!python
#
#************************************************************
#**
#** Created by: CodeBench 0.27 (07.09.2012)
#**
#** Project: SketchBlock
#**
#** File: ExportAs.py
#**
#** Date: 12-09-2012 01:26:36
#**
#************************************************************
#
#*/

# Export Files to any format via PIL

import sys
import os
import arexx
import tempfile

try:
	from PIL import Image
except ImportError:
	havepil = False
	formats = ['ILBM']
else:
	havepil = True
	Image.init()
	formats = Image.SAVE.keys()
	formats.sort()
	try:
		formats.remove('BUFR')
	except:
		pass
	try:
		formats.remove('FITS')
	except:
		pass
	try:
		formats.remove('GRIB')
	except:
		pass
	try:
		formats.remove('HDF5')
	except:
		pass
	try:
		formats.remove('WMF')
	except:
		pass
			

def saveJPEG(im, savepath):
	im.save(savepath, format="JPEG", quality=95,optimise=1) 

def saveBMP(im, savepath):
	im2 = im.convert('RGB')
	im2.save(savepath, format="BMP")
	del im2

def saveEPS(im, savepath):
	im2 = im.convert('RGB')
	im2.save(savepath, format="EPS")
	del im2
	
def saveMSP(im, savepath):
	im2 = im.convert('1')
	im2.save(savepath, format="MSP")
	del im2
	
def savePALM(im, savepath):
	im2 = im.convert('P')
	im2.save(savepath, format="PALM")
	del im2
		
def savePCX(im, savepath):
	im2 = im.convert('RGB')
	im2.save(savepath, format="PCX")
	del im2

def savePDF(im, savepath):
	im2 = im.convert('RGB')
	im2.save(savepath, format="PDF")
	del im2

def saveXBM(im, savepath):
	im2 = im.convert('1')
	im2.save(savepath, format="XBM")
	del im2

# Makes a ChooserNode list from a list of strings

def MakeCNList(guikey, *strings):
	
	# Create our list object.
	
	(rc,rc2,cnlist) = arexx.dorexx("PROACTION","NEWGUIOBJECT GUIID " +  guikey + " OBJECTTYPE \"GUIOBJ_List\"")
	concstrings = []
	if rc == 0:
		for string in strings:
			if not isinstance(string,list):
				concstrings.append(string)
			else:
				concstrings.extend(string)
		for string in concstrings:
			# Create a node for each string.
			
			cntags = ""
			cntags += "CNA_Text," + str(string) + ","
			cntags += "TAG_DONE"
			(rc,rc2,cn) = arexx.dorexx("PROACTION","ALLOCCHOOSERNODE GUIID " + guikey + " TAGSTRING \"" + cntags + "\"")
			# Add to list.
			if rc == 0:
				arexx.dorexx("PROACTION", "ADDTAIL GUIID " + guikey + " LISTID " + cnlist + " NODEID " + cn )
	return cnlist
	
def FreeCNList(guikey, cnlist):
	
	# Remove eac node in turn and free.
	
	(rc, rc2, cn) = arexx.dorexx("PROACTION","REMTAIL GUIID " + guikey + " LISTID " + cnlist)
	while rc == 0:
		arexx.dorexx("PROACTION","FREECHOOSERNODE GUIID " + guikey + " NODEID " + cn)
		(rc, rc2, cn) = arexx.dorexx("PROACTION","REMTAIL GUIID " + guikey + " LISTID " + cnlist)
		

def HandleInput(pyport,guikey):
	
	global save_gid
	global cancel_gid
	global getfile_gid
	
	result = None
	die = 0
	while die == 0:
		pyport.wait()
		msg = pyport.getmsg()
		if msg:
			cmd = msg.msg
			msg.reply()
			if cmd == "QUIT":
				die = 1
				break 
			elif cmd[:5] == "CLOSE":
				die = 1
				break
			elif cmd[:8] == "GADGETUP":
				# format is
				# GADGETUP GUIID guikey GADGETID gid CODE code
				(dummy,dummy,lguikey,dummy,gid,dummy,code) = cmd.split()
				if gid == save_gid:
					result = save_gid
					die = 1
					break
				elif gid == cancel_gid:
					die = 1
					break
				elif gid == getfile_gid:
					(rc,rc2,chosen) = arexx.dorexx("PROACTION","REQUESTFILE GUIID " + guikey + " OBJECTID " + getfile_gid)
					
                
	return result

def saveFORMAT(savename,savepath):
	if savename == 'ILBM':
		(rc,rc2,dummy) = arexx.dorexx("SKETCHBLOCK","EXPORT FILE \"" + savepath + "\"")
	else:
		(handle, temppath ) = tempfile.mkstemp()
		if handle:
			os.close(handle)
			(rc,rc2,dummy) = arexx.dorexx("SKETCHBLOCK","EXPORT FILE \"" + temppath + "\"")
			im = Image.open(temppath)
			if im:
				if savename=="JPEG":
					saveJPEG(im,savepath)
				elif savename=="BMP":
					saveBMP(im,savepath)
				elif savename=="EPS":
					saveEPS(im,savepath)
				elif savename=="MSP":
					saveMSP(im,savepath)
				elif savename=="PALM":
					savePALM(im,savepath)
				elif savename=="PCX":
					savePCX(im,savepath)
				elif savename=="PDF":
					savePDF(im,savepath)
				elif savename=="XBM":
					saveXBM(im,savepath)
				else:
					im.save(savepath, format=savename)
				del im
			os.unlink(temppath)	
			
# DoGUI does all the work for the GUI 

def DoGUI(pubscreen):
	
	global save_gid
	global cancel_gid
	global getfile_gid
	
	# Setup The ARexx Port for our script
	#
	pyport = arexx.Port("EXPORTAS")
	if pyport:
	
		# First we'll build our window.class tagslist 
		# Python can't use send StemVars at the moment so we'll
		# use the TAGSTRING tecnique throughout.
		
		wintags = ""
		wintags += "WA_Width,100,"
		wintags += "WA_Height,100,"
		wintags += "WA_DragBar,1,"
		wintags += "WA_DepthGadget,1,"
		wintags += "WA_SizeGadget,1,"
		wintags += "WA_CloseGadget,1,"
		wintags += "WA_Title,Export As ...,"
		wintags += "WA_PubScreenFallBack,1,"
		wintags += "WA_PubScreenName," + pubscreen + ","
		wintags += "WINDOW_Position,WPOS_CENTERSCREEN,"
		wintags += "WA_Activate,1,"
		wintags += "TAG_DONE"
	
		layouttags = ""
		layouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_VERT,"	
		layouttags += "TAG_DONE"
		

		savelayouttags = ""
		savelayouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,"		
		savelayouttags += "TAG_DONE" 
		
		getFileTags = ""
		getFileTags += "GETFILE_TitleText,Select Destination,"
		getFileTags += "GETFILE_DoSaveMode,1,"
		getFileTags += "GA_RelVerify,1,"
		getFileTags += "GETFILE_ReadOnly,1,"
		getFileTags += "GETFILE_DoMultiSelect,0,"
		getFileTags += "TAG_DONE"
		
		(rc,rc2,guikey) = arexx.dorexx("PROACTION","CREATEGUI PORTNAME \"EXPORTAS\" TAGSTRING \"" + wintags + "\"")
		
		if rc == 0:
			(rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + layouttags + "\"")
			
			cnlist = MakeCNList(guikey,formats)
			if (cnlist != None):
				(rc,rc2,savetype_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"CHOOSER_Labels," + cnlist + ",CHOOSER_MaxLabels," + str(len(formats)) + ",GA_Disabled,0,TAG_DONE\"")
				
			(rc,rc2,getfile_gid) = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"getfile.gadget\" TAGSTRING \"" + getFileTags + "\"")
			
			(rc,rc2,save_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + savelayouttags + "\"")

			(rc,rc2,save_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_Text,Export,GA_RelVerify,1,TAG_DONE\"") 			
			(rc,rc2,dummy)       = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + save_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + save_gid + ",CHILD_WeightedWidth,0,TAG_DONE\"")
			(rc,rc2,cancel_gid) = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_Text,Cancel,GA_RelVerify,1,TAG_DONE\"") 			
			(rc,rc2,dummy)       = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + save_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + cancel_gid + ",CHILD_WeightedWidth,0,TAG_DONE\"")			
			
			# End of Save / Cancel layout.
			(rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )

			(rc,rc2,dummy)       = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + current_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + save_layout_gid + ",CHILD_WeightedHeight,0,TAG_DONE\"")
						
			# End of top level layout.
			(rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )
			arexx.dorexx("SKETCHBLOCK","SETAPPLICATIONBUSY")
			 															
			(rc,rc2,result)      = arexx.dorexx("PROACTION","OPENGUIWINDOW GUIID " + guikey)

			if HandleInput(pyport,guikey) == save_gid:
				# Do Save
				# Get attributes  
				(rc,rc2,savetype) =  arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + savetype_gid + " TAGNAME \"CHOOSER_Selected\"")
				savename = formats[int(savetype)]
				file = ""
				drawer = ""
				(rc,rc2,file) = arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + getfile_gid + " TAGNAME \"GETFILE_File\"")
				(rc,rc2,drawer) = arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + getfile_gid + " TAGNAME \"GETFILE_Drawer\"")
				if file == "":
					(rc,rc2,chosen) = arexx.dorexx("PROACTION","REQUESTFILE GUIID " + guikey + " OBJECTID " + getfile_gid)	 
					if chosen == "1":
						(rc,rc2,file) = arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + getfile_gid + " TAGNAME \"GETFILE_File\"")
						(rc,rc2,drawer) = arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + getfile_gid + " TAGNAME \"GETFILE_Drawer\"")
				if file != "":
					savepath = os.path.join(drawer,file)
					
					saveFORMAT(savename,savepath)
									
			arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + guikey)
			arexx.dorexx("SKETCHBLOCK","UNSETAPPLICATIONBUSY")
			if(cnlist != None):
				FreeCNList(guikey,cnlist)
				
			arexx.dorexx("PROACTION","DESTROYGUI GUIID " + guikey)
	else:
		os.system('RequestChoice "SaveAsBrush.py" "Couldn\'t Create ARexx Port" "OK"')
		exit()
		
# First we need to check if ProAction is runing and start it if not.
# The python ARexx module lacks a ShowPorts function 
# Borrow it from ARexx
if len(sys.argv) == 3:
	saveFORMAT(sys.argv[1],sys.argv[2])
	exit()
	
(rc,rc2,ports) = arexx.dorexx("REXX","return show('P')")

if rc == 0:
	if -1 == ports.find("PROACTION"):
		# No ProAction start it
		os.system("RUN >\"T:proactionpid\" APPDIR:PROACTION")
		os.system("C:WaitForPort PROACTION")
		# Now check again
		(rc,rc2,ports) = arexx.dorexx("REXX","return show('P')")
		if rc == 0:
			if -1 == ports.find("PROACTION"):
				# Still not there :-(
				os.system('RequestChoice "SaveAsBrush.py" "Unable To Start ProAction GUIServer" "OK"')
				exit()
else:
	os.system('RequestChoice "SaveAsBrush.py" "Couldn\'t Find ARexx!" "OK"')
	exit();
	
# Okay we should be good to go now

# Before calling DoGUI() we need to get the sketcblock public screen

(rc,rc2,pubscreen) = arexx.dorexx("SKETCHBLOCK","GETAPPLICATIONINFO ATTR SCREENNAME")
if rc == 0:
	DoGUI(pubscreen)

