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

    if (!mmdp_create(&pool, 0x8000)) {
        perror("mmdp_create");
        return  -1;
    }

    struct timeval  end, start;
    void           *ptr;
    unsigned long   tma = 0, tmm = 0;
    int             size = 0;
    bool            need_free;

    srandom(time(NULL));

    for (int idx = 0; idx < 100000; idx++) {
        size = random() % 1024 + 1;
        need_free = true;

        gettimeofday(&start, NULL);
        ptr = malloc(size);
        gettimeofday(&end, NULL);

        tma += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

        gettimeofday(&start, NULL);
        ptr = mmdp_malloc(&pool, size);
        gettimeofday(&end, NULL);

        tmm += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
    }

    printf("Chunk: %d\n", pool.nchunk);
    printf("Malloc: %ld us - Musepool: %ld us\n", tma, tmm);

    mmdp_free_pool(&pool);

    return  -1;
}

