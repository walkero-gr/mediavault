import urllib2

service_type = "urn:schemas-upnp-org:service:ContentDirectory:1"
soap_action = "Browse"

soap_encoding = "http://schemas.xmlsoap.org/soap/encoding/"
soap_env = "http://schemas.xmlsoap.org/soap/envelope/"
#service_ns = "urn:schemas-upnp-org:service:ContentDirectory:1"

soap_body = \
'<?xml version="1.0"?>\n' \
'<s:Envelope xmlns:s="%s" s:encodingStyle="%s">\n' \
'  <s:Body>\n' \
'    <m:%s xmlns:m="%s">\n' \
'      <Result/>\n' \
'    </m:%s>\n' \
'   </s:Body>\n' \
'</s:Envelope>\n' % (soap_env, soap_encoding, soap_action, service_type, soap_action)

print soap_body, "\n"

headers = {
    'POST': "%s HTTP/1.1" % ("/contentdirectory/control"),
    'SOAPAction': '"%s#%s"' % (service_type, soap_action),
    'Host': 'http://192.168.0.92:8096',
    'Content-Type': 'text/xml',
    'Content-Length': len(soap_body),
}

ctrl_url = "http://192.168.0.92:8096/contentdirectory/control"

print headers, "\n"

request = urllib2.Request(ctrl_url, soap_body, headers)
try:
    response = urllib2.urlopen(request)
    print response.read()
except urllib2.HTTPError, e:
    #oap_error_xml = e.read()
    print e

