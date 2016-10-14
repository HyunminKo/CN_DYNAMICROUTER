// RoutingTable.cpp: implementation of the CRoutingTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouter.h"
#include "RoutingTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRoutingTable::CRoutingTable(char *pName )
	: CBaseLayer( pName )
{
	for(int i=0;i<MAX_ROUTING_TABLE;i++)
		m_routingTable[i]=NULL;
}

CRoutingTable::~CRoutingTable()
{

}
