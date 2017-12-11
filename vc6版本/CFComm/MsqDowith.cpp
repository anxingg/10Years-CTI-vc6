// MsqDowith.cpp: implementation of the CMsgDealWith class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Afxmt.h>
#include "CFComm.h"
#include "MsqDowith.h"
#include "ClientSocket.h"
#include "MSgPackDefine.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int MSGDEALWITH_SOCKET_STATE_BUFLEN = 300;//socket ״̬�¼����峤��

CCriticalSection section;


//Listen thread
UINT ServerRunModeListenProgress( LPVOID pParam )
{
	
	CMsgDealWith* pMsgDealWith = ( CMsgDealWith*)pParam;
	if ( NULL == pMsgDealWith   )
	{
		AfxMessageBox("The pMsgDealWith is not valid,\n the Accept client socket connect request thread is ended! ");
		return 0;   // if pMsgDealWith is not valid
	}

	section.Lock();
		pMsgDealWith->m_nThreadCount += 1;
	section.Unlock();

	//Creat listen socket code block
	pMsgDealWith->m_lpfuncExport( "[Listen thread Begin]",CFCOMM_EVENT_SYS_MSG,0 );
	pMsgDealWith->m_ListenSocket = socket(PF_INET,SOCK_STREAM,0); 
	if( INVALID_SOCKET == pMsgDealWith->m_ListenSocket )
	{
		pMsgDealWith->m_lpfuncExport( "[creat Listen socket Error]",CFCOMM_EVENT_SYS_MSG,0 );
		section.Lock();
			pMsgDealWith->m_nThreadCount -= 1;
		section.Unlock();
         return 0;
	}

	sockaddr_in local;
    memset(&local, 0, sizeof(local));
	memcpy( &local, &(pMsgDealWith->m_Addr) ,sizeof(local) );
	if( SOCKET_ERROR == bind( pMsgDealWith->m_ListenSocket, (sockaddr *)(&local),sizeof(local)) )
	{
		pMsgDealWith->m_lpfuncExport( "[Bind Listen socket Error]",CFCOMM_EVENT_SYS_MSG,0 );
		section.Lock();
			pMsgDealWith->m_nThreadCount -= 1;
		section.Unlock();
         return 0;
	}
	
	 
	if( SOCKET_ERROR == listen( pMsgDealWith->m_ListenSocket,10 ) )
	{
		pMsgDealWith->m_lpfuncExport( "[Listen socket Error]",CFCOMM_EVENT_SYS_MSG,0 );
		section.Lock();
			pMsgDealWith->m_nThreadCount -= 1;
		section.Unlock();
		return 0;
	}

	//set the listen socket to blocking mode
	//closesocket( pMsgDealWith->m_ListenSocket );
	ioctlsocket( pMsgDealWith->m_ListenSocket, FIONBIO,0);
	char szSocketStateMsgBuf[ MSGDEALWITH_SOCKET_STATE_BUFLEN ];//socket state show string
	int nClientAddrLen = 0;
	sockaddr_in ClientAddr;
	SOCKET NewSocketFd = INVALID_SOCKET;
	
	memset(szSocketStateMsgBuf, 0,  sizeof(szSocketStateMsgBuf)/sizeof(szSocketStateMsgBuf[0]) );
	memset((char *) &ClientAddr, 0, sizeof(ClientAddr));
	nClientAddrLen = sizeof(ClientAddr);
	
	while( !pMsgDealWith->m_bEndAllThread )
	{
		NewSocketFd = accept( pMsgDealWith->m_ListenSocket, (sockaddr *)&ClientAddr, &nClientAddrLen);
		if( INVALID_SOCKET == NewSocketFd )
		{
			Sleep(1);
			continue;
		}
		//judge scocket protocol
		if( AF_INET != ClientAddr.sin_family )
		{
			//not know protocol
			closesocket(NewSocketFd);
			sprintf( szSocketStateMsgBuf,"[ Not know protocol][IP:%s]  [Port:%d]",inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port) );
			pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf,CFCOMM_EVENT_SYS_MSG,0 );
			Sleep(1);
			continue;
		}

		//Creat new socket code block
		section.Lock();
			for( int i = 0; i< MSG_MAX_SOCKET_NUM; i++ )
			{
				if( !pMsgDealWith->m_NewSocket[i].m_bConnect )
				{
					pMsgDealWith->m_NewSocket[i].Initialize();
					pMsgDealWith->m_NewSocket[i].m_socket = NewSocketFd;
					pMsgDealWith->m_NewSocket[i].m_bConnect = true;
					pMsgDealWith->m_NewSocket[i].m_bNetAbend = true;
					memcpy( &(pMsgDealWith->m_NewSocket[i].m_addr), &ClientAddr ,sizeof(ClientAddr) );
					
					//Note :set the client socket to nonblocking mode
					//ע�⣺һ������Ϊ�첽��Socket������ͻ�������벻���Ĵ���
					unsigned long ioctl_opt = 1;
					pMsgDealWith->m_NewSocket[i].Ioctlsocket(FIONBIO , &ioctl_opt );

					sprintf( szSocketStateMsgBuf,"[ New Connect][%d][IP:%s][Port:%d]",i,inet_ntoa(ClientAddr.sin_addr),ntohs(ClientAddr.sin_port) );
					pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf,CFCOMM_EVENT_SOCKET_MSG,0 );
					
					break;
				}
			}
		section.Unlock();
		Sleep(20);
		
	}

	
