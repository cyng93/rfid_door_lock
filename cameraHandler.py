#!/usr/bin/python
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#|R|a|s|p|b|e|r|r|y|P|i|.|c|o|m|.|t|w|
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
# Copyright (c) 2014, raspberrypi.com.tw
# All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author : sosorry
# Date   : 11/14/2014
# Capture an image to a file

from time import gmtime, strftime
import picamera

currentTime = strftime("%H:%M:%S", gmtime())
outputFormat = ".jpg"
filename = "image_"+currentTime+".jpg"

def takePhoto():
    filename = "denied_"+currentTime+outputFormat
    with picamera.PiCamera() as camera:
        camera.capture(filename)

if __name__ == '__main__':
    filename = "image_"+currentTime+outputFormat
    with picamera.PiCamera() as camera:
        camera.start_preview()
        # Camera warm-up time
        #time.sleep(2)
        # The default resolution is 1280x800
        camera.capture(filename)
