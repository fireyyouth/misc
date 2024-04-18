from socket import socket, AF_INET, SOCK_STREAM
from time import sleep

listener = socket(AF_INET, SOCK_STREAM)

listener.bind(('127.0.0.1', 8000))
listener.listen(234)

while True:
    sleep(1)