//	pMsgDealWith->m_lpfuncExport( "[Listen thread end]",CFCOMM_EVENT_SYS_MSG,0 );
	section.Lock();
		pMsgDealWith->m_nThreadCount -= 1;
	section.Unlock();

	return 0;//Thread end
}



UINT ServerRunModeReciveProgress( LPVOID pParam )//Recive data thread
{
	CMsgDealWith* pMsgDealWith = ( CMsgDealWith*)pParam;
	if ( NULL == pMsgDealWith   )
	{
		AfxMessageBox("The pMsgDealWith is not valid,\n the Recive thread is ended! ");
		return 0;   // if pMsgDealWith is not valid
	}
	
	pMsgDealWith->m_lpfuncExport( "[Recive thread Begin]",CFCOMM_EVENT_SYS_MSG,0 );
	section.Lock();
		pMsgDealWith->m_nThreadCount += 1;
	section.Unlock();

	int i			= 0;
	bool bConnect	= false;
	char szSocketStateMsgBuf[ MSGDEALWITH_SOCKET_STATE_BUFLEN ];
	//ע��: ����������Ĵ�С��Ϊ����е����ݻ������Ĵ�Сһֱ
	char szRecMsg[ CFCQUEUE_BUFSIZE ];
	sockaddr_in tempSockAddr;//��ʱsockaddr_in����
	memset( szSocketStateMsgBuf, 0,  sizeof(szSocketStateMsgBuf)/sizeof(szSocketStateMsgBuf[0]) );
	memset( szRecMsg, 0, CFCQUEUE_BUFSIZE);
	memset((char *) &tempSockAddr, 0, sizeof(tempSockAddr));
	

	while(!pMsgDealWith->m_bEndAllThread )
	{
			section.Lock();
			for( i = 0; i < MSG_MAX_SOCKET_NUM; i++ )
			{
				//Judge the socket is conneting
				bConnect = pMsgDealWith->m_NewSocket[i].m_bConnect;
				if( !bConnect )
				{
					//net abend dealwith
					pMsgDealWith->NetAbendDealWith(i);
					continue;
				}
				//����״̬��
				switch( pMsgDealWith->m_nReciveState[i] )
				{
					case MSGDEALWITH_REC_INIT_STATE://Begin recive data state
						/***************************************************************************************/
									//recive data and judge connecting state
									switch( pMsgDealWith->m_NewSocket[i].Recive( szRecMsg, '\xFF', sizeof( CFC_MESSAGE_PACK ) ) )
									{
										case Unfinishing:
											pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
											break;
										case Finishing:
											/*******************************************************************/

													//send socket state event
													
													sprintf( szSocketStateMsgBuf,"[MType:%d][From MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType ,pMsgDealWith->m_NewSocket[i].m_nSMID,
															i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
															ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

													pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, pMsgDealWith->m_NewSocket[i].m_nSMType  ,0 );

													//�ж��ǲ�������ģ��ע����Ϣ�����Ǿ�ע��
													if( !pMsgDealWith->m_bClientRegModule[i] )
													{
														if( pMsgDealWith->RegClientModuleInfo(i, szRecMsg, sizeof( CFC_MESSAGE_PACK ) ) )
														{

															/*=============================================================
															���ע��ɹ�����Ҫ����ע����Ϣ�ӵ����ն����У�������Ϣ���ظ�ע��
															���𷽡�
															==============================================================*/
															CFC_MESSAGE_PACK MsgPackage;
															memset( &MsgPackage, 0, sizeof(MsgPackage) );
															memcpy( &MsgPackage, szRecMsg, sizeof(MsgPackage));
															
															MsgPackage.nSecDModularID = i;
															MsgPackage.nSecDMType = i;

															if( pMsgDealWith->m_RecQueue.push( (char*)&MsgPackage, CFCQUEUE_BUFSIZE ) )
															{
																sprintf( szSocketStateMsgBuf,"[MType:%d][ Reg Success From MID:%d][%d][IP:%s][Port:%d]",
																		pMsgDealWith->m_NewSocket[i].m_nSMType,pMsgDealWith->m_NewSocket[i].m_nSMID,
																		i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
																		ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

																pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );

																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
															}
															else
															{
																/*===========================================
																�������������������һ״̬���ȴ����룬ͬʱ��
																ע����ص���Ϣ�ָ���δע��״̬
																============================================*/
																pMsgDealWith->m_NewSocket[i].m_nSMType = 0;
																pMsgDealWith->m_NewSocket[i].m_nSMID = 0; 
																pMsgDealWith->m_bClientRegModule[i] = false;

																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_MSG_STATE1;
																pMsgDealWith->m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );
																break;
															}


														}
														else
														{
															/*===========================================================
															û��ע��ɹ�
															=============================================================*/
															CFC_MESSAGE_PACK MsgPackage;
															memset( &MsgPackage, 0, sizeof(MsgPackage) );
															memcpy( &MsgPackage, szRecMsg, sizeof(MsgPackage));
															
															MsgPackage.nSecDModularID = i;
															MsgPackage.nSecDMType = -1;

															if( pMsgDealWith->m_RecQueue.push( (char*)&MsgPackage, CFCQUEUE_BUFSIZE ) )
															{
																sprintf( szSocketStateMsgBuf,"[ Reg false][%d][IP:%s][Port:%d]",
																		i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
																		ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

																pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );

																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
															}
															else
															{
																/*===========================================
																�������������������һ״̬���ȴ����룬ͬʱ��
																ע����ص���Ϣ�ָ���δע��״̬
																============================================*/
																pMsgDealWith->m_NewSocket[i].m_nSMType = 0;
																pMsgDealWith->m_NewSocket[i].m_nSMID = 0; 
																pMsgDealWith->m_bClientRegModule[i] = false;

																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_MSG_STATE1;
																pMsgDealWith->m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );
																break;
															}

														
														}


													}
													else
													{
															/*===========================================================
															����Ѿ�ע��ֱ�ӽ�����Ϣ�������
															============================================================*/
															if( pMsgDealWith->m_RecQueue.push( szRecMsg, CFCQUEUE_BUFSIZE ) )
															{
																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
															}
															else
															{
																
																//����������������һ״̬���ȴ�����
																pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_MSG_STATE1;
																pMsgDealWith->m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );
																break;
															}
													}

											/******************************************************************************/
											break;
										case Closed:
											//send socket state event
											sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]",
													pMsgDealWith->m_NewSocket[i].m_nSMType,pMsgDealWith->m_NewSocket[i].m_nSMID,
													i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
													ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );
											
											//pMsgDealWith->ClearClientRegInfo(i);//���ģ��ע����Ϣ
											pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf,CFCOMM_EVENT_SOCKET_MSG  ,0 );
											break;
										case Error:
											//send socket state event
											sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]",
													pMsgDealWith->m_NewSocket[i].m_nSMType,pMsgDealWith->m_NewSocket[i].m_nSMID,
													i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
													ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );
											
											//pMsgDealWith->ClearClientRegInfo(i);//���ģ��ע����Ϣ
											pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf,CFCOMM_EVENT_SOCKET_MSG  ,0 );
											break;
										default:
											break;

									}//end switch
						/*******************************************************************************************/
						break;
					case MSGDEALWITH_REC_MSG_STATE1:
									/*====================================================================================
									ʹ�ø�״̬��Ŀ���ǵ����ն�����ʱ�����ڽ������ݣ������ݲ�����ն�����,ֱ���ɹ����룬
									Ȼ��ʼ��һ�ν�������
									======================================================================================*/
									pMsgDealWith->m_NewSocket[i].ReadBuf( szRecMsg, CFCQUEUE_BUFSIZE );

									//�ж��ǲ�������ģ��ע����Ϣ�����Ǿ�ע��
									if( !pMsgDealWith->m_bClientRegModule[i] )
									{
										if( pMsgDealWith->RegClientModuleInfo(i, szRecMsg, sizeof( CFC_MESSAGE_PACK ) ) )
										{
											//ע��ɹ�
											CFC_MESSAGE_PACK MsgPackage;
											memset( &MsgPackage, 0, sizeof(MsgPackage) );
											memcpy( &MsgPackage, szRecMsg, sizeof(MsgPackage));
											
											MsgPackage.nSecDModularID = i;
											MsgPackage.nSecDMType = i;
											if( pMsgDealWith->m_RecQueue.push( (char*)&MsgPackage, CFCQUEUE_BUFSIZE ) )
											{
												//����ɹ�
												pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;

												sprintf( szSocketStateMsgBuf,"[MType:%d][Reg Success From MID:%d][%d][IP:%s][Port:%d]",
														pMsgDealWith->m_NewSocket[i].m_nSMType,pMsgDealWith->m_NewSocket[i].m_nSMID,
														i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
														ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

												pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );

											}
											else
											{
												/*===========================================
												�������������������һ״̬���ȴ����룬ͬʱ��
												ע����ص���Ϣ�ָ���δע��״̬
												============================================*/
												pMsgDealWith->m_NewSocket[i].m_nSMType = 0;
												pMsgDealWith->m_NewSocket[i].m_nSMID = 0; 
												pMsgDealWith->m_bClientRegModule[i] = false;

												pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_MSG_STATE1;
												pMsgDealWith->m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );

											}


										}
										else
										{
												/*===========================================================
															û��ע��ɹ�
												=============================================================*/
												CFC_MESSAGE_PACK MsgPackage;
												memset( &MsgPackage, 0, sizeof(MsgPackage) );
												memcpy( &MsgPackage, szRecMsg, sizeof(MsgPackage));
												
												MsgPackage.nSecDModularID = i;
												MsgPackage.nSecDMType = -1;

												if( pMsgDealWith->m_RecQueue.push( (char*)&MsgPackage, CFCQUEUE_BUFSIZE ) )
												{
													sprintf( szSocketStateMsgBuf,"[ Reg false][%d][IP:%s][Port:%d]",
															i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
															ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

													pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );

													pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
												}
												else
												{
													/*===========================================
													�������������������һ״̬���ȴ����룬ͬʱ��
													ע����ص���Ϣ�ָ���δע��״̬
													============================================*/
													pMsgDealWith->m_NewSocket[i].m_nSMType = 0;
													pMsgDealWith->m_NewSocket[i].m_nSMID = 0; 
													pMsgDealWith->m_bClientRegModule[i] = false;

													pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_MSG_STATE1;
													pMsgDealWith->m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );
													break;
												}

										}

									}
									else
									{
										/*===========================================================
										����Ѿ�ע��ֱ�ӽ�����Ϣ�������
										============================================================*/
										if( pMsgDealWith->m_RecQueue.push( szRecMsg, CFCQUEUE_BUFSIZE ) )
										{
											//����ɹ�
											pMsgDealWith->m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
										}

									}
						/***********************************************************************************************/
						break;
					case MSGDEALWITH_REC_MSG_STATE2:

						break;
					default:
						break;
				}//end swith


			}//end for
			
		
			section.Unlock();
			Sleep(20);

	}//end while
	

	//pMsgDealWith->m_lpfuncExport( "[Recive thread end]",CFCOMM_EVENT_SYS_MSG,0 );
	section.Lock();
		pMsgDealWith->m_nThreadCount -= 1;
	section.Unlock();
	return 0;

}

