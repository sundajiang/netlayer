#ifndef NL_RCV_H
#define NL_RCV_H

#include "nl_common.h"
#include "nl_package.h"
#include "nl_send.h"

void* nl_qrv_thread(void *);

extern nl_buff_pool_t  *nl_buf_pool;
extern int nl_buff_num;


#endif // NL_RCV_H
