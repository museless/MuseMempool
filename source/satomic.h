/*---------------------------------------------
 *     modification time: 2016-07-11 12:57:45
 *     mender: Muse
-*---------------------------------------------*/

/*---------------------------------------------
 *     file: satomic.h 
 *     creation time: 2016-07-11 12:57:45
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

#include <stdbool.h>


/*---------------------------------------------
 *            Part Two: Typedef
-*---------------------------------------------*/

typedef struct matos    MATOS;


/*---------------------------------------------
 *            Part Three: Struct
-*---------------------------------------------*/

struct matos {
    int     cnt; 
};


/*---------------------------------------------
 *            Part Four: Function
-*---------------------------------------------*/

#define mato_init(ato, set) \
        ato.cnt = set

#define mato_inc(ato) \
        __sync_fetch_and_add(&ato.cnt, 1)

#define mato_add(ato, inc) \
        __sync_fetch_and_add(&ato.cnt, (inc))

#define mato_dec(ato) \
        __sync_fetch_and_sub(&ato.cnt, 1)

#define mato_sub(ato, sub) \
        __sync_fetch_and_sub(&ato.cnt, (sub))

#define mato_inc_and_test(ato) \
        ((__sync_fetch_and_sub(&ato.cnt, 1) - 1) ? false : true)

#define mato_add_and_test(ato, inc) \
        ((__sync_fetch_and_add(&ato.cnt, (inc)) + inc) ? false : true)

#define mato_dec_and_test(ato) \
        ((__sync_fetch_and_sub(&ato.cnt, 1) - 1) ? false : true)

#define mato_sub_and_test(ato, sub) \
        ((__sync_fetch_and_sub(&ato.cnt, (sub)) - sub) ? false : true)

#define mato_lock(ato) \
        while (!mato_dec_and_test((ato))) \
            mato_inc((ato));

#define mato_unlock(ato) \
        mato_inc((ato))

