# Muse Divide Mempool

## Overview
**MDPool**, a memory divide pool


## Brief introduction
A memory pool, when you need to malloc many size fixed
piece, you can use it replace malloc/free new/delete, this mempool
use _sync_xx_xx_xx to make pthread safe


## Advantage/Disadvantage
**Adv**<br>
1. It's speed as same as malloc, new when malloc small pieces<br>
2. Do not need to manage pieces<br>
3. If you use mmdp_reset_default wisely, you will save many memory and times<br>

**Disadv**<br>
1. May be waste many memory space when you malloc size unstable pieces<br>
2. Bad performace when using it to malloc large piece<br>


## Usage
Easy understanding api for everyone<br>

**First Move**<br>
Calling "mmdp_create()" to get a DMPH stru pointer<br>
Warning: when argument 1 borderSize for mmdp_create smaller than DEFAULT_BSIZE<br>
it will use DEFAULT_BSIZE replace borderSize to be a small chunk size<br><br>

**Second Move**<br>
1. malloc memory "mmdp_malloc()"<br>
2. "mmdp_free_handler()" just free the DMPH stru<br>
3. "mmdp_free_pool()" free the whole pool(DMPH stru and the memory that pool malloced<br>
4. "mmdp_free()" just free the memory space that the memory space called by mmdp_malloc<br>
5. "mmdp_reset_default()" to make the small chunk reuseable. But careful, you need to sure no one use that chunk anymore<br>


