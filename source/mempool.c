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

#define mmdp_handler_set_empty(hStru) { \
    hStru->mh_big = NULL; \
    hStru->mh_stru = hStru->mh_select = NULL; \
    hStru->mh_err = MMDP_NO_ERROR; \
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

static  mpt_t   *mmdp_big_malloc(DMPH *hPoint, msize_t maSize);
static  mpt_t   *mmdp_default_malloc(DMPH *hStru, msize_t nMalloc);

static  DMPBB   *mmdp_big_search(DMPBB *hStru, mpt_t *pFind);
static  DMPB    *mmdp_default_size_search(DMPH *pMph, msize_t nSize);
static  DMPB    *mmdp_default_block_search(DMPB *begBlock, mpt_t *pLoct);


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
 *          8. mmdp_show_size
 *
-*---------------------------------------------*/

/*-----mmdp_create-----*/
DMPH *mmdp_create(int borderSize)
{
    DMPH   *pMph;
    int     nRem;

    if (!(pMph = (DMPH *)malloc(sizeof(DMPH))))
        return  NULL;

    mmdp_handler_set_empty(pMph);

    nRem = (borderSize >> 1) ? 1 : 0;
    pMph->mh_sizebor = (borderSize < DEFAULT_BSIZE) ? DEFAULT_BSIZE : 
            (((borderSize >>  1) + nRem) << 1);

    mato_init(&pMph->mh_biglock, 1);
    mato_init(&pMph->mh_deflock, 1);

    return  pMph;
}


/*-----mmdp_malloc-----*/
void *mmdp_malloc(DMPH *mHand, msize_t maSize)
{
    mpt_t   *pMem;

    if (maSize > mHand->mh_sizebor)
        return  mmdp_big_malloc(mHand, maSize);

    mato_lock(mHand->mh_deflock);
    pMem = mmdp_default_malloc(mHand, maSize);
    mato_unlock(mHand->mh_deflock);

    return  pMem;
}


/*-----mmdp_free-----*/
void mmdp_free(DMPH *pHandler, mpt_t *pFree)
{
    DMPBB   *pBig, **bigPoint;
    DMPB    *pBlock;

    if ((pBig = mmdp_big_search(pHandler->mh_big, pFree))) {
        mato_lock(pHandler->mh_biglock);

        bigPoint = (pBig == pHandler->mh_big) ? &pHandler->mh_big : &pBig->mbb_fore;
        *bigPoint = pBig->mbb_next;

        if (pBig->mbb_fore)
            pBig->mbb_fore->mbb_next = pBig->mbb_next;

        mato_unlock(pHandler->mh_biglock);
        free(pBig);

        return;
    }

    if ((pBlock = mmdp_default_block_search(pHandler->mh_stru, pFree))) {
        mato_lock(pHandler->mh_deflock);

        /* when no taker to take this block */
        if (!(--pBlock->mb_taker)) {
            mmdp_reset_block(pBlock);
            
            /* link it to the select list */
            pBlock->mb_nselec = pHandler->mh_select;
            pHandler->mh_select = pBlock;
        }
        
        mato_unlock(pHandler->mh_deflock);
    }
}


/*-----mmdp_free_pool-----*/
void mmdp_free_pool(DMPH *pMfree)
{
    /* release smallchunk */
    DMPB    *pBnext, *pBmov;

    for (pBmov = pMfree->mh_stru; pBmov; pBmov = pBnext) {
        pBnext = pBmov->mb_next;
        free(pBmov);
    }

    /* release bigchunk */
    DMPBB   *bigNext, *bigMov;
    
    for (bigMov = pMfree->mh_big; bigMov; bigMov = bigNext) {
        bigNext = bigMov->mbb_next;
        free(bigMov);
    }

    mmdp_handler_set_empty(pMfree);
}


/*-----mmdp_free_handler-----*/
void mmdp_free_handler(DMPH *pMfree)
{
    free(pMfree);
}


/*-----mmdp_free_all-----*/
void mmdp_free_all(DMPH *pMfree)
{
    mmdp_free_pool(pMfree);
    mmdp_free_handler(pMfree);
}


/*-----mmdp_reset_default-----*/
void mmdp_reset_default(DMPH *pReset)
{
    DMPB    *pMov;

    pReset->mh_select = NULL;
    
    for (pMov = pReset->mh_stru; pMov; pMov = pMov->mb_next) {
        mmdp_reset_block(pMov);
        
        pMov->mb_nselec = pReset->mh_select;
        pReset->mh_select = pMov;
    }
}


/*-----mmdp_show_size-----*/
int mmdp_show_size(DMPH *pMp)
{
    return  pMp->mh_sizebor;
}


/*---------------------------------------------
 *          Part Five: Block malloc
 *
 *          1. mmdp_big_malloc
 *          2. mmdp_default_malloc
 *
-*---------------------------------------------*/

/*-----mmdp_big_malloc-----*/
static mpt_t *mmdp_big_malloc(DMPH *hPoint, msize_t maSize)
{
    DMPBB   *bigStru;

    if ((bigStru = (DMPBB *)malloc(sizeof(DMPBB) + maSize)) == NULL)
        return  NULL;

    bigStru->mbb_start = (mpt_t *)((char *)bigStru + sizeof(DMPBB));
    bigStru->mbb_next = NULL;

    mato_lock(hPoint->mh_biglock);

    bigStru->mbb_fore = NULL;

    if (hPoint->mh_big)
        hPoint->mh_big->mbb_fore = bigStru;

    bigStru->mbb_next = hPoint->mh_big;
    hPoint->mh_big = bigStru;

    mato_unlock(hPoint->mh_biglock);

    return  bigStru->mbb_start;
}


/*-----mmdp_default_malloc-----*/
static mpt_t *mmdp_default_malloc(DMPH *hStru, msize_t nMalloc)
{
    DMPB    *pBody;
    mpt_t   *pRet;

    if (!(pBody = mmdp_default_size_search(hStru, nMalloc))) {
        if (!(pBody = (DMPB *)malloc(sizeof(DMPB) + hStru->mh_sizebor)))
            return  NULL;
        
        /* make start point and end point link to memory chunk */
        pBody->mb_start = pBody->mb_end = (mpt_t *)((char *)pBody + sizeof(DMPB));
        
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
 *          1. mmdp_big_search
 *          2. mmdp_default_size_search
 *          3. mmdp_default_block_search
 *
-*---------------------------------------------*/

/*-----mmdp_big_search-----*/
DMPBB *mmdp_big_search(DMPBB *hStru, mpt_t *pFind)
{
    while (hStru) {
        if (hStru->mbb_start == pFind)
            return  hStru;

        hStru = hStru->mbb_next;
    }

    return  NULL;
}


/*-----mmdp_default_size_search-----*/
DMPB *mmdp_default_size_search(DMPH *pMph, msize_t nSize)
{
    DMPB    **pForward = &pMph->mh_select;
    
    while (*pForward) {
        if ((*pForward)->mb_left >= nSize)
            return  (*pForward);
        
        *pForward = (*pForward)->mb_nselec;
    }

    return  NULL;
}


/*-----mmdp_default_block_search-----*/
DMPB *mmdp_default_block_search(DMPB *begBlock, mpt_t *pLoct)
{
    for (; begBlock; begBlock = begBlock->mb_next) {
        if (pLoct >= begBlock->mb_start && pLoct <= mmdp_block_end_adrr(begBlock))
            return  begBlock;
    }

    return  NULL;
}

