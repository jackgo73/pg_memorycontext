# pg_memorycontext

The pg_memorycontext.......

The extension is compatible with PostgreSQL 9.2+.

## Installation guide

To install `pg_memorycontext`, execute this in the module's directory:
```shell
make install USE_PGXS=1
```
Modify the **`shared_preload_libraries`** parameter in `postgresql.conf` as following:
```
shared_preload_libraries = 'pg_memorycontext'
```
It is essential to restart the PostgreSQL instance. After that, execute the following query in psql:
```plpgsql
CREATE EXTENSION pg_memorycontext;
```
