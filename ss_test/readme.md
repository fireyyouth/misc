## overview
This experiment explain send-Q and recv-Q in ss output.
server listen to a port with backlog = 234
client create 10 socket and connect to server, then send 5 bytes

## output
- only server up
```
$ ss -atn | grep 8000 | head
LISTEN 0      234        127.0.0.1:8000       0.0.0.0:*
```
- after client connect
```
$ ss -atn | grep 8000 | head
LISTEN 10     234        127.0.0.1:8000       0.0.0.0:*
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
- for each connection socket, recv-Queue stand for bytes to be `read()`
