#!python
# 
# PAFindTag.py
# Example Script which allows the users to search for a tag and see if it is currently supported.
#

import os
import arexx

def HandleInput(pyport,guikey):
    
    global find_gid
    global tagname_gid
    global tagresult_gid
    die = 0
    while die == 0:
        # wait on our incoming port
        pyport.wait()
        # get the msg (if any)
        msg = pyport.getmsg()
        if msg:
            # it's a real msg get the cmd from it and reply quickly
            cmd = msg.msg
            msg.reply()
            # now process the command.
            if cmd == "QUIT":
                die = 1
                break 
            if cmd[:5] == "CLOSE":
                die = 1
                break
            if cmd[:8] == "GADGETUP":
                # format is
                # GADGETUP GUIKID guikey GADGETID gid CODE code
                (dummy,dummy,lguikey,dummy,gid,dummy,code) = cmd.split()

                # Check the message is from the correct GUI
                # We are only running one in this example, but a script could
                # handle an arbitrary number 
                
                if lguikey == guikey:
                    if gid == find_gid or gid == tagname_gid:
                        (rc,rc2,result) = arexx.dorexx("PROACTION","GETATTR GUIID " + guikey + " OBJECTID " + tagname_gid + " TAGNAME \"STRINGA_TextVal\"")
                        if rc == 0:
                            (rc, rc2, supported) = arexx.dorexx("PROACTION", "GETGUIATTR GUIID " + guikey + " ATTRNAME TAGSUPPORTED \"" + result + "\"")
                            if rc != 0:
                                supported = "Tag: *\"" + result + "*\" Unsupported."
                            else:
                                (tag,type) = supported.split()
                                supported = "Tag: *\"" + tag + "*\" Supported. Type: " + type
                            arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + tagresult_gid + " TAGSTRING \"STRINGA_TextVal," + supported + ",TAG_DONE\"")

                                        

# DoGUI does all the work for the GUI 

def DoGUI(pubscreen):
    # Setup The ARexx Port for our script
    #
    global find_gid
    global tagname_gid
    global tagresult_gid
    

    pyport = arexx.Port("PYPORT")
    if pyport:

        # First we'll build our window.class tagslist 
        # Python can't use send StemVars at the moment so we'll
        # use the TAGSTRING tecnique throughout.
        
        wintags = ""
        wintags += "WA_Width,400,"
        wintags += "WA_Height,10,"
        wintags += "WINDOW_LockHeight,1,"
        wintags += "WA_DragBar,1,"
        wintags += "WA_DepthGadget,1,"
        wintags += "WA_SizeGadget,1,"
        wintags += "WA_CloseGadget,1,"
        wintags += "WA_Title,Find Tag,"
        wintags += "WA_PubScreenFallBack,1,"
        wintags += "WA_PubScreenName," + pubscreen + ","
        wintags += "WINDOW_Position,WPOS_CENTERSCREEN,"
        wintags += "WA_Activate,1,"
        wintags += "TAG_DONE"
    
        layouttags = ""
        layouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_VERT,"    
        layouttags += "TAG_DONE"
        

        
        taglayouttags = ""
        taglayouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,"        
        taglayouttags += "TAG_DONE" 
        
        (rc,rc2,guikey) = arexx.dorexx("PROACTION","CREATEGUI PORTNAME \"PYPORT\" TAGSTRING \"" + wintags + "\"")
        
        if rc == 0:
            (rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + layouttags + "\"")
            

            
            (rc,rc2,tagname_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + taglayouttags + "\"")

            (rc,rc2,tagname_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"STRINGA_TextVal,Enter Tagname here...,GA_RelVerify,1,TAG_DONE\"")             
            (rc,rc2,find_gid)      = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_Text,Find Tag,GA_RelVerify,1,TAG_DONE\"")             
            (rc,rc2,dummy)         = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + tagname_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + find_gid + ",CHILD_WeightedWidth,0,TAG_DONE\"")            
            
            # End of tagname layout.
            (rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )
            # Set it's height to minimum.
            (rc,rc2,dummy)       = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + current_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + tagname_layout_gid + ",CHILD_WeightedHeight,0,TAG_DONE\"")

            (rc,rc2,tagresult_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"GA_ReadOnly,1,TAG_DONE\"")                                     
            # End of top level layout.
            (rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )
                                                                        
            (rc,rc2,result)      = arexx.dorexx("PROACTION","OPENGUIWINDOW GUIID " + guikey)

            HandleInput(pyport,guikey)
            
            arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + guikey)
            arexx.dorexx("PROACTION","DESTROYGUI GUIID " + guikey)
    else:
        os.system('RequestChoice "PAFindTag.py" "Couldn\'t Create ARexx Port" "OK"')
        exit()

        
# The real sequence of event starts here:

# First we need to check if ProAction is runing and start it if not.
# The python ARexx module lacks a ShowPorts function 
# Borrow it from ARexx

(rc,rc2,ports) = arexx.dorexx("REXX","return show('P')")

if rc == 0:
    if -1 == ports.find("PROACTION"):
        # No ProAction start it
        os.system("RUN >NIL: APPDIR:PROACTION")
        os.system("C:WaitForPort PROACTION")
        # Now check again
        (rc,rc2,ports) = arexx.dorexx("REXX","return show('P')")
        if rc == 0:
            if -1 == ports.find("PROACTION"):
                # Still not there :-(
                os.system('RequestChoice "SaveAsBrush.py" "Unable To Start ProAction GUIServer" "OK"')
                exit()

else:

    os.system('RequestChoice "PAFindTag.py" "Couldn\'t Find ARexx!" "OK"')
    exit()

# Okay we should be good to go now

DoGUI("Workbench")


