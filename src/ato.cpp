/* Copyright (c) 2015, William Muse
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/*------------------------------------------
	Source file content Five part

	Part Zero:	Include
	Part One:	Define
	Part Two:	Local data
	Part Three:	Local function

	Part Four:	Muse atomic oper

--------------------------------------------*/

/*------------------------------------------
	Part Zero: Include
--------------------------------------------*/

#include "ato.h"


/*------------------------------------------
	Part Four: Muse atomic oper

	1. mato_init
	2. mato_inc
	3. mato_add
	4. mato_dec
	5. mato_sub
	6. mato_inc_and_test 
	7. mato_add_and_test
	8. mato_dec_and_test
	9. mato_sub_and_test

--------------------------------------------*/

/*-----mato_init-----*/
void mato_init(MATOS *pMato, int nSet)
{
	pMato->mato_cnt = nSet;
	return;
}


/*-----mato_inc-----*/
mar_t mato_inc(MATOS *pMato)
{
	return	__sync_fetch_and_add(&pMato->mato_cnt, 1);
}


/*-----mato_add-----*/
mar_t mato_add(MATOS *pMato, int nInc)
{
	return	__sync_fetch_and_add(&pMato->mato_cnt, (nInc));
}


/*-----mato_dec-----*/
mar_t mato_dec(MATOS *pMato)
{
	return	__sync_fetch_and_sub(&pMato->mato_cnt, 1);
}


/*-----mato_sub-----*/
mar_t mato_sub(MATOS *pMato, int nSub)
{
	return	__sync_fetch_and_sub(&pMato->mato_cnt, (nSub));
}


/*-----mato_inc_and_test-----*/
mar_t mato_inc_and_test(MATOS *pMato)
{
	return	((__sync_fetch_and_add(&pMato->mato_cnt, 1) + 1) ? MATO_FALSE : MATO_TRUE);
}


/*-----mato_add_and_test-----*/
mar_t mato_add_and_test(MATOS *pMato, int nInc)
{
	return	((__sync_fetch_and_add(&pMato->mato_cnt, (nInc)) + nInc) ? MATO_FALSE : MATO_TRUE);
}


/*-----mato_dec_and_test-----*/
mar_t mato_dec_and_test(MATOS *pMato)
{
	return	((__sync_fetch_and_sub(&pMato->mato_cnt, 1) - 1) ? MATO_FALSE : MATO_TRUE);
}


/*-----mato_sub_and_test-----*/
mar_t mato_sub_and_test(MATOS *pMato, int nSub)
{
	return	((__sync_fetch_and_sub(&pMato->mato_cnt, (nSub)) - nSub) ? MATO_FALSE : MATO_TRUE);
}
