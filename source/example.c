/*---------------------------------------------
 *     modification time: 2016-07-12 18:29:21
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


int main(void)
{
    Mempool  pool;

    if (!mmdp_create(&pool, 0x4000)) {
        perror("mmdp_create");
        return  -1;
    }

    struct timeval  end, start;
    void           *ptr;
    unsigned long   tma = 0, tmm = 0;
    int             size = 0;
    bool            need_free;

    srandom(time(NULL));

    for (int idx = 0; idx < 1000000; idx++) {
        size = random() % 1024 + 1;
        need_free = true;

        ptr = malloc(size);
        gettimeofday(&start, NULL);

        if (need_free)
            free(ptr);

        gettimeofday(&end, NULL);

        tma += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

        ptr = mmdp_malloc(&pool, size);
        gettimeofday(&start, NULL);

        if (need_free)
            mmdp_free(&pool, ptr);

        gettimeofday(&end, NULL);

        tmm += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
    }

    printf("Chunk: %d\n", pool.nchunk);
    printf("Malloc: %ld us - Musepool: %ld us\n", tma, tmm);

    mmdp_free_pool(&pool);

    return  -1;
}

