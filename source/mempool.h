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

#include <errno.h>

#include <sys/types.h>

#include "satomic.h"


/*---------------------------------------------
 *            Part One: Define
-*---------------------------------------------*/

#define DEF_CHUNKSIZE   0x1000
#define DEF_MAX_BODY    0x10000 

#define DEF_CAP         0x4


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
    Chunk **chunks;
    Chunk  *free_chunk;
    Chunk  *current;

    Block  *blocks;
    Block  *last;

    uint    nchunk;
    uint    capablity;

    uint    nblock;

    uint    sizebor;

    MATOS   blockatom, chunkatom;
};

struct chunk {
    Chunk  *next_free;

    void   *start;

    uint    rest;
    uint    counter;
};

#define PCHUNK_LEN  sizeof(Chunk *)


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

