#!python
import os, sys
import arexx

class guiHandler:
    def __init__(self):
        self.AppName = "MediaVault"
        self.defScreenName = "Workbench"

        self.pyport = None
        self.guikey = None

        return None

    def checkProaction(self):
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
                        os.system('C:RequestChoice "' + self.AppName + '" "Unable To Start ProAction GUIServer" "OK"')
                        sys.exit()

        else:
            os.system('C:RequestChoice "' + self.AppName + '" "Couldn\'t Find ARexx!" "OK"')
            sys.exit()

    def HandleInput(self, pyport, guikey):
        global find_gid
        global tagname_gid
        global tagresult_gid
        die = 0

        #self.setStatusMessage("MediaVault started")

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
                                #arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + tagresult_gid + " TAGSTRING \"STRINGA_TextVal," + supported + ",TAG_DONE\"")

    def setStatusMessage(self, message):
        arexx.dorexx("PROACTION", "SETATTRS GUIID " + self.guikey + " OBJECTID " + status_gid + " TAGSTRING \"STRINGA_TextVal," + message + ",TAG_DONE\"")
        return None

    def addListItem(self, node):
        print node
        arexx.dorexx("PROACTION", 'ADDLISTBROWSERNODE GUIID ' + self.guikey + ' OBJECTID ' + itemslist_gid + ' TAIL TAGSTRING "' + node + 'TAG_DONE"')
        #'ADDLISTBROWSERNODE GUIID ' || bgui || ' OBJECTID ' || blist || ' TAIL TAGSTEM ' || lbntags

    def freeList(self):
        arexx.dorexx("PROACTION", "FREELISTBROWSERLIST GUIID " + self.guikey + " LISTID " + objlist_gid)

    def createGUI(self):
        # Setup The ARexx Port for our script
        #
        #global find_gid
        #global tagname_gid
        global status_gid
        global itemslist_gid
        global itemslist_CI_gid
        global objlist_gid

        pyport = arexx.Port(self.AppName)
        self.pyport = pyport
        if pyport:

            # First we'll build our window.class tagslist 
            # Python can't use send StemVars at the moment so we'll
            # use the TAGSTRING tecnique throughout.

            wintags = ""
            wintags += "WA_Width,400,"
            wintags += "WA_Height,650,"
            wintags += "WINDOW_LockHeight,0,"
            wintags += "WA_DragBar,1,"
            wintags += "WA_DepthGadget,1,"
            wintags += "WA_SizeGadget,1,"
            wintags += "WA_CloseGadget,1,"
            wintags += "WA_Title," + self.AppName + ","
            wintags += "WA_PubScreenFallBack,1,"
            wintags += "WA_PubScreenName," + self.defScreenName + ","
            wintags += "WINDOW_Position,WPOS_CENTERSCREEN,"
            wintags += "WA_Activate,1,"
            wintags += "TAG_DONE"

            layouttags = ""
            layouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_VERT,"
            layouttags += "TAG_DONE"

            taglayouttags = ""
            taglayouttags += "LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,"
            taglayouttags += "TAG_DONE"

            columnInfoTags = ""
            columnInfoTags += "LBCIA_Column,0,"
            columnInfoTags += "LBCIA_Title,Title,"
            columnInfoTags += "LBCIA_Weight,80,"
            columnInfoTags += "LBCIA_Column,1,"
            columnInfoTags += "LBCIA_Title,Type,"
            columnInfoTags += "LBCIA_Weight,20,"
            columnInfoTags += "LBCIA_DraggableSeparator,1,"
            #columnInfoTags += "TAG_DONE"

            listBrowserTags = ""
            listBrowserTags += "GA_RelVerify,1,"
            listBrowserTags += "LISTBROWSER_AutoFit,FAIL,"
            listBrowserTags += "LISTBROWSER_ColumnTitles,1,"
            #listBrowserTags += "LISTBROWSER_ColumnInfo, 0,"
            #listBrowserTags += "LISTBROWSER_Labels, 0,"
            listBrowserTags += "LISTBROWSER_HorizontalProp,FAIL,"
            listBrowserTags += "LISTBROWSER_ShowSelected,1,"
            listBrowserTags += "LISTBROWSER_Striping,LBS_ROWS,"



            (rc,rc2,guikey) = arexx.dorexx(
                    "PROACTION",
                    "CREATEGUI PORTNAME \"" + self.AppName + "\" TAGSTRING \"" + wintags + "\"")
            self.guikey = guikey
            
            if rc == 0:
                (rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + layouttags + "\"")

                (rc,rc2,itemslist_CI_gid) = arexx.dorexx("PROACTION",
                        'ALLOCLISTBROWSERCOLUMNINFO GUIID ' + self.guikey + ' COLUMNS 2 TAGSTRING "' + columnInfoTags + 'TAG_DONE"')
                listBrowserTags += "LISTBROWSER_ColumnInfo, " + itemslist_CI_gid + ","

                (rc,rc2,objlist_gid) = arexx.dorexx("PROACTION",
                        'NEWGUIOBJECT GUIID ' + self.guikey + ' OBJECTTYPE "GUIOBJ_List"')
                listBrowserTags += "LISTBROWSER_Labels," + objlist_gid + ","
                
                (rc,rc2,itemslist_gid) = arexx.dorexx("PROACTION",
                        'ADDGADGET GUIID ' + self.guikey + ' GADGETCLASS "listbrowser.gadget" TAGSTRING "' + listBrowserTags + 'TAG_DONE"')

                #(rc,rc2,tagname_layout_gid) = arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"" + taglayouttags + "\"")

                #(rc,rc2,tagname_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"STRINGA_TextVal,Enter Tagname here...,GA_RelVerify,1,TAG_DONE\"")
                #(rc,rc2,find_gid)      = arexx.dorexx(
                #        "PROACTION",
                #        "ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_Text,Find Tag,GA_RelVerify,1,TAG_DONE\"")

                #(rc,rc2,dummy)         = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + tagname_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + find_gid + ",CHILD_WeightedWidth,0,TAG_DONE\"")

                # End of tagname layout.
                #(rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )
                # Set it's height to minimum.
                #(rc,rc2,dummy)       = arexx.dorexx("PROACTION","SETATTRS GUIID " + guikey + " OBJECTID " + current_layout_gid + " TAGSTRING \"LAYOUT_ModifyChild," + tagname_layout_gid + ",CHILD_WeightedHeight,0,TAG_DONE\"")

                (rc,rc2,status_gid)   = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"GA_ReadOnly,1,TAG_DONE\"")
                # End of top level layout.
                (rc,rc2,current_layout_gid) = arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey )

                (rc,rc2,result)      = arexx.dorexx("PROACTION","OPENGUIWINDOW GUIID " + guikey)

                #self.HandleInput(pyport,guikey)

                #arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + guikey)
                #arexx.dorexx("PROACTION","DESTROYGUI GUIID " + guikey)
                #self.closeGUI(guikey)
        else:
            os.system('RequestChoice "' + self.AppName + '" "Couldn\'t Create ARexx Port" "OK"')
            exit()


    def closeGUI(self, guikey):
        arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + guikey)
        arexx.dorexx("PROACTION","DESTROYGUI GUIID " + guikey)
