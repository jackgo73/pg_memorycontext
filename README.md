[![Build Status](https://travis-ci.org/mutex73/pg_memorycontext.svg?branch=master)](https://travis-ci.org/mutex73/pg_memorycontext)

# pg_memorycontext

The pg_memorycontext module provides views to get postgresql memory context histogram.

The extension is compatible with PostgreSQL 9.2+.
## Overview
After loading the pg_memorycontext module， a view named pg_memorycontext is built.You can get memorycontext statistics by selecting views."totalsize" means the total size(byte) of same memorycontext name and "count" means the times it appearance.
For example:
```plpgsql
postgres=# select * from pg_memorycontext;
            memorycontextname            | count | totalsize(byte)
-----------------------------------------+-------+-----------
 CacheMemoryContext                      |     1 |    516096
 Timezones                               |     1 |    104120
 TopMemoryContext                        |     1 |     95880
 MessageContext                          |     1 |     65536
 WAL record construction                 |     1 |     49768
 TupleSort main                          |     1 |     32824
 TransactionAbortContext                 |     1 |     32768
 Relcache by OID                         |     1 |     24576
 ExecutorState                           |     1 |     24576
 smgr relation table                     |     1 |     24576
 pg_memorycontext hash table             |     1 |     24576
 Record information cache                |     1 |     24576
 CFuncHash                               |     1 |      8192
 PrivateRefCount                         |     1 |      8192
 TopTransactionContext                   |     1 |      8192
 ErrorContext                            |     1 |      8192
 Portal hash                             |     1 |      8192
 PortalMemory                            |     1 |      8192
 Operator class cache                    |     1 |      8192
 LOCALLOCK hash                          |     1 |      8192
 MdSmgr
```
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
