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

        
        
        

    }
    
}












