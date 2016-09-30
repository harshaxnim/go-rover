from socket import *
import time

cs = socket(AF_INET, SOCK_DGRAM)
cs.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
cs.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

while (1):
	print "sent"
	cs.sendto('Hi there!', ('255.255.255.255', 7037))
	time.sleep(1)