//Send data thread
UINT ServerRunModeSendProgress( LPVOID pParam )//Recive data thread
{
	CMsgDealWith* pMsgDealWith = ( CMsgDealWith*)pParam;
	if ( NULL == pMsgDealWith   )
	{
		AfxMessageBox("The pMsgDealWith is not valid,\n the Recive thread is ended! ");
		return 0;   // if pMsgDealWith is not valid
	}
	
	pMsgDealWith->m_lpfuncExport( "[Send thread Begin]",CFCOMM_EVENT_SYS_MSG,0 );
	section.Lock();
		pMsgDealWith->m_nThreadCount += 1;
	section.Unlock();

	CFC_MESSAGE_PACK MsgPackage;
	char szSocketStateMsgBuf[ MSGDEALWITH_SOCKET_STATE_BUFLEN ];
	char szBuf[ CFCQUEUE_BUFSIZE ];
	int nLen = sizeof(MsgPackage);
	bool bSendResult = false;//�������ݽ����true���ҵ�Ŀ�겢�ɹ�������false��δ�ҵ�Ŀ�����������
	char szMsgNoDest[300];
	bool bTemp = false;
	Client_SocketRecResult_T eSendResult;

	memset( &MsgPackage, 0, sizeof(MsgPackage) );
	memset( szSocketStateMsgBuf, 0, MSGDEALWITH_SOCKET_STATE_BUFLEN );
	memset( szBuf, 0, CFCQUEUE_BUFSIZE );
	memset( szMsgNoDest, 0, 300 );
	

	while( !pMsgDealWith->m_bEndAllThread )
	{
		bSendResult = false;
		//�ж϶������Ƿ�������
		section.Lock();
		bTemp = pMsgDealWith->m_SendQueue.empty();
		section.Unlock();
		if( bTemp )
		{
			Sleep(1);
			continue;
		}
		
		//�ж϶������Ƿ������ݣ����ж������ݣ�ͬʱɾ�����Ͷ����е�����
		section.Lock();
		bTemp = pMsgDealWith->m_SendQueue.front( szBuf, nLen );
		section.Unlock();
		if( !bTemp )
		{
			//��ȡ����ʧ��
			Sleep(1);
			continue;
		}

		memcpy( &MsgPackage, szBuf ,sizeof(MsgPackage) );
		int i = 0;
		//ȡ�õ�����copy ����Ϣ����,Ȼ��ʼѰ��Ŀ�꣬��û�������Ϣ��ȡ��
		if( 0x2000 == MsgPackage.nMsgType )//��ע����Ϣ
		{
					i = MsgPackage.nSecDModularID;//��ֵ����socket�ı��
					if( i < 0 || i >=MSG_MAX_SOCKET_NUM ){
						continue;
					}
					while( !pMsgDealWith->m_bEndAllThread )
					{
							section.Lock();
							eSendResult = pMsgDealWith->m_NewSocket[i].Send( szBuf,sizeof(MsgPackage) );
							section.Unlock();

							if( Unfinishing == eSendResult )
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
								/*
								pMsgDealWith->m_SendQueue.push( szBuf, CFCQUEUE_BUFSIZE ) ;
								
								bSendResult = true;
								break;
								*/
							}
							else if( Finishing == eSendResult )
							{
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][To MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, pMsgDealWith->m_NewSocket[i].m_nSMType ,0 );
								
								bSendResult = true;//Send success
								section.Unlock();
								break;
							}
							else if( Closed == eSendResult )
							{	
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );
								section.Unlock();

								break;
							}
							else if( Error == eSendResult )
							{
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );
								section.Unlock();
								break;
							}
							else
							{

								break;
							}

											
					/*********************************************************************************************/
					}//end while
				continue;
		}


		
		
		for( i = 0; i < MSG_MAX_SOCKET_NUM ; i++ )
		{
					
					/*�ж϶�Ӧ��Socket�Ƿ��ѽ�������*/
					section.Lock();
						bTemp = pMsgDealWith->m_NewSocket[i].m_bConnect;
					section.Unlock();
					if( !bTemp )
					{
						section.Lock();
						pMsgDealWith->NetAbendDealWith(i);
						section.Unlock();
						continue;
					}
					/*===========================================================================
					Ѱ��Ŀ��Socket,���ȱȽ�Ŀ��socket��ModularType�������ȣ����ж�socket��
					DModularID�������Ϣ��DModularID=0���ͷ���ModularType��ͬ������socket;
					�����Ϣ��DModularID != 0,���ҵ�������ȵ�socket
					============================================================================*/
					section.Lock();
					bTemp = pMsgDealWith->m_NewSocket[i].m_nSMType != MsgPackage.nDMType;
					section.Unlock();
					if( bTemp )
					{
						continue;
					}

					if( MsgPackage.nDMID != 0 )
					{
						section.Lock();
						bTemp = pMsgDealWith->m_NewSocket[i].m_nSMID != MsgPackage.nDMID;
						section.Unlock();
						if( bTemp )
							continue;
					}

					
					//if( pMsgDealWith->m_NewSocket[i].m_nSecDMType != MsgPackage.nSecDMType)
					//	continue;
					//if( pMsgDealWith->m_NewSocket[i].m_nSecDMID != MsgPackage.nSecDModularID )
					//	continue;

					/*===========================================================================*/
					/*�����ݽ���socketȥ����*/

					while( !pMsgDealWith->m_bEndAllThread )
					{
							section.Lock();
							eSendResult = pMsgDealWith->m_NewSocket[i].Send( szBuf,sizeof(MsgPackage) );
							section.Unlock();

							if( Unfinishing == eSendResult )
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
								/*
								pMsgDealWith->m_SendQueue.push( szBuf, CFCQUEUE_BUFSIZE ) ;
								
								bSendResult = true;
								break;
								*/
							}
							else if( Finishing == eSendResult )
							{
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][To MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, pMsgDealWith->m_NewSocket[i].m_nSMType ,0 );
								
								bSendResult = true;//Send success
								section.Unlock();
								break;
							}
							else if( Closed == eSendResult )
							{	
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );
								section.Unlock();

								break;
							}
							else if( Error == eSendResult )
							{
								section.Lock();
								sprintf( szSocketStateMsgBuf,"[MType:%d][Socket Close From MID:%d][%d][IP:%s][Port:%d]", pMsgDealWith->m_NewSocket[i].m_nSMType , pMsgDealWith->m_NewSocket[i].m_nSMID,
										i,inet_ntoa(pMsgDealWith->m_NewSocket[i].m_addr.sin_addr),
										ntohs(pMsgDealWith->m_NewSocket[i].m_addr.sin_port) );

								pMsgDealWith->m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SOCKET_MSG ,0 );
								section.Unlock();
								break;
							}
							else
							{

								break;
							}

											
					/*********************************************************************************************/
					}//end while
			/*********************************************************************************/
		}//end for

		if( !bSendResult )//������û�з�����ϵͳ�Զ�����ɾ��
		{
			section.Lock();
			sprintf( szMsgNoDest,"[DType:%d][Send Data False: No dest ][To DID:%d]", MsgPackage.nDMType, MsgPackage.nDMID );
			pMsgDealWith->m_lpfuncExport( szMsgNoDest, CFCOMM_EVENT_SYS_MSG, 0 );
			section.Unlock();
		}

		Sleep(1);
	}

	//pMsgDealWith->m_lpfuncExport( "[Send thread end]",CFCOMM_EVENT_SYS_MSG,0 );
	section.Lock();
		pMsgDealWith->m_nThreadCount -= 1;
	section.Unlock();
	return 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgDealWith::CMsgDealWith()
{

	memset( m_nReciveState, MSGDEALWITH_REC_INIT_STATE, MSG_MAX_SOCKET_NUM );
	memset( m_nSendState, MSGDEALWITH_SEND_INIT_STATE, MSG_MAX_SOCKET_NUM );
	memset( m_bClientRegModule, false, MSG_MAX_SOCKET_NUM );

	m_nSMType = DMODULARTYPE_BROKER;
	m_nSMID = 0;
	memset(&m_Addr, 0, sizeof(m_Addr));

	m_lpfuncExport = NULL;

	m_bEndAllThread = false;
	m_nThreadCount = 0;

}



