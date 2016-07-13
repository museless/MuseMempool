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
    MATOS   ato;

    mato_init(ato, 1);

    mato_lock(ato);
    mato_unlock(ato);

    return  -1;
}
