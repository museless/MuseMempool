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


#ifndef	_MATO_H
#define	_MATO_H


/*---------------------------
	define
-----------------------------*/

#define	MATO_FALSE	0x0
#define	MATO_TRUE	0x1


/*---------------------------
	typedef
-----------------------------*/

typedef	struct	matos		MATOS;
typedef	unsigned long int	mar_t;


/*---------------------------
	struct
-----------------------------*/

struct	matos {
	int	mato_cnt;
};


/*---------------------------
	global function
-----------------------------*/

void	mato_init(MATOS *pMato, int nSet);

mar_t	mato_inc(MATOS *pMato);
mar_t	mato_add(MATOS *pMato, int nInc);

mar_t	mato_dec(MATOS *pMato);
mar_t	mato_sub(MATOS *pMato, int nSub);

mar_t	mato_inc_and_test(MATOS *pMato);
mar_t	mato_add_and_test(MATOS *pMato, int nInc);

mar_t	mato_dec_and_test(MATOS *pMato);
mar_t	mato_sub_and_test(MATOS *pMato, int nSub);


#endif
