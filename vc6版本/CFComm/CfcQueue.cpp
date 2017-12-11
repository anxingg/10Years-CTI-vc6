// PsgQueue.cpp: implementation of the CCFQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ListTest.h"
#include "CfcQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCFQueue::CCFQueue()
{

	pFree = NULL;//ָ��Ľڵ㵱ǰ�ǿյ�
	pHead = NULL;
	pLast = NULL;
	pHead = &m_Queue[0];
	pFree = &m_Queue[0];
	pHead->pPrev = NULL;
	pHead->pNext = &m_Queue[1];
	for( int i=1; i < CFCQUEUE_SIZE-1; i++ )
	{
		m_Queue[i].pPrev=&m_Queue[i-1];
		m_Queue[i].pNext=&m_Queue[i+1];
	}
	this->pLast=&m_Queue[CFCQUEUE_SIZE-1];
	this->pLast->pNext=NULL;
	this->pLast->pPrev=&m_Queue[CFCQUEUE_SIZE-2];
}

CCFQueue::~CCFQueue()
{
	pFree = NULL;//ָ��Ľڵ㵱ǰ�ǿյ�
	pHead = NULL;
	pLast = NULL;
	

}

/*
CCFQueue::CCFQueue( const CCFQueue &s)
{
	pFree = new 
	pFree = s.pFree;
	pHead = s.pHead;
	pLast = s.pLast;
	memcpy( m_Queue , s.m_Queue, CFCQUEUE_SIZE) ;

}
*/
/***************************************************
���ܣ�ȡ�߶��еĵ�һ��
���������
	��
���������
	pszBuf����Ϣ����
	nBufLen����Ϣ����
����ֵ���ɹ������棬ʧ�ܷ��ؼ�
***************************************************/
bool CCFQueue::front(char *pszBuf, int nBufLen )//
{
	CMsgNode *ptempNode=NULL;//��ʱ�ڵ�ָ��
	if(pHead)
	{
		if(!pHead->bIfFree)//��Ϊ��
		{

			ptempNode=pHead;

			if( nBufLen > pHead->m_MsgRecord.m_nMsgLen )//new add  by ruize
			{
				memcpy(pszBuf,pHead->m_MsgRecord.m_szBuf,pHead->m_MsgRecord.m_nMsgLen);
			}
			else
			{
				memcpy(pszBuf,pHead->m_MsgRecord.m_szBuf, nBufLen );
			}
			//memcpy(pszBuf,pHead->m_MsgRecord.m_szBuf, nBufLen );
			//memcpy(pszBuf,pHead->m_MsgRecord.m_szBuf,pHead->m_MsgRecord.m_nMsgLen);
			//*nBufLen=pHead->m_MsgRecord.m_nMsgLen;
			pHead=pHead->pNext;//�ƶ�pHeadָ�룬ָ����һ���ڵ�
			pHead->pPrev=NULL;//ʹ�ýڵ��Ϊͷ��

			//��λȡ�ߵĽڵ����ݣ��������ƶ����������
			ptempNode->bIfFree=true;
			ptempNode->m_MsgRecord.m_nMsgLen=0;
			memset(ptempNode->m_MsgRecord.m_szBuf,0,CFCQUEUE_BUFSIZE);
			pLast->pNext=ptempNode;
			ptempNode->pPrev=pLast;//
			ptempNode->pNext=NULL;
			pLast=ptempNode;//�ƶ�pLastָ�룬ָ�����һ���ڵ�

			//ȡ��һ����Ϣ�󣬿���pFreeָ���Ƿ�ΪNULL(ΪNULL��ʾ���ڶ������������ȡ�ߵ���Ϣ)
			if(!pFree)
				pFree=pLast;//ָ�����һ���ڵ�
			
		}
		else
			return false;
	}
	else
		return false;
	return true;
}
/***************************************************
���ܣ������еĺ������Ϣ����
���������
	pszBuf����Ϣ����
	nBufLen����Ϣ����
���������
����ֵ���ɹ�Ϊ�棬ʧ��Ϊ��
***************************************************/

bool CCFQueue::push( const char * pszBuf ,int nBufLen )//
{
	if(nBufLen > CFCQUEUE_BUFSIZE)
		return false;
	if(pFree)//
	{
		if(pFree->bIfFree)
		{//������Ϣ���ڵ���
			pFree->bIfFree=false;//��Ϊ�ǿ�
			memcpy(pFree->m_MsgRecord.m_szBuf,pszBuf,nBufLen);
			pFree->m_MsgRecord.m_nMsgLen=nBufLen;
			pFree=pFree->pNext;//�ƶ�pPreeָ�뵽��һ���ڵ�
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}
/***************************************************
���ܣ��ж϶����Ƿ�Ϊ��
�����������
�����������
����ֵ������Ϊ��Ϊ�棬��Ϊ��Ϊ��
***************************************************/

bool CCFQueue::empty()//
{
	if(pHead->bIfFree)//��һ���ڵ㶼Ϊ�գ��ö��м�Ϊ��
		return true;
	else
		return false;
}

void CCFQueue::Clear()
{
	pFree = NULL;//ָ��Ľڵ㵱ǰ�ǿյ�
	pHead = NULL;
	pLast = NULL;
	pHead = &m_Queue[0];
	pFree = &m_Queue[0];
	pHead->pPrev = NULL;
	pHead->pNext = &m_Queue[1];
	int i=1;
	for(  i=1; i < CFCQUEUE_SIZE-1; i++ )
	{
		m_Queue[i].Clear();
	}
	for(  i=1; i < CFCQUEUE_SIZE-1; i++ )
	{
		m_Queue[i].pPrev=&m_Queue[i-1];
		m_Queue[i].pNext=&m_Queue[i+1];
	}
	this->pLast=&m_Queue[CFCQUEUE_SIZE-1];
	this->pLast->pNext=NULL;
	this->pLast->pPrev=&m_Queue[CFCQUEUE_SIZE-2];
}
