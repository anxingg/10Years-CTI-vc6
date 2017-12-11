// MsqDowith.cpp: implementation of the CMsgDealWith class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MsqDowith.h"
#include "ClientSocket.h"
#include "..//include//MSgPackDefine.h"
#include <fstream.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int MSGDEALWITH_SOCKET_STATE_BUFLEN = 300;//socket ״̬�¼����峤��



void WriteToLog(const char* szMsg) ;

UINT ClientRunModeSendProgress( LPVOID pParam )//Recive data thread
{

	CMsgDealWith* pMsgDealWith = ( CMsgDealWith*)pParam;
	if ( NULL == pMsgDealWith   )
	{
		return 0;   // if pMsgDealWith is not valid
	}

	
	CFC_MESSAGE_PACK MsgPackage;
	char szBuf[ CFCQUEUE_BUFSIZE ];
	int nLen = sizeof(MsgPackage);
	bool bTemp = false;

	memset( &MsgPackage, 0, sizeof(MsgPackage) );
	memset( szBuf, 0, CFCQUEUE_BUFSIZE );

	int nSendState = MSGDEALWITH_SEND_INIT_STATE;
	Client_SocketRecResult_T eSendResult;


	while(  !pMsgDealWith->m_bEndAllThread  )
	{
				
				bTemp = pMsgDealWith->m_CfcSocketClicent.m_bConnect;
				if( !bTemp )
				{
				
					/*===========================================
					���ڸ�socket�Ѿ��Ͽ��ˣ������߳̾���Ҫ������
					ͬʱ��Ҫ�ر�socket,������Ҫ�����߳̽������أ�
					֪ͨ�����߳̽�����
					============================================*/
					
						pMsgDealWith->m_CfcSocketClicent.Close();
					
					return 0;
				}
				
				
				bTemp = pMsgDealWith->m_SendQueue.empty();
				
				if( bTemp )
				{
					/*============================
					�ж϶������Ƿ�������,��Ϊ��;
					�ȴ�����ȥ����
					=============================*/
					Sleep(8);
					continue;
				}
				
				/*�ж϶������Ƿ������ݣ����ж������ݣ�ͬʱɾ�����Ͷ����е�����*/
				
				bTemp = pMsgDealWith->m_SendQueue.front( szBuf, nLen ) ;
				
				if( !bTemp )
				{
					//��ȡ����ʧ��
					Sleep(8);
					continue;
				}

				while( !pMsgDealWith->m_bEndAllThread )
				{
					
					eSendResult = pMsgDealWith->m_CfcSocketClicent.Send( szBuf,sizeof(MsgPackage) );
					
					if( eSendResult == Unfinishing )
					{
						/*=======================================================
						Because of Send buffers overflowed, so must add the data
						sent by the thread back the queue.
						��Ϊ����socket�ķ��ͻ�������������Ա��뽫�ôη��͵�����
						���·Żص����У����ڸ����ݴӶ����ж���ʱ��ɾ������������
						�ٻ����Բ���һ�����ݣ�����һ���ܲ��룬�ȴ��´η��͡��˳�
						����ѭ����
						========================================================*/
						
						Sleep(8);
						continue;

					}
					else if( eSendResult == Finishing )
					{	
						
						break;
					}
					else if( eSendResult == Closed )
					{
						break;
					}
					else if( eSendResult == Error )
					{	
						break;
					}
					else
					{
						break;
					}
				}//end while

				Sleep(8);

	}//end while


	return 0;


}

