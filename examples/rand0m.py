#!/usr/bin/python           # This is server.py file

import socket               # Import socket module
from random import random
from time import sleep

s = socket.socket()         # Create a socket object
host = socket.gethostname() # Get local machine name
port = 54321                # Reserve a port for your service.

s.connect((host, port))
s.send(b"NAME rand0m\n")

while True:
    sleep(0.1)
    r = int(random() * 4)
    if (r == 0):
        s.send(b"up\n")
    elif (r == 1):
        s.send(b"down\n")
    elif (r == 2):
        s.send(b"right\n")
    elif (r == 3):
        s.send(b"left\n")

s.close                     # Close the socket when done
