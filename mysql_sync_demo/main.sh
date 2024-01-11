mysql  -u root --protocol tcp --port 3306 < master_init.sql \
    && mysql  -u root --protocol tcp --port 3307 < slave_init.sql \
    && mysql  -u root --protocol tcp --port 3306 < master_data.sql \
    && sleep 5 && mysql  -u root --protocol tcp --port 3307 < slave_query.sql;
