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
    r = int(random() * 6)
    if (r == 0):
        s.send(b"ACCELERATE\n")
    elif (r == 1):
        s.send(b"MINE\n")
    elif (r == 2):
        s.send(b"RIGHT\n")
    elif (r == 3):
        s.send(b"LEFT\n")
    elif (r == 4):
        s.send(b"MISSILE\n")
    elif (r == 5):
        s.send(b"SEEKING\n")
s.close                     # Close the socket when done
