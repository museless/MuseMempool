#include <iostream>
#include "mmdpool.h"
#include <sys/time.h>
#include <errno.h>


/* using */
using namespace std;

/* define */
#define	MTIMES	1000000
#define	CHUNKS	0x10

/* typedef */
typedef	struct timeval	TVAL;
typedef	struct testbuf	TBUF;

/* struct */
struct testbuf {
	char	t_buf[16];
};

/* local function */
static	int	mmdp_test(void);
static	int	ma_test(void);
static	void	new_test(void);

static	void	mmdp_error(const char *errStr, int nErr);


/*-----main-----*/
int main(void)
{
	if(!mmdp_test())
		perror("main - mmdp_test");
	
	//if(!ma_test())
	//	perror("main - ma_test");
	
	//new_test();
	
	return	1;
}


/*-----mmdp_test-----*/
static int mmdp_test(void)
{
	DMPH	*pMem;
	char	*pBeg;
	TVAL	tStart, tEnd;
	int	nSize = sizeof(TBUF);
	
	if(!(pMem = mmdp_create(DEFAULT_BSIZE))) {
		mmdp_error("mmdp_test - mmdp_create", errno);
		return	-1;
	}
	
	gettimeofday(&tStart, NULL);
	
	for(int nCir = 0; nCir < 300; nCir++) {
		if(!(pBeg = (char *)mmdp_malloc(pMem, nSize))) {
			mmdp_error("mmdp_test - mmdp_malloc", errno);
			return	0;
		}
		
		for(int nCount = 0; nCount < nSize - 1; nCount++)
			pBeg[nCount] = 'a';
	}
	
	gettimeofday(&tEnd, NULL);
	mmdp_free_all(pMem);
	
	cout << "Mdp---> microseconds: " << tEnd.tv_usec - tStart.tv_usec << endl;
	
	return	1;
}


/*-----ma_test-----*/
static int ma_test(void)
{
	TVAL	tStart, tEnd;
	int	nSize = sizeof(TBUF);
	
	gettimeofday(&tStart, NULL);
	
	for(int nCir = 0; nCir < MTIMES; nCir++) {
		if(!malloc(nSize))
			return	0;
	}
	
	gettimeofday(&tEnd, NULL);
	
	cout << "Malloc---> microseconds: " << tEnd.tv_usec - tStart.tv_usec << endl;
	
	return	1;
}


/*-----new_test-----*/
static void new_test(void)
{
	TVAL	tStart, tEnd;
	
	gettimeofday(&tStart, NULL);
	
	for(int nCir = 0; nCir < MTIMES; nCir++)
		new	TBUF;
	
	gettimeofday(&tEnd, NULL);
	
	cout << "New---> microseconds: " << tEnd.tv_usec - tStart.tv_usec << endl;
	
	return;
}


/*-----mmdp_error-----*/
static void mmdp_error(const char *errStr, int nErr)
{
	printf("Mdp---> %s: %s\n", errStr, strerror(nErr));
}
