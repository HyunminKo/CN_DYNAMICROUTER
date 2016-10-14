// BaseLayer.cpp: implementation of the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseLayer::CBaseLayer( char* pName ) 
: m_nUpperLayerCount( 0 ), 
  mp_UnderLayer( NULL )
{
	m_pLayerName = pName ;
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}

//////////////////////// fill the blank ///////////////////////////////
	mp_UnderLayer=pUULayer;
	pUULayer->mp_aUpperLayer[pUULayer->m_nUpperLayerCount++]=this;
	//현재 Layer의 UnderLayer를 설정하고, 
	//UnderLayer의 UpperLayer pointer를 현재 Layer로 설정한다.
///////////////////////////////////////////////////////////////////////
}

void CBaseLayer::SetUpperUnderLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}

//////////////////////// fill the blank ///////////////////////////////
	mp_aUpperLayer[m_nUpperLayerCount++]=pUULayer;
	pUULayer->mp_UnderLayer=this;	
	//현재 Layer의 UpperLayer를 설정하고, 
	//UpperLayer의 UnderLayer pointer를 현재 Layer로 설정한다.
///////////////////////////////////////////////////////////////////////
}

void CBaseLayer::SetUpperLayer(CBaseLayer *pUpperLayer )
{
	if ( !pUpperLayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL" ) ;
#endif
		return ;			
	}

	// UpperLayer is added..
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUpperLayer ;
}

void CBaseLayer::SetUnderLayer(CBaseLayer *pUnderLayer)
{
	if ( !pUnderLayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n" ) ;
#endif
		return ;			
	}

	// UnderLayer assignment..
	this->mp_UnderLayer = pUnderLayer ;
}

CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if ( nindex < 0						||
		 nindex > m_nUpperLayerCount	||
		 m_nUpperLayerCount < 0			)
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_aUpperLayer[ nindex ] ;
}

CBaseLayer* CBaseLayer::GetUnderLayer()
{
	if ( !mp_UnderLayer )
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_UnderLayer ;
}

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName ; 
}

