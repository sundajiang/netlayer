#include "nl_timer.h"

//这里只定义了两个网络层定时器timer1和timer2，最多可以定义32个
tsche_t nl_tsch = {
	0,											    //第一个元素是定时器map
	0,												//第二个元素是定时器mask
	{
		{"timer1", 0, 5000 /*ms*/, nl_timer_test1},//四个参数分别为定时器名，启动等待时间，循环间隔，函数指针
		{"timer2", 0, 3000 /*ms*/, nl_timer_test2}
	}
};

const int nl_tfs = sizeof(nl_tsch.procs)/sizeof(nl_tsch.procs[0]); //定时器最大个数，32个

//1s后开启，间隔500ms
struct itimerval new_value = {
{NL_TDELAY_S, NL_TDELAY_US}, 
{NL_TINTVL_S, NL_TINTVL_US}
};

void nl_tsch_init()
{
	int i, cnt;
	tproc_t *p;
	
	for (i = 0; i < nl_tfs; i++)
	{
		p = &nl_tsch.procs[i];
		if (p->period == 0)
		{
			ASSERT(nl_tsch.procs[i].pf == NULL);		//如果该计时器循环周期为0，则断言该计时器指向函数为空
			continue;
		}
		nl_tsch.tmap = nl_tsch.tmap|(1<<i);				//把该计数器对应的map标志位置1
		cnt = (p->period * 1000)/NL_TINTVL_US;			//除以基准定时器的间隔500ms，得到该定时器的基准计数
		if (cnt < 1)
			p->period = 1;
		else
			p->period = cnt;
		
		p->wait = p->period;
	}
}

int nl_start_timer()
{
	int rval;

	nl_tsch_init();
	
	signal(SIGALRM, nl_timer_sche);
	rval = setitimer(ITIMER_REAL, &new_value, NULL);
	if (-1 == rval)
	{/* failure */
		EPT(stderr, "error occurs in setting timer %d[%s]\n", errno, strerror(errno));
	}
	else
	{/* success */
		rval = 0;
	}
	return rval;
}

void nl_timer_sche(int signo)
{
	int i;
	U32	emap;

	if (SIGALRM != signo)
	{
		EPT(stderr, "Caught the other signal %d\n", signo);
		return;
	}
		
//	EPT(stderr, "Caught the SIGALRM signal\n");	

	emap = nl_tsch.tmap&(nl_tsch.tmap^nl_tsch.tmask);  //^为按位异或运算符，先异或此处再与运算，使得nl_tsch.tmask起到掩码的作用
	for (i = 0; i < nl_tfs; i++)
	{
		if (!((1<<i)&emap))
			continue;
		
		nl_tsch.procs[i].wait -= 1;
		if (nl_tsch.procs[i].wait <= 0)
		{
			nl_tsch.procs[i].wait = nl_tsch.procs[i].period;
			(*nl_tsch.procs[i].pf)(&i);
		}
	}
}

void nl_timer_test1(void *data)
{  
	int id = *(int *)data;

	EPT(stderr, "Caught the SIGALRM signal for %s\n", nl_tsch.procs[id].name);
//	EPT(stderr, "timer in thread id = %ld\n", pthread_self());
}

void nl_timer_test2(void *data)
{  
	int id = *(int *)data;

	EPT(stderr, "Caught the SIGALRM signal for %s\n", nl_tsch.procs[id].name);
}  

void nl_timer_self(void* data)
{
	int id = *(int *)data;
}
