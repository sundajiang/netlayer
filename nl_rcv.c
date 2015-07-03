#include "nl_rcv.h"

void* nl_qrv_thread(void *arg)		  //参数为接收线程id
{
	int qid, rcnt;

	int rval = 0;

	nl_buf_pool = (nl_buff_pool_t *)malloc(nl_buff_num * sizeof(nl_buff_pool_t));	//为处理pkt的可复用缓存队列申请空间

	pthread_detach(pthread_self());	  //线程置为非阻塞，可立即返回

	qid = *(int *)arg;				  //获得接收线程id
	ASSERT(qinfs[re_qin].qid == qid);
	if (qid < 0)
	{
		EPT(stdout, "%s: wrong receive queue id %d", qinfs[re_qin].pname, qid);
		rval = 1;
		goto thread_return;
	}

	//测试pkt的存取和地址分配
	/*nl_package_t * pkt;
	pkt = (nl_package_t *)malloc(sizeof(nl_package_t));
	set_PR(pkt,0);
	set_TYPE(pkt,0);
	set_SubT(pkt,3);
	set_src_addr(pkt,SRC_ADDR);
	set_dst_addr(pkt,4);
	set_rcv_addr(pkt,5);
	set_snd_addr(pkt,SRC_ADDR);
	set_SEQ(pkt);
	set_H(pkt,1);
	set_SN(pkt,4);
	set_TTL(pkt,7);
	set_CoS(pkt,0);
	strcpy(pkt->data,"hello");
	set_CRC(pkt,99);
	printf("PR = %d \n",get_PR(pkt));
	printf("addr of pkt is : %p\n",pkt);
	printf("TYPE = %d \n",get_TYPE(pkt));
	printf("SubT = %d \n",get_SubT(pkt));
	printf("src_addr = %d \n",get_src_addr(pkt));
	printf("addr of src_addr is : %p\n",&(pkt->src_addr));
	printf("dst_addr = %d \n",get_dst_addr(pkt));
	printf("addr of dst_addr is : %p\n",&(pkt->dst_addr));
	printf("rcv_addr = %d \n",get_rcv_addr(pkt));
	printf("addr of rcv_addr is : %p\n",&(pkt->rcv_addr));
	printf("snd_addr = %d \n",get_snd_addr(pkt));
	printf("addr of snd_addr is : %p\n",&(pkt->snd_addr));
	printf("SEQ = %d \n",get_SEQ(pkt));
	printf("H = %d \n",get_H(pkt));
	printf("SN = %d \n",get_SN(pkt));
	printf("TTL = %d \n",get_TTL(pkt));
	printf("CoS = %d \n",get_CoS(pkt));
	printf("data = %s \n",pkt->data);
	printf("addr of data is : %p\n",pkt->data);
	printf("size of data is : %d\n",sizeof(pkt->data));
	printf("length of data is : %d\n",strlen(pkt->data));
	printf("CRC = %d \n",get_CRC(pkt));
	printf("addr of CRC is : %p\n",&(pkt->CRC));*/
	//测试mmsg_t的内存分配
	/*mmsg_t *test;
	test = (mmsg_t *)malloc(sizeof(mmsg_t));
	test->mtype = 3;
	test->mdest = 5;
	strcpy(test->data,"hello,mmsg_t test");
	printf("addr of mtype is : %p\n",&(test->mtype));
	printf("addr of mdest is : %p\n",&(test->mdest));
	printf("addr of data is : %p\n",test->data);
	printf("data = %s\n",test->data);
	printf("size of data is : %d\n",sizeof(test->data));
	printf("length of data is : %d\n",strlen(test->data));*/

	mmsg_t rcv_msg;						  //该对象用于消息队列数据的直接存储，只需要一个rcvbuff
	while(1)
	{
	    printf("===========nl_layer waiting for data from nl_layer==========\n");
		memset(&rcv_msg, sizeof(rcv_msg), 0);								//接收缓存每次使用前清空，因为只有这一个接收缓存
		rcnt = msgrcv(qid, &rcv_msg, sizeof(mmsg_t) - sizeof(long), 0, 0);	//成功返回拷贝到结构体数据部分的字节数,失败返回-1
		if (rcnt < 0)
		{
			if (errno == EINTR)												//系统调用阻塞时,被信号终端,所以重新开始接收
				continue;
			if(errno == E2BIG)
				EPT(stderr, "%s:E2BIG!!\n", qinfs[re_qin].pname);
			if(errno == EACCES)
				EPT(stderr, "%s:EACCES!!\n", qinfs[re_qin].pname);
			if(errno == EAGAIN)
				EPT(stderr, "%s:EAGAIN!!\n", qinfs[re_qin].pname);
			if(errno == EFAULT)
				EPT(stderr, "%s:EFAULT!!\n", qinfs[re_qin].pname);
			if(errno == EIDRM)
				EPT(stderr, "%s:EIDRM!!\n", qinfs[re_qin].pname);
			if(errno == EINVAL)
				EPT(stderr, "%s:EINVAL!!\n", qinfs[re_qin].pname);
			if(errno == ENOMSG)
				EPT(stderr, "%s:ENOMSG!!\n", qinfs[re_qin].pname);
			EPT(stderr, "%s:rcv wrong!!\n", qinfs[re_qin].pname);
			rval = 2;
			goto thread_return;
		}
		EPT(stdout, "%s:---- reveived---- %d \n", qinfs[re_qin].pname, rcnt);
		printf("nl_layer rcv mtype: %ld\n",rcv_msg.mtype);
		printf("nl_layer rcv dest: %d\n",rcv_msg.node);
		if(MMSG_MP_DATA == rcv_msg.mtype)
		{
            nl_package_t * pkt;
            pkt = (nl_package_t *)(rcv_msg.data);
            //printf("nl_layer rcv himac data: %s\n",((nl_package_t *)(rcv_msg.data))->data + sizeof(mmhd_t));		}
            printf("nl_layer rcv himac data: %s\n",rcv_msg.data + 12);
        }
        else
            printf("nl_layer rcv data: %s\n",rcv_msg.data);
		printf("MMSG_MP_DATA = %d\n",MMSG_MP_DATA);
		printf("MMSG_IP_DATA = %d\n",MMSG_IP_DATA);



		if(MMSG_MP_DATA == rcv_msg.mtype)		        //如果是HMAC发来的,就进行重组处理
		{
		    printf("ready to send to up_lavel_software \n");
			nl_package_t * pkt = (nl_package_t *)rcv_msg.data;
			combine_send_pkt(pkt);
		}
		else
		{
		    printf("ready to send to hihmac \n");
			int len = rcnt - sizeof(MADR);
			nl_send_to_himac(&rcv_msg,len);
		}
	}

thread_return:
	pthread_mutex_lock(&share.mutex);		//使用互斥锁，保证操作的原子性，不可中断
	free(nl_buf_pool);
	nl_buf_pool = NULL;
	share.qr_run = 0;
	pthread_cond_signal(&share.cond);		//唤醒主线程的退出阻塞，主线程退出并通过qr_run = 0获知是接收线程出错
	pthread_mutex_unlock(&share.mutex);
	sleep(1);
	pthread_exit((void *)&rval);
}






