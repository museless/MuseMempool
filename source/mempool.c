/*---------------------------------------------
 *     modification time: 2016-10-28 09:25:00
 *     mender: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *     file: mempool.c
 *     creation time: 2016-07-11 15:23:03
 *     author: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *       Source file content Eight part
 *
 *       Part Zero:  Include
 *       Part One:   Define 
 *       Part Two:   Local data
 *       Part Three: Local function
 *
 *       Part Four:  Mempool api
 *       Part Five:  Memory control 
 *       Part Six:   Memory search
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
    (pool)->nchunk = 0, (pool)->nblock = 0; \
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


#define _addr_in_chunk(chunk, addr, border) \
    (((addr) >= (chunk)->start && (addr) - (chunk)->start < border))


/*---------------------------------------------
 *          Part Three: Local function
-*---------------------------------------------*/

static void    *_block_alloc(Mempool *pool, uint size);

static void    *_chunk_alloc(Mempool *pool, uint size);
static void    *_chunk_divide(Chunk *chunk, uint size, uint border);
static void    *_chunk_new(Mempool *pool, uint size);
static bool     _block_free(Mempool *pool, void *addr);

static Block   *_block_search(Block *block, void *addr);
static Chunk   *_chunk_search(Mempool *pool, void *addr);

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
void mmdp_free(Mempool *pool, void *addr)
{
    if (!pool)
        return;

    bool    result = false;

    mato_lock(pool->blockatom);
    result = _block_free(pool, addr);
    mato_unlock(pool->blockatom);

    if (!result && pool->nchunk > 0) {
        Chunk  *chunk;

        mato_lock(pool->chunkatom);

        if ((chunk = _chunk_search(pool, addr))) {
            if (!(chunk->counter -= 1)) {
                if (chunk != pool->current) {
                    chunk->next_free = pool->free_chunk;
                    pool->free_chunk = chunk;

                } else {
                    chunk->rest = pool->sizebor;
                }
            }
        }

        mato_unlock(pool->chunkatom);
    }
}


/*-----mmdp_free_pool-----*/
void mmdp_free_pool(Mempool *pool)
{
    if (!pool)
        return;

    Block  *next, *block = pool->blocks;

    while (block) {
        next = block->next;
        free(block);
        block = next;
    }

    for (int idx = 0; idx < pool->nchunk; idx++)
        free(pool->chunks[idx]);

    free(pool->chunks);

    _pool_set(pool);
}


/*-----mmdp_reset_chunk-----*/
void mmdp_reset_chunk(Mempool *pool)
{
    if (!pool)
        return;

    pool->free_chunk = NULL;

    for (uint index = 0; index < pool->nchunk; index++) {
        Chunk  *chunk = pool->chunks[index];

        _chunk_reset(chunk, pool->sizebor);

        chunk->next_free = pool->free_chunk;
        pool->free_chunk = chunk->next_free;
    }
}


/*---------------------------------------------
 *          Part Five: Memory control
 *
 *          1. _block_alloc
 *          2. _chunk_alloc
 *          3. _chunk_divide
 *          4. _chunk_new
 *          5. _block_free
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
    pool->nblock += 1;

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

        if (__builtin_mul_overflow(pool->capablity, 2, &new_cap)) {
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


/*-----_block_free-----*/
bool _block_free(Mempool *pool, void *addr)
{
    Block  *block = _block_search(pool->blocks, addr);

    if (!block)
        return  false;

    if (block->fore)
        block->fore->next = block->next;

    if (block->next)
        block->next->fore = block->fore;

    if (block == pool->blocks)
        pool->blocks = block->next;

    pool->nblock -= 1;
    free(block);

    return  true;
}


/*---------------------------------------------
 *          Part Six: Memory search
 *
 *          1. _block_search
 *          2. _chunk_search
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


/*-----_chunk_search-----*/
Chunk *_chunk_search(Mempool *pool, void *addr)
{
    Chunk **chunks = pool->chunks;
    uint    head = 0, tail = pool->nchunk - 1, mid = ((head + tail) >> 1);
    uint    border = pool->sizebor;

    if ((Chunk *)addr < chunks[head] || 
        (char *)addr > (char *)chunks[tail]->start + border)
        return  NULL;

    if (tail == 0)
        return  (_addr_in_chunk(chunks[0], addr, border)) ? chunks[0] : NULL; 

    if (_addr_in_chunk(chunks[tail], addr, border))
        return  chunks[tail];

    while (tail > head) {
        if (_addr_in_chunk(chunks[mid], addr, border))
            return  chunks[mid];

        if ((char *)addr < (char *)chunks[mid]) {
            tail = mid;

        } else if ((char *)addr > (char *)(chunks[mid]->start + border)) {
            head = mid + 1;

        } else {
            break;
        }

        mid = (head + tail) >> 1;
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

    for (uint offset = 0; offset < index; offset++) {
        Chunk **this = pool->chunks;

        if ((char *)chunk < (char *)this[offset]) {
            memmove(&this[offset + 1], 
                &this[offset], (index - offset) * PCHUNK_LEN);

            pool->chunks[offset] = chunk;
            return;
        }
    }
}

