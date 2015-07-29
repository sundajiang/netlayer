#include "nl_send.h"

static int lock = 0;								  //c中没有bool类型，这里定义成int型, nl_send_to_others函数的lock
static int lock_of_himac = 0;						  //nl_send_to_himac函数的lock

nl_buff_pool_t  *nl_buf_pool;						  //动态申请针对himac的接收缓存，总数量为nl_buff_num，循环使用，用于整理pkt包
int nl_buff_num = 5;
static int nl_buff_timeout = 5;

int combine_send_pkt(nl_package_t * pkt)
{
	int H, SN;
	H = get_H(pkt);
	SN = get_SN(pkt);
	/*************************************************不经过拆分的包，直接转发******************************************************/
	if(H == 1 && SN == 0)
	{
		printf("sigle pkt\n");
		mmsg_t * snd_buf;
		snd_buf = (mmsg_t *)malloc(sizeof(mmsg_t));
		if (snd_buf == NULL)
			EPT(stderr, "malloc error when deal sigle pkt\n");
		memset(&snd_buf, sizeof(snd_buf), 0);
		snd_buf->mtype = get_data_type(pkt);
		snd_buf->node = get_src_addr(pkt);
		memcpy(&snd_buf->data, pkt->data + sizeof(mmhd_t), get_data_length(pkt));
		nl_send_to_others(snd_buf,get_data_length(pkt));
		free(snd_buf);
		snd_buf == NULL;
		return 0;
	}
	/*************************************************预约或者获取pkt整理缓存******************************************************/
	nl_buff_t * pkt_buf;
	int pool_id ;
	int key = get_data_type(pkt) + 10*get_src_addr(pkt);
	while(pool_id = manage_nl_buf(key) == -1)
	{
		EPT(stderr, "order a nl_buf error!!maybe full used!!\n");
		sleep(1);
	}
	pkt_buf =&((&nl_buf_pool[pool_id])->nl_buf);
	/******************************************************重组pkt包************************************************************/
	if (H == 1)
	{
		pkt_buf->number = SN;
		pkt_buf->count = 1;
		memcpy(pkt_buf->package[0], pkt, sizeof(nl_package_t));					//拷贝越界没有问题，因为后面的空间还属于接收buff，值为0
	}
	else
	{
		pkt_buf->count++;
		memcpy(pkt_buf->package[SN-1], pkt, sizeof(nl_package_t));
	}
	//if (pkt_buf->number == SN && pkt_buf->count == SN)
	//if(pkt_buf->number == SN)
	if (pkt_buf->number == pkt_buf->count)
	{
		int i,len = 0;
		U16 data_len;
		nl_package_t *tmp_ptr;
		mmsg_t * snd_buf;
		snd_buf = (mmsg_t *)malloc(sizeof(mmsg_t));
		if (snd_buf == NULL)
			return;
		snd_buf->mtype = get_data_type(pkt);
		snd_buf->node = get_src_addr(pkt);
		char * ptr = snd_buf->data;
		for (i=0; i<pkt_buf->number; i++)
		{
			tmp_ptr = (nl_package_t*)(pkt_buf->package[i]);
			data_len = get_data_length(tmp_ptr);
			memcpy(ptr, tmp_ptr->data + sizeof(mmhd_t), data_len);
			ptr += data_len;
			len += data_len;
		}

		nl_send_to_others(snd_buf,len);
		free(snd_buf);
		snd_buf == NULL;
	}
	return 0;
}

//根据snd_msg不同的数据类型发送到相应的进程
int nl_send_to_others(mmsg_t *snd_msg, U16 length)
{
	while(lock)					//保证统一时间只有该函数只被一个对象访问
	{
		sleep(1);
	}
	lock = 1;

	int qid;

	switch(snd_msg->mtype)
	{
		case MMSG_IP_DATA:
			qid = vi_qid;
			snd_msg->mtype = MMSG_IP_DATA;
			break;
		case MMSG_RPM:
			qid = rp_qid;
			snd_msg->mtype = MMSG_RPM;
			break;
        case MMSG_FT_DATA:
            qid = rp_qid;
			snd_msg->mtype = MMSG_FT_DATA;
			break;
		case MMSG_MAODV:
			qid = ma_qid;
			snd_msg->mtype = MMSG_MAODV;
			break;
		default:
			qid = -1;
			break;
	}
	while(msgsnd(qid, snd_msg, length + sizeof(MADR), 0) < 0)
	{
		if (errno == EINTR)
			continue;
		else
			{
				EPT(stdout, "%s:------snd to himac wrong------------\n", qinfs[re_qin].pname);
				return -1;
			}
	}

	printf("nl,send\n");

	lock = 0;
	return 0;
}

