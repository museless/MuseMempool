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


/*---------------------------------------------
 *            Part One: Define
-*---------------------------------------------*/

#define DEFAULT_BSIZE       0x1000
#define DEFALUT_MAX_BODY    0x10000 


/*---------------------------------------------
 *            Part Two: Typedef
-*---------------------------------------------*/

typedef struct muse_mempool Mempool;

typedef struct chunk        Chunk;
typedef struct block        Block; 


/*---------------------------------------------
 *            Part Three: Struct
-*---------------------------------------------*/

struct muse_mempool {
    Chunk  *chunks;
    Chunk  *current;
    Chunk  *free_chunk;

    Block  *blocks;

    uint    nchunk;
    uint    capablity;
    uint    sizebor;

    MATOS   blockatom, chunkatom;
};

struct chunk {
    void   *start;
    void   *end;

    Chunk  *next_free;

    uint    rest;
    uint    counter;
};

struct block {
    void   *start;
    Block  *fore, *next;
};


/*---------------------------------------------
 *            Part Four: Function
-*---------------------------------------------*/

bool    mmdp_create(Mempool *pool, int border);
void   *mmdp_malloc(Mempool *pool, uint size);
void    mmdp_free(Mempool *pool, void *addr);

void    mmdp_free_pool(Mempool *pool);
void    mmdp_free_handler(DMPH *pMfree);

void    mmdp_reset_default(DMPH *pReset);


