/* contrib/pg_memorycontext/pg_memorycontext--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_memorycontext" to load this file. \quit

ALTER EXTENSION pg_memorycontext ADD function pg_memorycontext();
ALTER EXTENSION pg_memorycontext ADD view pg_memorycontext;
















