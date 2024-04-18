from socket import socket, AF_INET, SOCK_STREAM
from time import sleep

a = []
for _ in range(10):
    client = socket(AF_INET, SOCK_STREAM)
    client.connect(('127.0.0.1', 8000))
    a.append(client)

for client in a:
    client.send(b'hello')

while True:
    sleep(1)

