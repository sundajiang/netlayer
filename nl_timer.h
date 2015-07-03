#ifndef _NL_TIMER_H
#define _NL_TIMER_H

#include "nl_common.h"

#define NL_TDELAY_S		1			/* second */
#define NL_TDELAY_US	0			/* us 0~999999 */
#define NL_TINTVL_S		0			/* second */
#define NL_TINTVL_US	500000		/* us 0~999999 */

void nl_tsch_init();
int  nl_start_timer();
void nl_timer_sche(int);
void nl_timer_proc(int);

void nl_timer_self(void*);

/* for debug */
void nl_timer_test1(void*);
void nl_timer_test2(void*);
/* for debug */
#endif
