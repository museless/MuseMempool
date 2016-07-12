/*---------------------------------------------
 *     modification time: 2016.07.12 18:25
 *     mender: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *     file: mempool.h 
 *     creation time: 2016.07.12 18:25
 *     author: Muse 
-*---------------------------------------------*/

/*---------------------------------------------
 *       Source file content Five part
 *
 *       Part Zero:  Include
 *       Part One:   Define 
 *       Part Two:   Typedef
 *       Part Three: Struct
 *
 *       Part Four:  Function
 *
-*---------------------------------------------*/

/*---------------------------------------------
 *             Part Zero: Include
-*---------------------------------------------*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mmdperr.h"
#include "ato.h"


/*---------------------------------------------
 *            Part One: Define
-*---------------------------------------------*/

#define DEFAULT_BSIZE       0x1000
#define DEFALUT_MAX_BODY    0x10000 


/*---------------------------------------------
 *            Part Two: Typedef
-*---------------------------------------------*/

typedef struct  dmp_handler DMPH;
typedef struct  dmp_body    DMPB;
typedef struct  dmp_bigbody DMPBB;

typedef unsigned char   mpt_t;
typedef unsigned short  merr_t;


/*---------------------------------------------
 *            Part Three: Struct
-*---------------------------------------------*/

struct dmp_handler {
    DMPB       *mh_stru;       /* list to all small chunk */
    DMPB       *mh_select;     /* select list */

    DMPBB      *mh_big;        /* big chunk head */

    uint32_t    mh_sizebor;    /* size border */

    uint32_t    mh_cnt;        /* cnt for pool smaller than DEFAULT_BSIZE */
    MATOS       bigato, defato;
};

struct dmp_body {
    mpt_t      *mb_start;
    mpt_t      *mb_end;

    DMPB       *mb_next;
    DMPB       *mb_nselec;

    uint32_t    mb_size;
    uint32_t    mb_left;
    uint32_t    mb_taker;
};

struct dmp_bigbody {
    mpt_t   *mbb_start;

    DMPBB   *mbb_fore;  /* point to forward */
    DMPBB   *mbb_next;  /* point to next */
};


/*---------------------------------------------
 *            Part Four: Function
-*---------------------------------------------*/

DMPH   *mmdp_create(int borderSize);
void   *mmdp_malloc(DMPH *mHand, uint32_t maSize);
void    mmdp_free(DMPH *pHandler, mpt_t *pFree);
void    mmdp_free_pool(DMPH *pMfree);
void    mmdp_free_handler(DMPH *pMfree);
void    mmdp_free_all(DMPH *pMfree);
void    mmdp_reset_default(DMPH *pReset);
int     mmdp_show_size(DMPH *pMp);


