/*---------------------------------------------
 *     modification time: 2016-10-28 09:30:21
 *     mender: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *     file: example.c
 *     creation time: 2016-07-12 18:29:21
 *     author: Muse
-*---------------------------------------------*/

#include "mempool.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


/* define */
#define TIME_DIFF(s, e) \
    ((e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec))

#define LOG_MEMORY(log, addr, idx) \
    do { \
        log[idx].ptr = addr; \
        log[idx].is_free = false; \
    } while (0)

#define TIMES   0xC000

#define ALLOC(addr, alloctor, freer, saver, timer) \
    do { \
        gettimeofday(&start, NULL); \
\
        if (!(ptr = alloctor(addr, size))) \
            perror("malloc"); \
\
        if (need_free && !saver[find].is_free) { \
            saver[find].is_free = true; \
            freer(saver[find].ptr); \
        } \
\
        gettimeofday(&end, NULL); \
\
        timer += TIME_DIFF(start, end); \
        LOG_MEMORY(saver, ptr, idx); \
\
    } while (0)


/* typedef */
typedef struct memlog {
    char   *ptr;
    bool    is_free;

} Memlog;


/* functions */
static void mmdp_free_wrap(void *addr);


/* data */
static Memlog   mallocSave[TIMES], museSave[TIMES];
static Mempool  pool;


int main(void)
{
    if (!mmdp_create(&pool, 0x8000)) {
        perror("mmdp_create");
        return  -1;
    }

    void __attribute__ ((unused))  *ptr;
    bool __attribute__ ((unused))   need_free = false;

    struct timeval  end, start;
    unsigned long   tma = 0, tmm = 0;
    int32_t         size = 0, find = 0;

    srandom(time(NULL));

    for (int idx = 0; idx < TIMES; idx++) {
        size = random() % 10240 + 1;

        if (idx > 0) {
            need_free = (size < 2048);
            find = random() % idx;
        }

        ALLOC(NULL, realloc, free, mallocSave, tma);
        ALLOC(&pool, mmdp_malloc, mmdp_free_wrap, museSave, tmm);

        need_free = false;
    }

    printf("Chunk: %d\n", pool.nchunk);
    printf("Malloc: %ld us - Musepool: %ld us\n", tma, tmm);

    mmdp_free_pool(&pool);

    return  -1;
}


void mmdp_free_wrap(void *addr)
{
    mmdp_free(&pool, addr);
}

