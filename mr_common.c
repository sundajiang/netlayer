#include "mr_common.h"

qinfo_t  qinfs[] = {
	{PNAME_NETLAYER, SN_NETPROC, -1, -1},
	{PNAME_HIGHMAC,  SN_HIGHMAC, -1, -1},
	{PNAME_ROUTINGP, SN_ROUTINGP, -1, -1},
	{PNAME_IF2TCPIP, SN_IF2TCPIP, -1, -1},
	{PNAME_MAODV,    SN_MAODV,    -1, -1}
};//第三个参数是key，第四个参数是消息队列id
const int cnt_p = sizeof(qinfs)/sizeof(qinfs[0]);

int qs = 0;								//记录创建或者获得的消息队列数量
int re_qin= -1;							//记录队列数组元素中对应本进程申请队列的下标
int nl_qid = -1;
int hm_qid = -1;
int vi_qid = -1;
int rp_qid = -1;
int ma_qid = -1;

int mr_queues_init(void *arg)		   //获取所有消息队列的id（包括自己的），若消息队列不存在则就地创建
{
	int i;
	int qid, rval, stop;				//rval是return_value
	char *name;

	name = (char *)arg;

	for (i = 0; i < cnt_p; i++)			//获得所有消息队列的key,使用的路径需要更改
	{
		qinfs[i].key_q = ftok(PATH_CREATE_KEY, qinfs[i].sub);

		if(qinfs[i].key_q < 0)
		{
			EPT(stderr,"get key of queue :%d error\n",i);
		}

		if (NULL != strstr(name, qinfs[i].pname))//如果输入参数和队列名对应
		{
			re_qin = i;					//记录队列数组元素中对应本进程申请队列的下标
		}
	}

	if (-1 == re_qin)//如果输入参数和任何队列名都不对应，则直接结束消息队列初始化进程
	{
		rval = 1;			//错误情况1
		EPT(stderr, "%s: can not create the key of myself queue\n", name);
		return -1;
	}

	stop = 0;
	while (0 == stop)
	{
		qs = 0;
		for (i = 0; i < cnt_p; i++)
		{
			qid = msgget(qinfs[i].key_q, IPC_CREAT|QUEUE_MODE);
			if (qid == -1)
				EPT(stderr, "%s: can not get queue for %s\n", name, qinfs[i].pname);
			else
			{
				EPT(stderr, "process:%s, qid:%d\n", qinfs[i].pname, qid);
				qinfs[i].qid = qid;
				qs += 1;

				/* set the qid */
				if (NULL != strstr(PNAME_NETLAYER, qinfs[i].pname))
				{
					nl_qid = qid;
				}
				else if (NULL != strstr(PNAME_ROUTINGP, qinfs[i].pname))
				{
					rp_qid = qid;
				}
				else if (NULL != strstr(PNAME_IF2TCPIP, qinfs[i].pname))
				{
					vi_qid = qid;
				}
				else if (NULL != strstr(PNAME_HIGHMAC, qinfs[i].pname))
				{
					hm_qid = qid;
				}
				else if (NULL != strstr(PNAME_MAODV, qinfs[i].pname))
				{
					ma_qid = qid;
				}

			}
		}

		if (qs == cnt_p)   //获取id的队列数和设计的队列总数相符，正常退出
		{
			rval = 0;
			stop = 1;
		}
		else
		{
			sleep(2);
		}
	}

	EPT(stdout, "exit from finding queue thread.\n");

	return 0;
}


int mr_queues_delete()
{
	int i;

	for (i = 0; i < cnt_p; i++)
	{
		if (qinfs[i].qid != -1);
			msgctl(qinfs[i].qid, IPC_RMID, NULL);
	}

	return 0;
}
