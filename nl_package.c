#include "nl_package.h"

static U8 seq = 0;
//只需要数据类型和目的地址就能生成完整的帧头
void init_package_head(nl_package_t* pkt,U8 mtype,MADR mdest)
{
	U8 PR = 0;						//协议版本，当前为0
	U8 TYPE = 0;
	U8 SubT = 0;
	U8 ttl = MAX_HOPS;
	U8 cos = 0;

	long type;
	type = mtype;
	if (type == MMSG_RPM)
	{
        cos = 0;
		TYPE = 0;
		SubT = 0;
	}
	else if(type == MMSG_IP_DATA)
	{
	    cos = 3;
		TYPE = 0;
		SubT = 2;
	}

	set_PR(pkt,PR);
	set_TYPE(pkt, TYPE);
	set_SubT(pkt,SubT);
	set_src_addr(pkt,SRC_ADDR);
	set_dst_addr(pkt,mdest);
	//set_rcv_addr(pkt,rcv_addr);
	find_and_set_rcv_addr(pkt);
	set_snd_addr(pkt,SRC_ADDR);		//发送地址也设置为源地址
	set_SEQ(pkt);
	set_H(pkt,1);					//默认设置为完整帧不需要分割
	set_SN(pkt,0);
	set_TTL(pkt,ttl);
	set_CoS(pkt,cos);

	set_data_type(pkt,mtype);
}

inline void set_PR(nl_package_t* pkt,U8 PR)
{
	pkt->PR = PR;
}

inline U8 get_PR(nl_package_t *pkt)
{
	return pkt->PR;
}

inline void set_TYPE(nl_package_t* pkt,U8 TYPE)
{
	pkt->TYPE = TYPE;
}

inline U8 get_TYPE(nl_package_t *pkt)
{
	return pkt->TYPE;
}

inline void set_SubT(nl_package_t* pkt,U8 SubT)
{

	pkt->SubT = SubT;
}

inline U8 get_SubT(nl_package_t *pkt)
{
	return pkt->SubT;
}

inline void set_src_addr(nl_package_t* pkt,U8 addr)
{
	pkt->src_addr = addr;
}

inline U8 get_src_addr(nl_package_t *pkt)
{
	return pkt->src_addr;
}

inline void set_dst_addr(nl_package_t* pkt,U8 addr)
{
	pkt->dst_addr = addr;
}

inline U8 get_dst_addr(nl_package_t *pkt)
{
	return pkt->dst_addr;
}

inline void set_rcv_addr(nl_package_t* pkt,U8 addr)
{
	pkt->rcv_addr = addr;
}

void find_and_set_rcv_addr(nl_package_t *pkt)
{
	fwi_t* rt;
	if(MADR_BRDCAST == pkt->dst_addr)
		set_rcv_addr(pkt,MADR_BRDCAST);
	else
	{
		rt = &(shm_fwt->ft[pkt->dst_addr]);
		if (rt == NULL)
		{
			EPT(stderr, "have no route to %d ,set rcv_addr as 0\n", pkt->dst_addr);
			set_rcv_addr(pkt,MADR_INVALID);
			return;
		}
		set_rcv_addr(pkt, rt->fnd);
	}
}

inline U8 get_rcv_addr(nl_package_t *pkt)
{
	return pkt->rcv_addr;
}

inline void set_snd_addr(nl_package_t* pkt,U8 addr)
{
	pkt->snd_addr = addr;
}

inline U8 get_snd_addr(nl_package_t *pkt)
{
	return pkt->snd_addr;
}

inline void set_SEQ(nl_package_t* pkt)
{
	if(seq > 127)
		seq = 0;
	pkt->SEQ = seq;
	seq++;
}

inline U8 get_SEQ(nl_package_t *pkt)
{
	return pkt->SEQ;
}

inline void set_H(nl_package_t *pkt, U8 H)
{
	pkt->H = H;
}

inline U8 get_H(nl_package_t *pkt)
{
	return pkt->H;
}

inline void set_SN(nl_package_t *pkt, U8 SN)
{
	pkt->SN = SN;
}

inline U8 get_SN(nl_package_t *pkt)
{
	return pkt->SN;
}

inline void set_TTL(nl_package_t *pkt, U8 ttl)
{
	if (ttl > MAX_HOPS)
		ttl = MAX_HOPS;
	pkt->TTL = ttl;
}

inline U8 get_TTL(nl_package_t *pkt)
{
	return pkt->TTL;
}

inline void set_CoS(nl_package_t *pkt, U8 cos)
{
	pkt->CoS = cos;
}

inline U8 get_CoS(nl_package_t *pkt)
{
	return pkt->CoS;
}

inline void set_CRC(nl_package_t *pkt, U8 CRC)
{
	//pkt->CRC = CRC;
}

inline U8 get_CRC(nl_package_t *pkt)
{
	//return pkt->CRC;
}


inline void set_data_type(nl_package_t *pkt,U8 data_type)
{
	((mmhd_t*)&pkt->data)->type = data_type;
}

inline U16 get_data_type(nl_package_t *pkt)
{
	return ((mmhd_t*)&pkt->data)->type;
}

inline void set_data_length(nl_package_t *pkt,U16 data_length)
{
	((mmhd_t*)&pkt->data)->len = data_length;
}

inline U16 get_data_length(nl_package_t *pkt)
{
	return ((mmhd_t*)&pkt->data)->len;
}