CMsgDealWith::~CMsgDealWith()
{
	ExitInstance();

}

bool CMsgDealWith::Initialize( unsigned int Port, LPFUNCEXPORTDATA *p,	int nSMType, int nSMID )
{

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	
	for( int i = 0; i < MSG_MAX_SOCKET_NUM; i++ )
	{
		m_NewSocket[i].Initialize();
		m_nReciveState[i] = MSGDEALWITH_REC_INIT_STATE;
	}
	
	m_nSMID = nSMID;
	m_nSMType = nSMType;

	memset((char *)&m_Addr,0,sizeof(m_Addr));
	m_Addr.sin_family = AF_INET;
    m_Addr.sin_port =(unsigned short)Port ;
    m_Addr.sin_addr.S_un.S_addr = INADDR_ANY;

	m_lpfuncExport = p;
	m_bEndAllThread = false;
	m_nThreadCount = 0;
	//Creat Listen thread
	AfxBeginThread( ServerRunModeListenProgress, this, NULL );

	//Creat Recive data thread
	AfxBeginThread( ServerRunModeReciveProgress, this, NULL );

	//Creat Send data thread
	AfxBeginThread( ServerRunModeSendProgress, this, NULL );

	return true;
}


void CMsgDealWith::ExitInstance()
{
	for( int i = 0; i < MSG_MAX_SOCKET_NUM ; i++ )
	{
		m_NewSocket[i].Close();
	}
	closesocket(m_ListenSocket);
	m_cfcSocketClicent.Close();
	m_bEndAllThread = true;
	while( 1 )
	{
		if( m_nThreadCount <= 0 )
		{
			break;
		}
		Sleep(100);
	}
	

	memset(&m_Addr, 0, sizeof(m_Addr));
	memset( m_bClientRegModule, false, MSG_MAX_SOCKET_NUM );
	memset( m_nReciveState, MSGDEALWITH_REC_INIT_STATE, MSG_MAX_SOCKET_NUM );
	memset( m_nSendState, MSGDEALWITH_SEND_INIT_STATE, MSG_MAX_SOCKET_NUM );
	

	m_nSMType = DMODULARTYPE_BROKER;
	m_nSMID = 0;
    
	return;
}



