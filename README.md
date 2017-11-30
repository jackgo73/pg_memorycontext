[![Build Status](https://travis-ci.org/mutex73/pg_memorycontext.svg?branch=master)](https://travis-ci.org/mutex73/pg_memorycontext)

# pg_memorycontext
 

 插件提供了postgresql的内存上下文直方图。
 
The extension is compatible with PostgreSQL 9.2+.
## Overview
插件安装后，在数据库中创建一个视图，视图含义：
| 列名 | 含义 | 
| --------- |------| 
| totalsize | the total size(byte) of the context |
| count |  内存上下文树中该context出现的次数 |


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

To install `pg_memorycontext`, execute in the module's directory:
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

## Feedback

Do not hesitate to post your issues, questions and new ideas at the issues page.

## Authors

Jack Gao <jackgo73@outlook.com> Huawei, China
