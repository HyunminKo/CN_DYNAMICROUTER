// UDP.cpp: implementation of the CUDP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouter.h"
#include "UDP.h"
#include <string.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUDP::CUDP(char* pName)
	: CBaseLayer( pName )
{

}

CUDP::~CUDP()
{

}

BOOL CUDP::Send( unsigned char* ppayload, int nlength)
{
	BOOL bSuccess=FALSE;
	unsigned char dstIPAddr[6];
	m_header.uh_dport=UDP_PORT;
	m_header.uh_sport=UDP_PORT;
	m_header.uh_ulen=ntohs(SIZE_UDP_HEADER+nlength);	
	m_header.uh_sum=0x0000;
	memcpy(m_header.uh_data, ppayload, nlength);
	
	dstIPAddr[0]=0xe0;
	dstIPAddr[1]=0x00;
	dstIPAddr[2]=0x00;
	dstIPAddr[3]=0x09;
																//CheckSum()호출전에 IP주소를 설정
	memcpy(m_pseudoHeader.DestinationAddress, dstIPAddr, 4);
	memcpy(m_pseudoHeader.SourceAddress, ((CIpLayer*)mp_UnderLayer)->m_header.ip_source.S_ip_addr, 4);
	m_pseudoHeader.Zeros = 0x00 ;
	m_pseudoHeader.PTCL = 0x17 ;
	m_pseudoHeader.Length = ntohs(SIZE_UDP_HEADER+nlength);
	
	MakeCheckSum(SIZE_UDP_HEADER+nlength);

	if(((CIpLayer*)mp_UnderLayer)->Send((unsigned char*)&m_header,SIZE_UDP_HEADER+nlength,dstIPAddr,TRUE))
		bSuccess=TRUE;

	return bSuccess;
}

BOOL CUDP::Receive(unsigned char *ppayload, unsigned char *dstIPAddr,unsigned char *srcIPAddr)
{
	BOOL bSuccess=FALSE;
	ptrUDP header=(ptrUDP)ppayload;
																//CheckSum()호출전에 IP주소를 설정
	memcpy(m_pseudoHeader.DestinationAddress, dstIPAddr, 4);
	memcpy(m_pseudoHeader.SourceAddress, ((CIpLayer*)mp_UnderLayer)->m_header.ip_source.S_ip_addr, 4);

		
	if( header->uh_dport == UDP_PORT ){
		if(((CRIP*)mp_aUpperLayer[0])->Receive((unsigned char*)header->uh_data,m_NICDescription,srcIPAddr))
			bSuccess=TRUE;
	}

	return bSuccess;
}

void CUDP::SetAddress(char *NICDescription,unsigned char* srcIP)
{

	sprintf(m_NICDescription,"%s",NICDescription);
	//memcpy(m_NICDescription,NICDescription,strlen(NICDescription));
	memcpy(m_srcIPAddr,srcIP,4);
}

void CUDP::MakeCheckSum(int iPacketLength)								
{	
	int pseudoHeaderLength = 12 ;
	int iTotalLength = iPacketLength + pseudoHeaderLength ;	// byte단위
	unsigned char padding = 0x00;
	unsigned short* usBuff;

	unsigned long usChksum= 0 ;

	if((iTotalLength%2) != 0)	{ // 짝수가 아니면 1byte확장
		iTotalLength += 1 ;
		usBuff = new unsigned short[iTotalLength/2];
		memcpy(usBuff, &m_pseudoHeader, pseudoHeaderLength);
		memcpy(usBuff+(pseudoHeaderLength/2), &m_header, iPacketLength);
		memcpy((unsigned char*)(usBuff)+iTotalLength-1, &padding, 1);
	}
	else	{
		usBuff = new unsigned short[iTotalLength/2];
		memcpy(usBuff, &m_pseudoHeader, pseudoHeaderLength);
		memcpy(usBuff+(pseudoHeaderLength/2), &m_header, iPacketLength);
	}

	while(iTotalLength>1)
	{
		usChksum += *usBuff++;
		iTotalLength -= sizeof(unsigned short);
	}

	if(iTotalLength)	// 1byte,,,, 들어갈일 없음...
		usChksum += *(unsigned char*)usBuff;

	usChksum = (usChksum >> 16) + (usChksum & 0xffff);
	usChksum += (usChksum >> 16);

	m_header.uh_sum = ((unsigned short)(~usChksum)) + 0x0600;
}

BOOL CUDP::IsCorrectCheckSum(ptrUDP header)
{
	//header 랑 m_pseudoHeader를 이용해서 체크썸이 맞는 지 확인해서 맞으면 TRUE, 아니면 FALSE리턴 플리즈

	return TRUE;
}