UINT ClientRunModeReciveProgress( LPVOID pParam )//Recive data thread
{
	CMsgDealWith* pMsgDealWith = ( CMsgDealWith*)pParam;
	if ( NULL == pMsgDealWith   )
	{
		return 0;   // if pMsgDealWith is not valid
	}


	int i = 0;
	int nRevBytes = 0;
	char szTemp[1] = {'0'};
	bool bConnect = false;
	int nError = 0;

	char szRecMsg[ CFCQUEUE_BUFSIZE ];	//ע��: ����������Ĵ�С��Ϊ����е����ݻ������Ĵ�Сһֱ
	sockaddr_in tempSockAddr;			//��ʱsockaddr_in����
	memset( szRecMsg, 0, CFCQUEUE_BUFSIZE);
	memset((char *) &tempSockAddr, 0, sizeof(tempSockAddr));

	int nReciveState = MSGDEALWITH_REC_INIT_STATE;
	bool bClientRegModule = false;
	while(  !pMsgDealWith->m_bEndAllThread  )
	{
		if( !pMsgDealWith->m_CfcSocketClicent.m_bConnect )
		{
			//�����߳̽������أ�֪ͨ�����߳̽���
			pMsgDealWith->m_CfcSocketClicent.Close();
			return 0;
			
		}
		switch(nReciveState)
		{
		case MSGDEALWITH_REC_INIT_STATE:
			/***********************************************************************************************************************/
								switch( pMsgDealWith->m_CfcSocketClicent.Recive( szRecMsg, '\xFF', sizeof( CFC_MESSAGE_PACK ) ) )
								{
								case Unfinishing:
									nReciveState = MSGDEALWITH_REC_INIT_STATE;
									break;
								case Finishing:
									//�����ݲ�����ն�����
									if( pMsgDealWith->m_RecQueue.push( szRecMsg, CFCQUEUE_BUFSIZE ) )
									{
										nReciveState = MSGDEALWITH_REC_INIT_STATE;
									}
									else
									{
										
										//����������������һ״̬���ȴ�����
										nReciveState = MSGDEALWITH_REC_MSG_STATE1;
									}
									//send socket state event
								
									//�ж��ǲ�������ģ��ע����Ϣ�����Ǿ�ע��
									if( !bClientRegModule )
									{
										if( pMsgDealWith->RegClientModuleInfo( szRecMsg, sizeof( CFC_MESSAGE_PACK ) ) )
										{
											//ע��ɹ�
											bClientRegModule = true;
										
										}
										else
										{
											bClientRegModule = true;
											
										}

									}

									
									break;
								case Closed:
									//send socket state event

									break;
								case Error:

									break;
								default:
									break;

								}//end switch
			/***********************************************************************************************************************/
			
			break;
		case MSGDEALWITH_REC_MSG_STATE1:
			/***********************************************************************************************************************/
								//�����ݲ�����ն�����,ֱ���ɹ����룬Ȼ��ʼ��һ�ν�������
								pMsgDealWith->m_CfcSocketClicent.ReadBuf( szRecMsg, CFCQUEUE_BUFSIZE );
								if( pMsgDealWith->m_RecQueue.push( szRecMsg, CFCQUEUE_BUFSIZE ) )
								{
									//����ɹ�
									nReciveState = MSGDEALWITH_REC_INIT_STATE;
								}

								//�ж��ǲ�������ģ��ע����Ϣ�����Ǿ�ע��
								if( !bClientRegModule )
								{
									if( pMsgDealWith->RegClientModuleInfo( szRecMsg, sizeof( CFC_MESSAGE_PACK ) ) )
									{
										bClientRegModule = true;
										
									}
									else
									{
										bClientRegModule = false;

									}

								}
			/***********************************************************************************************************************/
			
			break;
		default:
			
			break;

		}//end switch
		
		Sleep(8);

	}//end while
	

	return 0;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgDealWith::CMsgDealWith()
{
	m_hReciveThreadhandle = NULL;//��ǰ�������ݵ��߳̾��
	m_hSendThreadhandle = NULL;//��ǰ�������ݵ��߳̾��

	m_bEndAllThread = true;



	m_nSMType = DMODULARTYPE_BROKER;
	m_nSMID = 0;
	memset(&m_Addr, 0, sizeof(m_Addr));

}


CMsgDealWith::~CMsgDealWith()
{
	ExitInstance();

}


//Client run mode initalize
bool CMsgDealWith::Initialize(unsigned int Port, const char *IP, 	int nSMType, int nSMID )
{
	bool bResult = false;

	if (!AfxSocketInit())
	{

		return FALSE;
	}
	


	m_nSMID = nSMID;
	m_nSMType = nSMType;

	
	//Creat client socket
	m_Addr.sin_family = AF_INET;
    m_Addr.sin_port = (unsigned short)Port ;
    m_Addr.sin_addr.S_un.S_addr = inet_addr(IP);

	SOCKET cli_sock = INVALID_SOCKET;
	sockaddr_in  cli_addr;

	memset(&cli_addr,0,sizeof(cli_addr));
	cli_addr.sin_family=AF_INET;
	cli_addr.sin_addr.s_addr=INADDR_ANY;        
	cli_addr.sin_port=0;    

	cli_sock = socket(PF_INET,SOCK_STREAM,0);
	if (cli_sock==INVALID_SOCKET)
	{
		//AfxMessageBox("Create Socket Error !");
		return false;
	}

	if (bind(cli_sock,(LPSOCKADDR)&cli_addr,sizeof(cli_addr))==SOCKET_ERROR)
	{
		//AfxMessageBox("Bind socket Error!");
		closesocket(cli_sock);
		return false;
	}

	if (connect(cli_sock,(LPSOCKADDR)&m_Addr,sizeof(m_Addr))==SOCKET_ERROR)
	{ 

		//AfxMessageBox("connect false");
		closesocket(cli_sock);
		return false;
	}

	memcpy( &(m_CfcSocketClicent.m_addr), &cli_addr ,sizeof(cli_addr) );
	m_CfcSocketClicent.m_socket = cli_sock;

	/********************************************************/
	//ע�⣺һ������Ϊ�첽��Socket������ͻ�������벻���Ĵ���
	unsigned long ioctl_opt = 1;
	m_CfcSocketClicent.Ioctlsocket(FIONBIO , &ioctl_opt );
	/******************************************************/

	m_CfcSocketClicent.m_bConnect = true;



	m_bEndAllThread = false;

	//Creat clent Recive data thread
	CWinThread* pReciveThread = AfxBeginThread(ClientRunModeReciveProgress, this,  THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	::DuplicateHandle(GetCurrentProcess(), pReciveThread->m_hThread,  GetCurrentProcess()  , &m_hReciveThreadhandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	ResumeThread(m_hReciveThreadhandle);

	//AfxBeginThread( ClientRunModeReciveProgress, this, NULL );

	//Creat clent Send data thread
	CWinThread* pSendThread = AfxBeginThread(ClientRunModeSendProgress, this,  THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	::DuplicateHandle(GetCurrentProcess(), pSendThread->m_hThread,  GetCurrentProcess()  , &m_hSendThreadhandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	ResumeThread(m_hSendThreadhandle);

	//AfxBeginThread( ClientRunModeSendProgress, this, NULL );

	return true;
}

void CMsgDealWith::ExitInstance()
{

	m_CfcSocketClicent.Close();

	m_bEndAllThread = true;

	while(1)
	{
		DWORD   dwExitCode = 0;
		GetExitCodeThread( m_hReciveThreadhandle, &dwExitCode );
		if( STILL_ACTIVE == dwExitCode )
		{
			Sleep(100);
			continue;
		}
		else
		{
			break;
		}
	}
	while(1)
	{
		DWORD   dwExitCode = 0;
		GetExitCodeThread( m_hSendThreadhandle, &dwExitCode );
		if( STILL_ACTIVE == dwExitCode )
		{
			Sleep(100);
			continue;
		}
		else
		{
			break;
		}
	}

	m_RecQueue.Clear();
	m_SendQueue.Clear();
	
	m_hReciveThreadhandle = NULL;//��ǰ�������ݵ��߳̾��
	m_hSendThreadhandle = NULL;//��ǰ�������ݵ��߳̾��
	memset(&m_Addr, 0, sizeof(m_Addr));

	m_nSMType = DMODULARTYPE_BROKER;
	m_nSMID = 0;

}



bool CMsgDealWith::ReadData(char *buf, int nLen)
{
	if( m_bEndAllThread ){
		return false;
	}
	
	bool bHaveData = false;

	
	if( m_RecQueue.empty() )
	{
		//û�����ݿɶ�
		bHaveData = false;
	}
	else
	{
		if( m_RecQueue.front( buf, nLen ) )
		{
			//�ɹ���ȡ����
			bHaveData =  true;
		}
		else
		{
			//û�ж�������
			bHaveData = false;
		}
	}




	return bHaveData;
}

bool CMsgDealWith::SendData(char *buf, int nLen)
{
	if( m_bEndAllThread ){
		return false;
	}
	
	bool bResult = false;



	if( m_SendQueue.push( buf,nLen) )
	{
		bResult = true;
	}
	else
	{
		bResult = false;
	}




	return bResult;

}


bool CMsgDealWith::RegClientModuleInfo( char *buf, int nLen)
{

	if( buf == NULL )
		return false;


	bool bResult = true;
	CFC_MESSAGE_PACK MsgPackage;
	
	memcpy( &MsgPackage, buf, sizeof(MsgPackage));
	if( MSGDEALWITH_MOUAL_REG_MSG == MsgPackage.nMsgType )//�ж��ǲ���ע����Ϣ
	{

		m_CfcSocketClicent.m_nDMType = MsgPackage.nDMType;
		m_CfcSocketClicent.m_nDMID = MsgPackage.nDMID;
		m_CfcSocketClicent.m_nSMType = MsgPackage.nSMType;
		m_CfcSocketClicent.m_nSMID = MsgPackage.nSMID;
		m_CfcSocketClicent.m_nSecDMType  = MsgPackage.nSecDMType;
		m_CfcSocketClicent.m_nSecDMID  = MsgPackage.nSecDModularID;
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	
	return bResult;
}

bool CMsgDealWith::SocketConnect()
{
	bool bResult = false;
	bResult = m_CfcSocketClicent.m_bConnect;
	return bResult;
}
