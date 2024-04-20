# lock free rule list
## in-place update version
little overhead, but possibly unexpected behavior
- unexpected behavior example:

worker executing 2
```
1 2 3
  ^
```
manager prepend 4, worker still at 2
```
4 1 2 3
    ^
```
manager append 4, worker still at 2
```
4 1 2 3 4
    ^
```
worker continue match to second 4
```
4 1 2 3 4
        ^
```

*second 4 could never be matched from user's perspective*

## copy-on-write version
no unexpected behavior, but some copy overhead



