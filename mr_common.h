#ifndef _MR_COMMON_H
#define _MR_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/sendfile.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <assert.h>
#include <pthread.h>

/* conditional compiltion */
#define _ERR_PRINT
#ifdef _ERR_PRINT
#define EPT			fprintf
#define ASSERT		assert
#else
#define EPT
#define ASSERT
#endif

/* used data types */
#define U8			unsigned char
#define U16			unsigned short
#define U32			unsigned int

/* determain address length of manet node */
//#define _MADR_32BIT
//#define _MADR_16BIT

#if _32BIT_ADDR
#define MADR		U32
#else
#if _16BIT_ADDR
#define MADR		U16
#else
#define MADR		U8
#endif
#endif

/** very important  */
#define _MR_TEST						/* for routing protocol test */

/* netlayer constants  */
#define MAX_NODE_CNT		32			/* maximum number of network nodes */
#define MAX_HOPS			10			/* maximum hops */
#define MADR_UNI_MIN		1
#define MADR_UNI_MAX		MAX_NODE_CNT
#define MADR_INVALID		0
#define MADR_BRDCAST		((MADR)0xFFFFFFFF)

#define MR_ISUNI(n)			(n >= MADR_UNI_MIN && n <= MADR_UNI_MAX)

/* mac layer contants */
#define MAX_SLTS_PCF		64			/* slot number per complex frame */
#define MAX_FSLS_PCF		8			/* fixed slot number of a complex frame */
#define MAX_DSLS_PCF		(MAX_SLTS_PCF - MAX_FSLS_PCF - 1)
										/* dynamic slot number of a complex frame */
#define MAX_CFS_PSF			32			/* complex frames per super frame */
#define CF_SBS_SPT			0
#define CF_FLT_SPT			(CF_SBS_SPT + 1)
#define CF_DLT_SPT			(CF_FLT_SPT + 1)

/* constants related to creating message queues */
#define PATH_CREATE_KEY		"/etc/profile"	/* path name for creating all queue, can be modified */
#define SN_NETPROC			1					/* sub number of creating or getting the queue id of netlayer process */
#define SN_HIGHMAC			2					/* sub number of creating or getting the queue id of highmac process */
#define SN_IF2TCPIP			3					/* sub number of creating or getting the queue id of if2tcpip process */
#define SN_GUI_APP			4					/* sub number of creating or getting the queue id of GUI process */
#define SN_VOICE_APP		5					/* sub number of creating or getting the queue id of voice process */
#define SN_VIDEO_APP		6					/* sub number of creating or getting the queue id of video process */
#define SN_ROUTINGP			7					/* sub number of creating or getting the queue id of routing process */
#define SN_MRTEST			100					/* sub number of creating or getting the queue id of routing process */
/* constants related to creating shared memory */
#define SM_FWT				20

/* all message types used in MANET routing software */
#define MSG_NETLAY_BASE		10					/* all the types of messages originate from neylayer process */
#define MSG_HIGHMAC_BASE	100					/* all the types of messages originate from highmac process */
#define MSG_IF2TCPIP_BASE	200					/* all the types of messages originate from if2tcpip process */
#define MSG_ROUTINGP_BASE	300					/* all the types of messages originate from routingp process */
#define MSG_GUIAPP_BASE		400					/* all the types of messages originate from GUI process */
#define MSG_VOICEAPP_BASE	500					/* all the types of messages originate from voice process */
#define MSG_VIDEOAPP_BASE	600					/* all the types of messages originate from video process */
#define MSG_MRTEST_BASE		1000				/* all the types of messages related mr_test process */

