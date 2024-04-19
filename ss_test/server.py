from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEPORT
from time import sleep

a = []
for i in range(2):
    listener = socket(AF_INET, SOCK_STREAM)

    listener.setsockopt(SOL_SOCKET, SO_REUSEPORT, 1)
    listener.bind(('127.0.0.1', 8000))
    listener.listen(234)

    a.append(listener)

while True:
    sleep(1)

