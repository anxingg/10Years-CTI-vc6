#ifndef CFCQUEUE_H
#define CFCQUEUE_H


const int CFCQUEUE_BUFSIZE = 250;	//���д洢�ռ�
/*======================================================
note: CFCQUEUE_BUFSIZE > sizeof(CFC_MESSAGE_PACK) + 1
=======================================================*/
const int CFCQUEUE_SIZE = 100;		//���г���>=2

class CMsgRecord//��Ϣ����
{
public:
	void Clear( void ){memset(m_szBuf,0,CFCQUEUE_BUFSIZE);
				m_nMsgLen=0;};
	CMsgRecord(){memset(m_szBuf,0,CFCQUEUE_BUFSIZE);
				m_nMsgLen=0;	};
	virtual ~CMsgRecord(){};
	//CMsgRecord( const CMsgRecord &s ){ memcpy( m_szBuf, s.m_szBuf, CFCQUEUE_BUFSIZE ); m_nMsgLen = s.m_nMsgLen; };
	char m_szBuf[CFCQUEUE_BUFSIZE];//��Ϣ������
	int	m_nMsgLen;//��Ϣʵ�ʵĳ���
};

class CMsgNode
{
public:
	void Clear( void ){m_MsgRecord.Clear();
				pNext=NULL;pPrev=NULL;bIfFree=1;};
	CMsgNode(){pNext=NULL;pPrev=NULL;bIfFree=1;};
	virtual ~CMsgNode(){};
	//CMsgNode( const CMsgNode &s ){ pNext = s.pNext; pPrev = s.pPrev; bIfFree = s.bIfFree;  m_MsgRecord = s.m_MsgRecord; };
	CMsgNode *pNext;
	CMsgNode *pPrev;
	bool	   bIfFree;//��־�˽ڵ��Ƿ���ʵ�����ݣ�1��ʾû��ʵ�����ݣ�0��ʾ��ʵ������
	CMsgRecord m_MsgRecord;//��Ϣ����
	
};


class CCFQueue  
{
public:
	CCFQueue();
	virtual ~CCFQueue();
	//CCFQueue( const CCFQueue &s);
public:
	void Clear( void );
	bool front(char *pszBuf, int nBufLen );//ȡ�߶��еĵ�һ��
	bool push( const char * pszBuf ,int nBufLen );//�����еĺ������Ϣ����
	bool empty();//�ж϶����Ƿ�Ϊ��
	CMsgNode *pFree;//�սڵ�ָ��
	CMsgNode *pHead;
	CMsgNode *pLast;
	CMsgNode m_Queue[CFCQUEUE_SIZE];//���нڵ������

};

#endif 
