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

#define TIMES   0x4000


/* typedef */
typedef struct memlog {
    char   *ptr;
    bool    is_free;

} Memlog;


/* function */
static void log_memory(Memlog *log, char *addr);


/* data */
static Memlog   mallocSave[TIMES], museSave[TIMES];


int main(void)
{
    Mempool  pool;

    if (!mmdp_create(&pool, 0x8000)) {
        perror("mmdp_create");
        return  -1;
    }

    void __attribute__ ((unused))  *ptr;
    bool __attribute__ ((unused))   need_free = false;

    struct timeval  end, start;
    unsigned long   tma = 0, tmm = 0;
    int32_t         size = 0;

    srandom(time(NULL));

    for (int idx = 0; idx < TIMES; idx++) {
        size = random() % 10240 + 1;
        need_free = (size < 2048);

        gettimeofday(&start, NULL);

        if (!(ptr = malloc(size)))
            perror("malloc");

        if (need_free)
            free(ptr);

        gettimeofday(&end, NULL);

        tma += TIME_DIFF(start, end);

        gettimeofday(&start, NULL);
        ptr = mmdp_malloc(&pool, size);

        if (need_free)
            mmdp_free(&pool, ptr);

        gettimeofday(&end, NULL);

        tmm += TIME_DIFF(start, end);
        need_free = false;
    }

    printf("Chunk: %d\n", pool.nchunk);
    printf("Malloc: %ld us - Musepool: %ld us\n", tma, tmm);

    mmdp_free_pool(&pool);

    return  -1;
}


void log_memory(Memlog *log, char *addr, )
{

}

