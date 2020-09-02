#!C:python
# coding=utf-8

import re
import socket

ATTRIBUTELISTPATTERN = re.compile(r'''((?:[^,"']|"[^"]*"|'[^']*')+)''')

class ssdpHandler:
    def __init__(self):
        self.ssdpInitialMsg = \
        'M-SEARCH * HTTP/1.1\r\n' \
        'HOST:239.255.255.250:1900\r\n' \
        'ST:upnp:rootdevice\r\n' \
        'MX:2\r\n' \
        'MAN:"ssdp:discover"\r\n' \
        '\r\n'

        self.servers = {}

        return None

    def getSSDP(self):
        # Set up UDP socket
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        s.settimeout(2)
        s.sendto(self.ssdpInitialMsg, ('239.255.255.250', 1900) )

        try:
            while True:
                data, addr = s.recvfrom(65507)
                key = "%s:%d" % (addr[0], addr[1])
                self.servers[key] = self.parseData(data)
        except socket.timeout:
            pass

    def parseData(self, data):
        retData = {}
        for line in data.splitlines():
            if line:
                if (line.startswith("Location:")):
                    params = self.parseExt(line, "Location:")
                    retData['location'] = params[0].strip()

                if (line.startswith("LOCATION:")):
                    params = self.parseExt(line, "LOCATION:")
                    retData['location'] = params[0].strip()

        return retData

    def parseExt(self, line, tag):
        extMedia = line.replace(tag, "")
        extMedia = extMedia.replace("\n", "")

        return ATTRIBUTELISTPATTERN.split(extMedia)[1::2]

