# libmodsecurity usage example
- depedency
```
modsecurity v3.0.4 development file (tested on debian 11)
```

- build
```
make
```

- run
```
./demo
```

- test positive
```
$ curl localhost:8080/hi -H 'foo: <script>'   -v
*   Trying ::1:8080...
* connect to ::1 port 8080 failed: Connection refused
*   Trying 127.0.0.1:8080...
* Connected to localhost (127.0.0.1) port 8080 (#0)
> GET /hi HTTP/1.1
> Host: localhost:8080
> User-Agent: curl/7.74.0
> Accept: */*
> foo: <script>
>
* Mark bundle as not supporting multiuse
< HTTP/1.1 403 Forbidden
< Content-Length: 0
< Keep-Alive: timeout=5, max=5
<
* Connection #0 to host localhost left intact
```

- test negative
```
$ curl  localhost:8080/hi -H 'foo: <script'   -v
*   Trying ::1:8080...
* connect to ::1 port 8080 failed: Connection refused
*   Trying 127.0.0.1:8080...
* Connected to localhost (127.0.0.1) port 8080 (#0)
> GET /hi HTTP/1.1
> Host: localhost:8080
> User-Agent: curl/7.74.0
> Accept: */*
> foo: <script
>
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Length: 12
< Content-Type: text/plain
< Keep-Alive: timeout=5, max=5
<
* Connection #0 to host localhost left intact
Hello World!
```

- note
1. rules.conf is copied from [ModSecurity repo](https://github.com/owasp-modsecurity/ModSecurity/blob/v3/master/examples/simple_example_using_c/basic_rules.conf), plus one simple xss rule
1. the file `modsec_audit.log` is there so that rules.conf can be correctly loaded