//把消息数据经过封装分解发送到himac层
int nl_send_to_himac(mmsg_t *msg,int len)
{
	while(lock_of_himac)									//保证同一时间只有该函数只被一个对象访问
	{
		sleep(1);
	}
	lock_of_himac = 1;

	mmsg_t * snd_buf;
	snd_buf = (mmsg_t *)malloc(sizeof(mmsg_t));
	if (snd_buf == NULL)
		EPT(stderr, "malloc error when deal sigle pkt\n");
	memset(&snd_buf, sizeof(snd_buf), 0);
	//snd_buf->mtype = MMSG_MP_DATA;							//设置类型为从网络层到HIMAC

	//一共由三种类型，MMSG_IP_DATA，MMSG_FT_DATA，MMSG_RPM,MMSG_MAODV
	snd_buf->mtype = msg->mtype;
	snd_buf->node = msg->node;

	char *ptr = NULL;
	int left,n;
	int count = 2;
	nl_package_t * pkt;
	pkt = (nl_package_t *)(snd_buf->data);					//把pkt放在snd_buf的data部分
	init_package_head(pkt,msg->mtype,msg->node);			//初始化头部

	//int length = ((mmhd_t *)msg->data)->len;
	//消息队列data的长度
	int length =len;
	left = length;
	ptr = (char *)(msg->data);
	while(left > 0)
	{
		if (left > (MAX_PACKAGE_DATA_LENGTH -sizeof(mmhd_t)))
		{
			n = MAX_PACKAGE_DATA_LENGTH -sizeof(mmhd_t);
		}
		else
		{
			n = left;
		}

		set_data_length(pkt, n);

		if (left == length)									//拆出的第一个pkt
		{
		    printf("this is the pkt : 1\n");
			set_H(pkt, 1);
			if(length >(MAX_PACKAGE_DATA_LENGTH -sizeof(mmhd_t)))
				set_SN(pkt,(length-1)/(MAX_PACKAGE_DATA_LENGTH -sizeof(mmhd_t))+1);//若接收的长度小于pkt容量,则默认SN = 0
		}
		else
		{
		    printf("this is the pkt : %d\n",count);
			set_H(pkt, 0);
			set_SN(pkt,count++);
		}
		memcpy(pkt->data + sizeof(mmhd_t), ptr, n);
		ptr += n;
		left -= n;
        printf("pkt data : %s\n",pkt->data + sizeof(mmhd_t));
		int size_for_snd = sizeof(MADR) + sizeof(mmhd_t)  + 8 + n;
		while(msgsnd(hm_qid, snd_buf, size_for_snd, 0) < 0)
		{
			if (errno == EINTR)
				continue;
			else
				{
					EPT(stdout, "%s:------snd to himac wrong------------\n", qinfs[re_qin].pname);
					break;
				}
		}

		/*nl_package_t * pkt_1 = (nl_package_t *)snd_buf->data;
		combine_send_pkt(pkt_1);

		printf("here2\n");*/

	}

	free(snd_buf);
	snd_buf == NULL;
	lock_of_himac = 0;
	return 0;
}

int manage_nl_buf(int key)				//管理nl_buff，输入key，输出对应的buff_pool序号
{
	time_t 	ctime;
	ctime = time(NULL);
	int id;

	for(id = 0;id < nl_buff_num;id++)
	{
		if((&nl_buf_pool[id])->flag == 1 && (&nl_buf_pool[id])->KEY == key)
		{
			(&nl_buf_pool[id])->time = ctime;
			return id;
		}
	}

	for(id = 0;id < nl_buff_num;id++)
	{
		if(ctime - (&nl_buf_pool[id])->time > nl_buff_timeout)
			(&nl_buf_pool[id])->flag = 0;
		if((&nl_buf_pool[id])->flag == 0)		//如果找到空闲缓存则清空并预约这个缓存
		{
			memset(&nl_buf_pool[id], sizeof(nl_buf_pool), 0);
			(&nl_buf_pool[id])->KEY = key;
			(&nl_buf_pool[id])->flag = 1;
			(&nl_buf_pool[id])->time = ctime;
			return id;
		}
	}

	return -1;
}