bool CMsgDealWith::ReadData(char *buf, int nLen)
{
	bool bHaveData = false;

	section.Lock();
	
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


	section.Unlock();

	return bHaveData;
}

bool CMsgDealWith::SendData(char *buf, int nLen)
{
	bool bResult = false;

	section.Lock();

	if( m_SendQueue.push( buf,nLen) )
	{
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	section.Unlock();


	return bResult;

}

bool CMsgDealWith::RegClientModuleInfo(int nSocketIndex, char *buf, int nLen)
{
	
	if( buf == NULL )
	{
		m_lpfuncExport( "[Reg Buf is NULL]",CFCOMM_EVENT_SYS_MSG,0 );
		return false;
	}
	if( nSocketIndex < 0 )
	{
		m_lpfuncExport( "[Reg SocketIndex < 0]",CFCOMM_EVENT_SYS_MSG,0 );
		return false;
	}
	if( nSocketIndex > MSG_MAX_SOCKET_NUM )
	{
		m_lpfuncExport( "[nSocketIndex > MSG_MAX_SOCKET_NUM]",CFCOMM_EVENT_SYS_MSG,0 );
		return false;
	}

	bool bResult = true;
	CFC_MESSAGE_PACK MsgPackage;
	memset( &MsgPackage, 0, sizeof(MsgPackage) );
	
	memcpy( &MsgPackage, buf, sizeof(MsgPackage));
	if( MSGDEALWITH_MOUAL_REG_MSG == MsgPackage.nMsgType )//�ж��ǲ���ע����Ϣ
	{
			if( 0 == MsgPackage.nSMType )
			{
				m_lpfuncExport( "[0 == MsgPackage.nSMType ]",CFCOMM_EVENT_SYS_MSG,0 );
				return false;
			}
			for( int i = 0; i < MSG_MAX_SOCKET_NUM; i++ )
			{
				if( MsgPackage.nSMType == m_NewSocket[i].m_nSMType )
					if( m_NewSocket[i].m_nSMID == MsgPackage.nSMID )
					{
						m_lpfuncExport( "[have registered]",CFCOMM_EVENT_SYS_MSG,0 );
						return false;//����ע���ˣ���ʾע��ʧ��
					}
			}

			//m_NewSocket[nSocketIndex].m_nDMType = MsgPackage.nDMType;
			//m_NewSocket[nSocketIndex].m_nDMID = MsgPackage.nDMID;
			m_NewSocket[nSocketIndex].m_nSMType = MsgPackage.nSMType;
			m_NewSocket[nSocketIndex].m_nSMID = MsgPackage.nSMID;
			//m_NewSocket[nSocketIndex].m_nSecDMType  = MsgPackage.nSecDMType;
			//m_NewSocket[nSocketIndex].m_nSecDMID  = MsgPackage.nSecDModularID;
			m_bClientRegModule[nSocketIndex] = true;
			bResult = true;
	}
	else
	{
			m_lpfuncExport( "[is not reg info]",CFCOMM_EVENT_SYS_MSG,0 );
			m_bClientRegModule[nSocketIndex] = false;
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

	memset( &MsgPackage, 0, sizeof(MsgPackage) );
	memcpy( &MsgPackage, buf, sizeof(MsgPackage));
	if( MSGDEALWITH_MOUAL_REG_MSG == MsgPackage.nMsgType )//�ж��ǲ���ע����Ϣ
	{

		//m_cfcSocketClicent.m_nDMType = MsgPackage.nDMType;
		//m_cfcSocketClicent.m_nDMID = MsgPackage.nDMID;
		m_cfcSocketClicent.m_nSMType = MsgPackage.nSMType;
		m_cfcSocketClicent.m_nSMID = MsgPackage.nSMID;
		//m_cfcSocketClicent.m_nSecDMType  = MsgPackage.nSecDMType;
		//m_cfcSocketClicent.m_nSecDMID  = MsgPackage.nSecDModularID;
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	
	return bResult;
}

void CMsgDealWith::ClearClientRegInfo(int nSocketIndex)
{
	m_NewSocket[nSocketIndex].m_nDMType =0;
	m_NewSocket[nSocketIndex].m_nDMID = 0;
	m_NewSocket[nSocketIndex].m_nSMType = 0;
	m_NewSocket[nSocketIndex].m_nSMID = 0;
	m_NewSocket[nSocketIndex].m_nSecDMType  =0;
	m_NewSocket[nSocketIndex].m_nSecDMID  = 0;
	m_bClientRegModule[nSocketIndex] = false;
}



void CMsgDealWith::NetAbendDealWith(int i)
{
	if( m_NewSocket[i].m_bNetAbend )
	{
		if( 0 == m_NewSocket[i].m_nSMType )
		{
			m_NewSocket[i].m_bNetAbend = false;
			ClearClientRegInfo(i);
			return ;

		}

		CFC_MESSAGE_PACK msgpackage;
		CFC_NET_ABEND_TYPE netAbend;
		char szSocketStateMsgBuf[ MSGDEALWITH_SOCKET_STATE_BUFLEN ];

		memset( &msgpackage,0, sizeof(msgpackage) );
		memset( &netAbend,0, sizeof(netAbend) );
		memset( szSocketStateMsgBuf,0, MSGDEALWITH_SOCKET_STATE_BUFLEN );

		msgpackage.cIdentifier = '\xFF';
		msgpackage.nMsgLen = sizeof(netAbend) + sizeof(msgpackage) ;
		msgpackage.nMsgType = 0x2001;


		msgpackage.nDMType	=	DMODULARTYPE_CTI;
		msgpackage.nDMID		=	0;
		msgpackage.nSMType	=	m_NewSocket[i].m_nSMType;
		msgpackage.nSMID		=	m_NewSocket[i].m_nSMID;
		msgpackage.nSecDMType =	0;
		msgpackage.nSecDModularID	=	0;


		netAbend.nModularID = m_NewSocket[i].m_nSMID;
		netAbend.nModularType = m_NewSocket[i].m_nSMType;
		
		
		memcpy( msgpackage.szMessageBody, (char*)&netAbend, sizeof(CFC_NET_ABEND_TYPE) );
		
		if( m_RecQueue.push( (char*)&msgpackage, sizeof( CFC_MESSAGE_PACK ) ) )
		{
			sprintf( szSocketStateMsgBuf, "[ Net Abend ][SMType:%d][SMID:%d]", m_NewSocket[i].m_nSMType, m_NewSocket[i].m_nSMID );
			//m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_ACD_MSG ,0 );
			m_lpfuncExport( szSocketStateMsgBuf, CFCOMM_EVENT_SYS_MSG ,0 );

			ClearClientRegInfo(i);
			m_NewSocket[i].m_bNetAbend = false;
		}
		else
		{
			m_lpfuncExport( "[Recive Queue full]",CFCOMM_EVENT_SYS_MSG,0 );
		}

	}

	return;
}

void CMsgDealWith::InitVar()
{

}
