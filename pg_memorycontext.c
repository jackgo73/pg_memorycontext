#include "postgres.h"

#include "executor/executor.h"
#include "funcapi.h"
#include "utils/memutils.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#define MXT_NAME_LENGTH 128

/* 1024 different memory context at most */
#define MXT_NAME_NUMBER 1024

#define ALLOCSET_NUM_FREELISTS 11

typedef struct AllocBlockData *AllocBlock;
typedef struct AllocChunkData *AllocChunk;
typedef AllocSetContext *AllocSet;

typedef void *AllocPointer;

typedef struct MxtStat
{
    char name[MXT_NAME_LENGTH];
    unsigned int cnt;
    long total_size;
}MxtStat;

typedef struct AllocBlockData
{
	AllocSet	aset;
	AllocBlock	next;
	char	   *freeptr;
	char	   *endptr;
}AllocBlockData;

typedef struct AllocChunkData
{
	void	   *aset;
	Size		size;
}AllocChunkData;

typedef struct AllocSetContext
{
	MemoryContextData header;
	AllocBlock	blocks;
	AllocChunk	freelist[ALLOCSET_NUM_FREELISTS];
	Size		initBlockSize;
	Size		maxBlockSize;
	Size		nextBlockSize;
	Size		allocChunkLimit;
	AllocBlock	keeper;
} AllocSetContext;


void _PG_init(void);
void _PG_fini(void);
Datum pg_memorycontext(PG_FUNCTION_ARGS);
void MxtCacheInitialize(void);
void MxtCacheInsert(char *name, long totalspace);
long MxtAllocSetStats(MemoryContext context);
void MxtMemoryContextStats(MemoryContext context);
void MxtMemoryContextStatsInternal(MemoryContext context);

static HTAB *MxtCache;


void _PG_init(void)
{

}

void _PG_fini(void)
{

}

PG_FUNCTION_INFO_V1(pg_memorycontext);

Datum pg_memorycontext(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;
    int             call_cntr;
    int             max_calls;
    TupleDesc       tupdesc;
    AttInMetadata   *attinmeta;

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext   oldcontext;
        long            mxt_number;
        HASH_SEQ_STATUS *mxt_status;

        funcctx = SRF_FIRSTCALL_INIT();

        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        MxtCacheInitialize();
        MxtMemoryContextStats(TopMemoryContext);

        mxt_number = hash_get_num_entries(MxtCache);
        funcctx->max_calls = mxt_number;

        mxt_status = (HASH_SEQ_STATUS*)palloc(sizeof(HASH_SEQ_STATUS));
        hash_seq_init(mxt_status, MxtCache);
        funcctx->user_fctx = (void*)mxt_status;

        if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        {
            ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                    errmsg("function returning record called in context"
                        "that cannot accept type record")));
        }

        attinmeta = TupleDescGetAttInMetadata(tupdesc);
        funcctx->attinmeta = attinmeta;

        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();

    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    attinmeta = funcctx->attinmeta;

    if (call_cntr < max_calls)
    {
        char                **values;
        HeapTuple           tuple;
        Datum               result = 0;
        MxtStat             *mxt_stat;
        HASH_SEQ_STATUS     *user_mxt_status;

        user_mxt_status = (HASH_SEQ_STATUS*)funcctx->user_fctx;
        mxt_stat = (MxtStat*)hash_seq_search(user_mxt_status);

        if (mxt_stat)
        {
            /* int take up 12 char at most */
            values = (char**)palloc(3 * sizeof(char*));
            values[0] = (char*)palloc(MXT_NAME_LENGTH * sizeof(char));
            values[1] = (char*)palloc(16 * sizeof(char));
            values[2] = (char*)palloc(16 * sizeof(char));

            snprintf(values[0], MXT_NAME_LENGTH, "%s", mxt_stat->name);
            snprintf(values[1], 16, "%d", mxt_stat->cnt);
            snprintf(values[2], 16, "%ld", mxt_stat->total_size);

            tuple = BuildTupleFromCStrings(attinmeta, values);
            result = HeapTupleGetDatum(tuple);

            pfree(values[0]);
            pfree(values[1]);
            pfree(values[2]);
            pfree(values);       
        }
        SRF_RETURN_NEXT(funcctx, result);
    }
    else
    {
        /* finish the whole seq_search */
        if (hash_seq_search((HASH_SEQ_STATUS*)funcctx->user_fctx)
        {
            ereport(ERROR, 
                "pg_memorycontext: leaked scan hash table")
        }
        SRF_RETURN_DONE(funcctx);
    }
}


void MxtCacheInitialize(void)
{
    HASHCTL mxt_ctl;

    MemSet(&mxt_ctl, 0, sizeof(mxt_ctl));

    /*
     * create memorycontext hashtable under SRF multi-call context
     *
     */

    mxt_ctl.keysize = MXT_NAME_LENGTH;
    mxt_ctl.entrysize = sizeof(MxtStat);
    mxt_ctl.hash = string_hash;
    mxt_ctl.hcxt = CurrentMemoryContext;
    MxtCache = hash_create("pg_memorycontext hash table", MXT_NAME_NUMBER,
                            &mxt_ctl, HASH_ELEM | HASH_FUNCTION| HASH_CONTEXT);

    if (NULL == MxtCache)
    {
        ereport(ERROR, 
            "pg_memorycontext: can not create pg_memorycontext hash table");
    }
}


void MxtCacheInsert(char * name,long totalspace)
{
    MxtStat *mxt_stat;
    bool    found;

    mxt_stat = (MxtStat*)hash_search(MxtCache, (void*)name, HASH_ENTER, &found);

    if(found)
    {
        mxt_stat->cnt += 1;
        mxt_stat->total_size += totalspace;  
    }
    else
    {
        mxt_stat->cnt = 0;
        mxt_stat->total_size = 0;
    }
}

long MxtAllocSetStats(MemoryContext context)
{
    AllocSet    set = (AllocSet)context;
    long        totalspace = 0;
    AllocBlock  block;

    for (block = set->blocks; block != NULL; block = block->next)
    {
        totalspace += block->endptr - ((char*)block);
    }

    MxtCacheInsert(set->header.name, totalspace);

    return totalspace;
}








