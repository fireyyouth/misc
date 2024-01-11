## how to run
```
docker-compose up;
# wait mysqld ready
sh +x main.sh
```

## expected output
```
osboxes@osboxes:~/misc/mysql_sync_demo$
osboxes@osboxes:~/misc/mysql_sync_demo$ sh -x main.sh mysql -u root --protocol tcp --port 3306
+ mysql -u root --protocol tcp --port 3307
+ mysql -u root --protocol tcp --port 3306
+ sleep 5
+ mysql -u root --protocol tcp --port 3307
id
1
2
3
99
```
