## generate certificate and private key
```
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout foo.key -out foo.crt
```
## run server
```
./main --certfile foo.crt --keyfile foo.key
```
