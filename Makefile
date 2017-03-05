[![Build Status](https://travis-ci.org/mutex73/pg_memorycontext.svg?branch=master)](https://travis-ci.org/mutex73/pg_memorycontext)
# contrib/pg_memorycontext/Makefile

MODULE_big = pg_memorycontext
OBJS = pg_memorycontext.o

EXTENSION = pg_memorycontext
DATA = pg_memorycontext--1.0.sql pg_memorycontext--unpackaged--1.0.sql

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_memorycontext
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

