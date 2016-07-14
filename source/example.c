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

    if (!mmdp_create(&pool, 8192)) {
        perror("mmdp_create");
        return  -1;
    }

    struct timeval  end, start;
    unsigned long   tma = 0, tmm = 0;
    int             size = 0;

    srandom(time(NULL));

    for (int idx = 0; idx < 100000; idx++) {
        size = random() % 256 + 1;

        gettimeofday(&start, NULL);
        malloc(size);
        gettimeofday(&end, NULL);

        tma += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

        gettimeofday(&start, NULL);
        mmdp_malloc(&pool, size);
        gettimeofday(&end, NULL);

        tmm += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
    }

    printf("Malloc: %ld us - Musepool: %ld us\n", tma, tmm);

    mmdp_free_pool(&pool);

    return  -1;
}

