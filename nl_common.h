#ifndef _NL_COMMON_H
#define _NL_COMMON_H

#include "mr_common.h"
#include <sys/shm.h>

typedef struct _nl_tshare_t {
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	int qr_run;
	int gi_run;
} nl_tshare_t;

extern nl_tshare_t  share;
extern MADR SRC_ADDR;

int shm_init();

fwt_t * shm_fwt;


#endif

