/*---------------------------------------------
 *     modification time: 2016-07-13 17:05:03
 *     mender: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *     file: mempool.c
 *     creation time: 2016-07-11 15:23:03
 *     author: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *       Source file content Seven part
 *
 *       Part Zero:  Include
 *       Part One:   Define 
 *       Part Two:   Local data
 *       Part Three: Local function
 *
 *       Part Four:  Mempool api
 *       Part Five:  Block malloc
 *       Part Six:   Block search
 *       Part Seven: Chunks control
 *
-*---------------------------------------------*/

/*---------------------------------------------
 *             Part Zero: Include
-*---------------------------------------------*/

#include "mempool.h"


/*---------------------------------------------
 *             Part One: Define
-*---------------------------------------------*/

#define _pool_set(pool) { \
    (pool)->free_chunk = pool->current = NULL; \
    (pool)->blocks = pool->last = NULL; \
    (pool)->nchunk = 0; \
    (pool)->capablity = DEF_CAP; \
    mato_init((pool)->chunkatom, 1); \
    mato_init((pool)->blockatom, 1); \
}


#define _chunk_reset(chunk, border) { \
    (chunk)->start = (char *)chunk + sizeof(Chunk); \
    (chunk)->next_free = NULL; \
    (chunk)->rest = border; \
    (chunk)->counter = 0; \
}


/*---------------------------------------------
 *            Part Two: Local data
-*---------------------------------------------*/

static void    *_block_alloc(Mempool *pool, uint size);

static void    *_chunk_alloc(Mempool *pool, uint size);
static void    *_chunk_divide(Chunk *chunk, uint size, uint border);
static void    *_chunk_new(Mempool *pool, uint size);

static Block   *_block_search(Block *block, void *addr);

static void     _chunk_record(Mempool *pool, void *chunk);


/*---------------------------------------------
 *          Part Four: Mempool api 
 *
 *          1. mmdp_create
 *          2. mmdp_malloc
 *          3. mmdp_free
 *          4. mmdp_free_pool
 *          6. mmdp_free_all
 *          7. mmdp_reset_chunk
 *
-*---------------------------------------------*/

/*-----mmdp_create-----*/
bool mmdp_create(Mempool *pool, int border)
{
    if (!pool) {
        errno = EINVAL;
        return  false;
    }

    if (!(pool->chunks = calloc(DEF_CAP, PCHUNK_LEN)))
        return  false;

    pool->sizebor = (border < DEF_CHUNKSIZE) ? DEF_CHUNKSIZE : border;
    _pool_set(pool);

    return  true;
}


/*-----mmdp_malloc-----*/
void *mmdp_malloc(Mempool *pool, uint size)
{
    if (!pool || size <= 0) {
        errno = EINVAL;
        return  NULL;
    }

    if (size > pool->sizebor)
        return  _block_alloc(pool, size);

    void   *addr;

    mato_lock(pool->chunkatom);
    addr = _chunk_alloc(pool, size);
    mato_unlock(pool->chunkatom);

    return  addr;
}


/*-----mmdp_free-----*/
void mmdp_free(Mempool *pool, void *add)
{
}


/*-----mmdp_free_pool-----*/
void mmdp_free_pool(Mempool *pool)
{
    _pool_set(pool);
}


/*-----mmdp_free_all-----*/
void mmdp_free_all(Mempool *pool)
{
    mmdp_free_pool(pool);
}


/*-----mmdp_reset_chunk-----*/
void mmdp_reset_chunk(Mempool *pool)
{
    pool->free_chunk = NULL;

    for (uint index = 0; index < pool->nchunk; index++) {
        Chunk  *chunk = pool->chunks[index];

        _chunk_reset(chunk, pool->sizebor);

        chunk->next_free = pool->free_chunk;
        pool->free_chunk = chunk->next_free;
    }
}


/*---------------------------------------------
 *          Part Five: Block malloc
 *
 *          1. _block_alloc
 *          2. _chunk_alloc
 *          3. _chunk_divide
 *          4. _chunk_new
 *
-*---------------------------------------------*/

/*-----_block_alloc-----*/
void *_block_alloc(Mempool *pool, uint size)
{
    Block  *block;

    if (!(block = (Block *)malloc(sizeof(Block) + size)))
        return  NULL;

    block->start = (void *)((char *)block + sizeof(Block));
    block->next = block->fore = NULL;

    mato_lock(pool->blockatom);

    if (pool->blocks)
        pool->blocks->fore = block;

    block->next = pool->blocks;
    pool->blocks = block;

    mato_unlock(pool->blockatom);

    return  block->start;
}


/*-----_chunk_alloc-----*/
void *_chunk_alloc(Mempool *pool, uint size)
{
    if (!pool->current && pool->free_chunk) {
        pool->current = pool->free_chunk;
        pool->free_chunk = pool->current->next_free;
        pool->current->next_free = NULL;
    }

    if (pool->current && pool->current->rest >= size)
        return  _chunk_divide(pool->current, size, pool->sizebor);

    return  _chunk_new(pool, size);
}


/*-----_chunk_divide-----*/
void *_chunk_divide(Chunk *chunk, uint size, uint border)
{
    void   *addr = (char *)chunk->start + (border - chunk->rest);

    chunk->rest -= size;
    chunk->counter += 1;

    return  addr;
}


/*-----_chunk_new-----*/
void *_chunk_new(Mempool *pool, uint size)
{
    if (pool->nchunk == pool->capablity) {
        uint    new_cap;

        if (!__builtin_mul_overflow(pool->capablity, 2, &new_cap)) {
            errno = ERANGE;
            return  NULL;
        }

        if (!(pool->chunks = realloc(pool->chunks, new_cap * PCHUNK_LEN)))
            return  NULL;

        memset(pool->chunks + pool->capablity, 0, pool->capablity * PCHUNK_LEN);
        pool->capablity = new_cap;
    }

    uint    border = pool->sizebor;
    Chunk  *new_chunk = malloc(sizeof(Chunk) + border);

    if (!new_chunk)
        return  NULL;

    pool->current = new_chunk;

    _chunk_reset(new_chunk, border);
    _chunk_record(pool, new_chunk);

    return  _chunk_divide(new_chunk, size, border);
}


/*---------------------------------------------
 *          Part Six: Block search
 *
 *          1. _block_search
 *
-*---------------------------------------------*/

/*-----_block_search-----*/
Block *_block_search(Block *block, void *addr)
{
    for (; block; block = block->next) {
        if (block->start == addr)
            return  block;
    }

    return  NULL;
}


/*---------------------------------------------
 *          Part Seven: Chunk control
 *
 *          1. _chunk_record 
 *
-*---------------------------------------------*/

/*-----_chunk_record-----*/
void _chunk_record(Mempool *pool, void *chunk)
{
    uint    index = pool->nchunk;

    pool->nchunk += 1;

    if (!pool->chunks[0]) {
        pool->chunks[0] = chunk;
        return;
    }

    if ((char *)chunk > (char *)pool->chunks[index - 1]) {
        pool->chunks[index] = chunk;
        return;
    }

    uint    offset = 0;

    for (; offset < index; offset++) {
        Chunk **this = pool->chunks;

        if ((char *)chunk < (char *)this[offset]) {
            memmove(&this[offset + 1], &this[offset], (index - offset) * PCHUNK_LEN);
            pool->chunks[offset] = chunk;
            return;
        }
    }
}

