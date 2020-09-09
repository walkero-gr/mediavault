#!C:python
# coding=utf-8

import sys, re, urllib, urllib2
import os
# import arexx
import xml.parsers.expat
# import gui
import ssdp
import xmltodict
import upnpclient

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
        except urllib2.URLError, e:
            print e
            pass

        return None

    def getXml(self, url):
        response = self.getURL(url)
        return response

class helpersHandler:
    def clearEntryName(self, value):
        return value.replace(",", "")

def main(argv):
    # ssdpH = ssdp.ssdpHandler()
    # urlH = urlHandler()
    # guiH = gui.guiHandler()
    # helpers = helpersHandler()


    # ssdp = upnpclient.SSDP(wait_time=5)
    # servers = ssdp.discover()

    # for server in servers:
    #     print server.friendly_name, '@', server.location
    #     for service in server.services:
    #         print "   ", service.service_id, service.service_type




    # server = upnpclient.Server('http://192.168.0.92:8096/dlna/83cc3fd68b6b4dafa611971bdc8a2914/description.xml') # Jellyfin
    # server = upnpclient.Server('http://192.168.0.90:9000/TMSDeviceDescription.xml') # Twonky
    server = upnpclient.Server('http://192.168.0.90:32469/DeviceDescription.xml') # Plex Media Server

    action = server.find_action('GetSortCapabilities')
    response = action.call()
    print response
    sys.exit()

    # action = server.find_action('Browse')
    # response = action.call({
    #     'ObjectID': '',
    #     'StartingIndex': 0,
    #     'BrowseFlag': 'BrowseDirectChildren',
    #     'Filter': '',
    #     'RequestedCount': 10,
    #     'SortCriteria': ''
    # })
    # print response
    # sys.exit()

    xmlRespDict = xmltodict.parse(response['Result'])
    # print xmlRespDict
    for container in xmlRespDict['DIDL-Lite']['container']:
        # print container
        print container['@id']
        print container['@parentID']
        print container['dc:title']
        # print container['upnp:class']
        # print container['upnp:icon']
        # print container['upnp:albumArtURI']['#text']
        print "\n"
    
    sys.exit()



    # guiH.checkProaction()

    # guiH.createGUI()
    
    #print guiH.guikey
    #print guiH.pyport
    
    # guiH.setStatusMessage("Getting the available DLNA Servers")

    # Get the available Servers
    ssdpH.getSSDP()
    availableServers = ssdpH.servers
    if availableServers:
        print availableServers
        print "########################################"
        print "\n\n"
        # guiH.setStatusMessage("DLNA Servers found")
        # guiH.freeList()
        
        # Get more info for each server like name and url           
        for server in availableServers:               
            print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
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

            # guiH.addListItem(nodeTag)

        #print availableServers
        #print "\n\n"

    

    # guiH.HandleInput(guiH.pyport,guiH.guikey)

    # guiH.closeGUI(guiH.guikey)

    #print guiH.pyport
    #print "GUIKEY: %s" % (guikey)

    sys.exit()

if __name__ == "__main__":
    main(sys.argv[1:])