typedef enum _MR_MSG_TYPE {
	MMSG_NULL = 0,
	MMSG_TEST = 1,
	MMSG_SELF = 2,								/* just wake the blocking thread */
	/* netlayer process */
	MMSG_MP_DATA = MSG_NETLAY_BASE,				/* manet data packet */
	MMSG_BR_CFG,								/* BB/RF config data */
	MMSG_BR_REQ,								/* request BB/RF state  */
	/* highmac process */
	MMSG_ST_DATA = MSG_HIGHMAC_BASE,			/* set slot table to low mac */
	MMSG_ST_REQ,								/* request slot table in low mac */
	MMSG_LM_REQ,								/* request lowmac state of low mac */
	MMSG_LM_STATE,								/* state info of low mac */
	/* if2tcpi process */
	MMSG_IP_DATA = MSG_IF2TCPIP_BASE,			/* ip packet */
	MMSG_EF_DATA,								/* ethernet frame */
	/* routingp process */
	MMSG_RPM = MSG_ROUTINGP_BASE,				/* routing protocol message */
	MMSG_FT_DATA,								/* set forarding table */
	MMSG_FT_REQ,								/* request ft in low mac */
	MMSG_RT_DATA,								/* route table */
	/* gui process */
	MMSG_RT_REQ = MSG_GUIAPP_BASE,				/* request route table from gui */
	/* to voice app process */
	MMSG_VO_DATA = MSG_VOICEAPP_BASE,			/* voice data */
	/* to video app process */
	MMSG_VD_DATA = MSG_VIDEOAPP_BASE,			/* video data */
	/* others msg id */
	MMSG_MT_RQID = MSG_MRTEST_BASE,				/* qid data of routingp process */
	MMSG_MT_FHR									/* fhr data for test */
} MR_MSG;

#define QUEUE_R				0400
#define QUEUE_W				0200
#define QUEUE_MODE      	(QUEUE_R | QUEUE_W | QUEUE_W>>3 | QUEUE_W>>6)

/* common constants */
#define PNAME_NETLAYER		"netlayer"
#define PNAME_ROUTINGP		"routingp"
#define PNAME_IF2TCPIP		"if2tcpip"
#define PNAME_HIGHMAC		"highmac"
#define PNAME_MR_TEST		"mrtest"

typedef struct _qinfo_t {
	char  	pname[64];
	int   	sub;
	key_t 	key_q;
	int   	qid;
} qinfo_t;

typedef struct _tproc_t {
	char	name[64];		/* name of the timer */
	U32		wait;			/* time for waiting next periodical timer */
	U32		period;			/* timer interval */
	void	(*pf)(void*);	/* pointer to the executiove function when wait = 0 */
} tproc_t;

typedef struct _tsche_t {
	U32		tmap;			/* timer map for support multiple timer function */
	U32		tmask;			/* timer mask for disabling certain timer function 1: enbale */
	tproc_t	procs[32];		/* the maximum number of the functions is 32 */
//	tproc_t	procs[2];		/* for test */
} tsche_t;

#define MAX_DATA_LENGTH		2048
typedef struct _mmsg_t {
	long  mtype;
	MADR  node;				/* differnet meaning dependent on mtype */
	char  data[MAX_DATA_LENGTH];
} mmsg_t;

/* header of data in mmsg_t  struct, this hedder can tell the function hwo to process the data in mmsg_t */
typedef struct _mmhd_t {
	U16		type;			/* sub type */
	U16		len;			/* len of the data in mmsg_t except mmhd_t */
} mmhd_t;
#define MMHD_LEN			sizeof(mmhd_t)
#define MMSG_FIXLEN			(sizeof(MADR)+MMHD_LEN)

/* item of forwarding table */
typedef struct _fwi_t {
	MADR	dest;
	MADR	fnd;
	MADR	snd;
} fwi_t;


/* forwading table */
typedef struct _fwt_t {
	MADR	self;
	fwi_t	ft[MAX_NODE_CNT];
} fwt_t;


int mr_queues_init(void *arg);
int   mr_queues_delete();

extern int qs, re_qin, nl_qid, hm_qid, vi_qid, rp_qid;    //这几个消息队列的id声明为全局
extern qinfo_t qinfs[];									  //每个消息队列的info
extern const int cnt_p;									  //消息队列的个数


#if 0
#define DE_BUG

extern void sys_info(const char *msg);
extern void sys_err(const char *msg);
extern void sys_exit(const char *msg);
extern void sys_debug(const char *msg);
#endif
#endif

