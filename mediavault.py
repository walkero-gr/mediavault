#!C:python
# coding=utf-8

import sys, re, urllib, urllib2
import os
import arexx
import xml.parsers.expat
import gui, ssdp
import xmltodict

ATTRIBUTELISTPATTERN = re.compile(r'''((?:[^,"']|"[^"]*"|'[^']*')+)''')

class urlHandler:
    def __init__(self):  
        return None

    def getURL(self, url):
        request = urllib2.Request(url)
        try:
            response = urllib2.urlopen(request)
            retData = response.read()
            response.close()
            return retData
        except URLError, e:
            print e

        return None

    def getXml(self, url):
        response = self.getURL(url)
        return response

class helpersHandler:
    def clearEntryName(self, value):
        return value.replace(",", "")

def main(argv):
    ssdpH = ssdp.ssdpHandler()
    urlH = urlHandler()
    guiH = gui.guiHandler()
    helpers = helpersHandler()



    guiH.checkProaction()

    guiH.createGUI()
    
    #print guiH.guikey
    #print guiH.pyport
    
    guiH.setStatusMessage("Getting the available DLNA Servers")

    # Get the available Servers
    ssdpH.getSSDP()
    availableServers = ssdpH.servers
    if availableServers:
        print availableServers
        print "########################################"
        print "\n\n"
        guiH.setStatusMessage("DLNA Servers found")
        guiH.freeList()
        
        # Get more info for each server like name and url           
        for server in availableServers:               
            print "~~~~~~~~~~~~~~~~~~~~~~~"
            print availableServers[server]['location']
            print "\n"

            xmlResp = urlH.getXml(availableServers[server]['location'])
            xmlRespDict = xmltodict.parse(xmlResp)


            print xmlRespDict['root']['device']['modelName']
            print xmlRespDict['root']['device']['friendlyName']
            print helpers.clearEntryName(xmlRespDict['root']['device']['friendlyName'])
            print xmlRespDict['root']['device']['deviceType']

            availableServers[server]['device'] = xmlRespDict['root']['device']
            try:
                availableServers[server]['baseurl'] = xmlRespDict['root']['URLBase']
            except KeyError, e:
                availableServers[server]['baseurl'] = "http://" + server
                pass

            # TODO: Add the available servers to the list
            nodeTag = ""
            nodeTag += "LBNA_Column,0,"
            nodeTag += "LBNCA_HorizJustify,LCJ_LEFT,"
            nodeTag += "LBNCA_Text, " + helpers.clearEntryName(xmlRespDict['root']['device']['friendlyName']) + ","
            nodeTag += "LBNA_Column,1,"
            nodeTag += "LBNCA_HorizJustify,LCJ_LEFT,"
            nodeTag += "LBNCA_Text,Media Server,"
            nodeTag += "LBNA_UserData,0,"

            guiH.addListItem(nodeTag)

        #print availableServers
        #print "\n\n"

    

    guiH.HandleInput(guiH.pyport,guiH.guikey)

    guiH.closeGUI(guiH.guikey)

    #print guiH.pyport
    #print "GUIKEY: %s" % (guikey)

    sys.exit()

if __name__ == "__main__":
    main(sys.argv[1:])
