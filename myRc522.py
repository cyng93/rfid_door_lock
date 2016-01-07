#!/usr/bin/python

import helper
import serial

ser = serial.Serial('/dev/ttyACM0', baudrate='9600')

cyID="63326DC"
whiteList=list()

while True:
    msg = ser.readline()
    subMsg = msg.split(":")
    if subMsg[0]!="A2R" :
        print msg
    else :
        cmd = subMsg[1]
        readCard = subMsg[2]
        if cmd=="MOD" :
            if readCard in whiteList :
                whiteList.remove(readCard)
            else:
                whiteList.append(readCard)
            print whiteList
            #ser.write("1")
        elif cmd=="ENQ" :
            if readCard in whiteList :
                helper.granted_callback()
                ser.write("1")
                #print("R2A:1:"+readCard+":")
            else :
                helper.denied_callback()
                ser.write("0")
                #print("R2A:0:"+readCard+":")
