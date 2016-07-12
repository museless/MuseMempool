/*---------------------------------------------
 *     modification time: 2016-07-11 15:23:03
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
 *       Part Four:  Mempool port
 *       Part Five:  Block malloc
 *       Part Six:   Block search
 *
-*---------------------------------------------*/

/*---------------------------------------------
 *             Part Zero: Include
-*---------------------------------------------*/

#include "mmdpool.h"
#include "satomic.h"


/*---------------------------------------------
 *             Part One: Define
-*---------------------------------------------*/

#define mmdp_set(hStru) { \
    hStru->mh_big = NULL; \
    hStru->mh_stru = hStru->mh_select = NULL; \
    hStru->mh_cnt = 0; \
}


#define mmdp_block_end_adrr(pMpool) \
    (pMpool->mb_start + pMpool->mb_size)


#define mmdp_reset_block(pHandler) { \
    pHandler->mb_end = pHandler->mb_start; \
    pHandler->mb_left = pHandler->mb_size; \
    pHandler->mb_taker = 0; \
}


/*---------------------------------------------
 *            Part Two: Local data
-*---------------------------------------------*/

static  void   *_block_alloc(Mempool *pool, uint size);
static  void   *_chunk_alloc(Mempool *hStru, msize_t nMalloc);

static  Block  *_block_search(Block *hStru, void *pFind);
static  Chunk  *_chunk_size_search(Mempool *pool, msize_t nSize);
static  Chunk  *_chunk_search(Chunk *begBlock, void *pLoct);


/*---------------------------------------------
 *          Part Four: Mempool port
 *
 *          1. mmdp_create
 *          2. mmdp_malloc
 *          3. mmdp_free
 *          4. mmdp_free_pool
 *          5. mmdp_free_handler
 *          6. mmdp_free_all
 *          7. mmdp_reset_default
 *
-*---------------------------------------------*/

