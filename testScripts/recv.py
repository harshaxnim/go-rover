#!/usr/bin/python

from socket import *
import os

print "Waiting.."

s=socket(AF_INET, SOCK_DGRAM)
s.bind(('',7037))

while 1:
	m=s.recvfrom(1024)
	ip = m[1][0]
	pt = m[1][1]
	print "Found at: ",ip,":",pt
	s.sendto("Shoving it!", (ip, pt))