## theory
hrw will have *1 / node* keys remapped after addition or deletion

## build
```
c++ hrw.cc -lcrypto -o hrw
```
## run
```
$ ./hrw 50 10000 # node num and key num
add miss = 189 : 0.0189
remove miss = 201 : 0.0201
```
