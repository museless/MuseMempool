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


#ifndef	_MMDPOOL_H
#define	_MMDPOOL_H


/* include */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mmdperr.h"
#include "ato.h"

/* typedef */
typedef	struct	dmp_handler	DMPH;
typedef	struct	dmp_body	DMPB;
typedef	struct	dmp_bigbody	DMPBB;

typedef	unsigned int	msize_t;
typedef	unsigned char	mpt_t;
typedef	unsigned short	merr_t;

/* struct */
struct	dmp_handler {
	DMPB	*mh_stru;		/* list to all small chunk */
	DMPB	*mh_select;		/* select list */

	DMPBB	*mh_big;		/* big chunk head */

	msize_t	mh_sizebor;		/* size border */

	msize_t	mh_cnt;			/* cnt for pool smaller than DEFAULT_BSIZE */
	merr_t	mh_err;			/* pool err num, one handler one err */
	MATOS	mh_biglock, mh_deflock;
};

struct	dmp_body {
	mpt_t	*mb_start;
	mpt_t	*mb_end;

	DMPB	*mb_next;
	DMPB	*mb_nselec;

	msize_t	mb_size;
	msize_t	mb_left;
	msize_t	mb_taker;
};

struct	dmp_bigbody {
	mpt_t	*mbb_start;

	DMPBB	*mbb_fore;	/* point to forward */
	DMPBB	*mbb_next;	/* point to next */
};

/* define */
#define	DEFAULT_BSIZE		0x1000
#define	DEFALUT_MAX_BODY	0x10000	

/* API */
DMPH	*mmdp_create(int borderSize);
void	*mmdp_malloc(DMPH *mHand, msize_t maSize);
void	mmdp_free(DMPH *pHandler, mpt_t *pFree);
void	mmdp_free_pool(DMPH *pMfree);
void	mmdp_free_handler(DMPH *pMfree);
void	mmdp_free_all(DMPH *pMfree);
void	mmdp_reset_default(DMPH *pReset);
int	mmdp_show_size(DMPH *pMp);

#endif
