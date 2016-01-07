#!/usr/bin/python

import sys
import picamera
import xml.etree.ElementTree as ET
from time import gmtime, strftime

currentTime = strftime("%H:%M:%S", gmtime())
xml_path = "/home/lks/web/sockets/socket.xml"
img_out_path = "/home/lks/web/public/images/"
img_out_format = ".jpg"

tree = ET.parse(xml_path)
root = tree.getroot()

def granted_callback():
    #--1. Modify xml to inform user
    for granted in root.iter('granted'):
        granted.text = '1'

    for filename in root.iter('filename'):
        filename.text = '-1'

    tree.write(xml_path)



def denied_callback():
    #--1. Take photo
    img_filename = "denied_" + currentTime + img_out_format
    with picamera.PiCamera() as camera:
        camera.capture(img_out_path + img_filename)

    #--2. Modify xml to inform user
    #for log in root.iter('log'):
    #    log.text = ''
    #    log.set('updated','yes')

    for granted in root.iter('granted'):
        granted.text = '0'

    for filename in root.iter('filename'):
        filename.text = img_filename

    tree.write(xml_path)

if __name__ == '__main__':
    img_filename = "image_" + currentTime + img_out_format
    with picamera.PiCamera() as camera:
        camera.capture(img_filename)
