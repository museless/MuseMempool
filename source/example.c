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


int main(void)
{
    Mempool  pool;

    if (!mmdp_create(&pool, 4096)) {
        perror("mmdp_create");
        return  -1;
    }

    mmdp_malloc(&pool, 4095);
    mmdp_malloc(&pool, 4095);
    mmdp_malloc(&pool, 4095);

    mmdp_malloc(&pool, 8192);

    mmdp_free_pool(&pool);

    return  -1;
}
