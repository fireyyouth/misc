## test connection balance among multiple thread
## supported mode
- multiple listening fd reuseport
- single listening fd shared by all threads
## run server
```
./target/debug/server r # r is optional, means reuseport mode
```

## run client
```
./target/debug/client 1000
```

## result
- reuseport mode
```
 osboxes@osboxes:~/misc/conn_balance$ ./target/debug/server r
 multiple listening fd with reuseport
 0: [0, 0, 0, 0, 0, 0, 0, 0]
 0: [0, 0, 0, 0, 0, 0, 0, 0]
 1000: [123, 137, 118, 119, 124, 138, 124, 117]
 1000: [123, 137, 118, 119, 124, 138, 124, 117]
 1000: [123, 137, 118, 119, 124, 138, 124, 117]
```
- shared fd mode
```
osboxes@osboxes:~/misc/conn_balance$ ./target/debug/server
shared listening fd
0: [0, 0, 0, 0, 0, 0, 0, 0]
629: [167, 173, 46, 75, 47, 37, 42, 42]
1000: [305, 173, 265, 75, 47, 39, 43, 53]
1000: [305, 173, 265, 75, 47, 39, 43, 53]
1000: [305, 173, 265, 75, 47, 39, 43, 53]
1000: [305, 173, 265, 75, 47, 39, 43, 53]
```

## conclusion
reuseport mode is better than shared fd mode for connection balance
