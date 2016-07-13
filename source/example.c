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
    char     buff[4096 + sizeof(Chunk)];
    Chunk   *ptr = (Chunk *)buff;

    ptr->next_free = NULL;
    ptr->start = buff + sizeof(Chunk);
    ptr->rest = 4096;
    ptr->counter = 0;

    if (!mmdp_create(&pool, 4096)) {
        perror("mmdp_create");
        return  -1;
    }

    mmdp_malloc(&pool, 4095);
    pool.chunks[1] = ptr;
    pool.nchunk = 2;
    pool.current = ptr;

    mmdp_malloc(&pool, 4095);
    mmdp_malloc(&pool, 4095);

    return  -1;
}
