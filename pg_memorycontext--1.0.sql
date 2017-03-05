/* contrib/pg_memorycontext/pg_memorycontext--1.0.sql */

-- complain if script is sourced in psql, rather than via ALTER EXTENSION
\echo Use "ALTER EXTENSION pg_memorycontext" to load this file. \quit

CREATE OR REPLACE FUNCTION pg_memorycontext(
	OUT memorycontextname text, OUT count integer, OUT totalsize integer)
	RETURNS SETOF record
	AS 'MODULE_PATHNAME', 'pg_memorycontext'
	LANGUAGE C IMMUTABLE STRICT;

CREATE VIEW pg_memorycontext AS
	SELECT * FROM pg_memorycontext() ORDER BY totalsize DESC, count DESC;

GRANT SELECT ON pg_memorycontext TO public;
REVOKE all ON FUNCTION pg_memorycontext() FROM public;