/*-----mmdp_create-----*/
bool mmdp_create(Musepool *pool, int border)
{
    if (!pool) {
        errno = EINVAL;
        return  false;
    }

    mmdp_set(pool);

    pool->sizebor = (border < DEFAULT_BSIZE) ? 
        DEFAULT_BSIZE : (((border >>  1) + ((border % 2) ? 1 : 0)) << 1);

    mato_init(pool->chunkatom, 1);
    mato_init(pool->blockatom, 1);

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
void mmdp_free(Mempool *pHandler, void *pFree)
{
    Block   *pBig, **bigPoint;
    Chunk    *pBlock;

    if ((pBig = _block_search(pHandler->mh_big, pFree))) {
        mato_lock(pHandler->blockatom);

        bigPoint = (pBig == pHandler->mh_big) ? &pHandler->mh_big : &pBig->mbb_fore;
        *bigPoint = pBig->mbb_next;

        if (pBig->mbb_fore)
            pBig->mbb_fore->mbb_next = pBig->mbb_next;

        mato_unlock(pHandler->blockatom);
        free(pBig);

        return;
    }

    if ((pBlock = _chunk_search(pHandler->mh_stru, pFree))) {
        mato_lock(pHandler->defato);

        /* when no taker to take this block */
        if (!(--pBlock->mb_taker)) {
            mmdp_reset_block(pBlock);
            
            /* link it to the select list */
            pBlock->mb_nselec = pHandler->mh_select;
            pHandler->mh_select = pBlock;
        }
        
        mato_unlock(pHandler->defato);
    }
}


/*-----mmdp_free_pool-----*/
void mmdp_free_pool(Mempool *pMfree)
{
    /* release smallchunk */
    Chunk    *pBnext, *pBmov;

    for (pBmov = pMfree->mh_stru; pBmov; pBmov = pBnext) {
        pBnext = pBmov->mb_next;
        free(pBmov);
    }

    /* release bigchunk */
    Block   *bigNext, *bigMov;
    
    for (bigMov = pMfree->mh_big; bigMov; bigMov = bigNext) {
        bigNext = bigMov->mbb_next;
        free(bigMov);
    }

    mmdp_set(pMfree);
}


/*-----mmdp_free_handler-----*/
void mmdp_free_handler(Mempool *pMfree)
{
    free(pMfree);
}


/*-----mmdp_free_all-----*/
void mmdp_free_all(Mempool *pMfree)
{
    mmdp_free_pool(pMfree);
    mmdp_free_handler(pMfree);
}


/*-----mmdp_reset_default-----*/
void mmdp_reset_default(Mempool *pReset)
{
    Chunk    *pMov;

    pReset->mh_select = NULL;
    
    for (pMov = pReset->mh_stru; pMov; pMov = pMov->mb_next) {
        mmdp_reset_block(pMov);
        
        pMov->mb_nselec = pReset->mh_select;
        pReset->mh_select = pMov;
    }
}


/*---------------------------------------------
 *          Part Five: Block malloc
 *
 *          1. _block_alloc
 *          2. _chunk_alloc
 *
-*---------------------------------------------*/

/*-----_block_alloc-----*/
void *_block_alloc(Mempool *pool, uint size)
{
    Block   *block;

    if (!(addr = (Block *)malloc(sizeof(Block) + size)))
        return  NULL;

    block->start = (void *)((char *)block + sizeof(Block));
    block->next = block->fore = NULL;

    mato_lock(pool->blockatom);

    if (pool->blocks)
        pool->blocks->fore = block;

    block->next = pool->chunks;
    pool->chunks = block;

    mato_unlock(pool->blockatom);

    return  block->start;
}


/*-----_chunk_alloc-----*/
static void *_chunk_alloc(Mempool *hStru, msize_t nMalloc)
{
    Chunk    *pBody;
    void   *pRet;

    if (!(pBody = _chunk_size_search(hStru, nMalloc))) {
        if (!(pBody = (Chunk *)malloc(sizeof(Chunk) + hStru->mh_sizebor)))
            return  NULL;
        
        /* make start point and end point link to memory chunk */
        pBody->mb_start = pBody->mb_end = (void *)((char *)pBody + sizeof(Chunk));
        
        pBody->mb_left = pBody->mb_size = hStru->mh_sizebor;
        pBody->mb_taker = 0;
        
        /* link to stru list */
        pBody->mb_next = hStru->mh_stru;
        hStru->mh_stru = pBody;

        /* link to select list */
        pBody->mb_nselec = hStru->mh_select;
        hStru->mh_select = pBody;
        
        hStru->mh_cnt++;
    }

    pRet = pBody->mb_end;
    pBody->mb_end += nMalloc;
    pBody->mb_left -= nMalloc;
    pBody->mb_taker++;

    return  pRet;
}


/*---------------------------------------------
 *          Part Six: Block search
 *
 *          1. _block_search
 *          2. _chunk_size_search
 *          3. _chunk_search
 *
-*---------------------------------------------*/

/*-----_block_search-----*/
Block *_block_search(Block *block, void *pFind)
{
    while (hStru) {
        if (hStru->start == pFind)
            return  hStru;

        hStru = hStru->mbb_next;
    }

    return  NULL;
}


/*-----_chunk_size_search-----*/
Chunk *_chunk_size_search(Mempool *pool, msize_t nSize)
{
    Chunk    **pForward = &pool->mh_select;
    
    while (*pForward) {
        if ((*pForward)->mb_left >= nSize)
            return  (*pForward);
        
        *pForward = (*pForward)->mb_nselec;
    }

    return  NULL;
}


/*-----_chunk_search-----*/
Chunk *_chunk_search(Chunk *begBlock, void *pLoct)
{
    for (; begBlock; begBlock = begBlock->mb_next) {
        if (pLoct >= begBlock->mb_start && pLoct <= mmdp_block_end_adrr(begBlock))
            return  begBlock;
    }

    return  NULL;
}

