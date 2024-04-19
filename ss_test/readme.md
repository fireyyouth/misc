## overview
This experiment explain send-Q and recv-Q in ss output.
server create 2 listen socket with reuseport on, backlog = 234
client create 10 socket and connect to server, then send 5 bytes

## output
- only server up
```
$ ss -atn | grep 8000 | head
LISTEN 0      234        127.0.0.1:8000       0.0.0.0:*
LISTEN 0      234        127.0.0.1:8000       0.0.0.0:*
```
- after client connect
```
$ ss -atn | grep 8000 | head
LISTEN 4      234        127.0.0.1:8000       0.0.0.0:*
LISTEN 6      234        127.0.0.1:8000       0.0.0.0:*
ESTAB  0      0          127.0.0.1:45220    127.0.0.1:8000
ESTAB  5      0          127.0.0.1:8000     127.0.0.1:45232
ESTAB  0      0          127.0.0.1:45222    127.0.0.1:8000
ESTAB  0      0          127.0.0.1:45216    127.0.0.1:8000
ESTAB  0      0          127.0.0.1:45234    127.0.0.1:8000
ESTAB  5      0          127.0.0.1:8000     127.0.0.1:45226
ESTAB  5      0          127.0.0.1:8000     127.0.0.1:45222
ESTAB  5      0          127.0.0.1:8000     127.0.0.1:45220
ESTAB  0      0          127.0.0.1:45228    127.0.0.1:8000
...
```

## conclusion
- for listening socket, recv-Queue stand for connection to be `accept()`, send-Q stand for accept queue capacity(i.e. backlog)
- each listening socket with resueport on has its own accept queue, kernel does load balance
- for each connection socket, recv-Queue stand for bytes to be `read()`
