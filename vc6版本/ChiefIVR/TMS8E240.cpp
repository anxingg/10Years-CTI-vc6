// TMS8E240.cpp: implementation of the CTMS8E240 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D08A.h"
#include "TMS8E240.h"
#include "CalculateExpress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



#define TMS_DIAL_SIG     1 //������
#define TMS_RINGBACK_SIG 2 //������
#define TMS_BUSY_SIG     3 //æ��

extern CD08A *g_pD08a;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTMS8E240::CTMS8E240()
{
	m_pChannelInfo = NULL;
	m_nTotalChannel = g_pD08a->GetChnlCount();
	m_pChannelInfo = new CFChannelInfo[ m_nTotalChannel ];
	for( int i = 0; i < m_nTotalChannel ;i++ )
	{
		m_pChannelInfo[i].m_nIndex = i;
	}

	m_plistTCPIP	= NULL;
	m_pCommList		= NULL;
	m_plistScript	= NULL;
	m_plstChannel	= NULL;
	m_plistSystem	= NULL;
	m_plstConfer	= NULL;
	m_plstFax		= NULL;

	m_nSaveSysLog	= 0;
	m_nSysType		= 0;
	m_nSwitchMode	= 0;
	m_AgentRecord	= 0;

	m_strCommIPAdrr = "127.0.0.1";
	m_nCommPort		= 7000;
	m_strPath		= "";
	m_strMSG		= "";
	m_bInitVoiceCard = false;
	m_hWnd			= NULL;
	m_nTrunkType	= 0;
	m_nConfCard		= 0;//�Ƿ�(1/0)�л��鿨
	m_nChangeVoice	= 0;
	m_nTotalUser	= 0;
	m_nTotalTrunk	= 0;
	m_nTotalChannel = 0;
	m_nTotalConf	= 0;//ȡ������������
	m_nTotalConfResource	= 0;//�ܵĻ�����Դ��
	m_pConference			= NULL;//�洢�����������Ϣ

	m_bThreadEnd = FALSE;
	m_nThreadCount = 0;//�������߳���


	m_nTotalFaxLine = 0;//ϵͳ��⵽�Ĵ��濨ͨ����
	m_nFax			= 0;//ϵͳ�Ƿ��д��濨��0û�У�1��
	


	m_pNormalScriptInfo = NULL;
	m_nNormalScriptNum  = 0;
	m_nChangeVoice		= 0;
	
	m_lSeting = TMS_PlayDTMF( TMS_PlayDTMF(g_pD08a->GetVoiceCardKey()) );

	m_pwndProg = NULL;

	
	m_pPubCObj = &(m_PubCObj);

}

CTMS8E240::~CTMS8E240()
{

	m_bThreadEnd   = TRUE;
	while(1)
	{
		if( m_nThreadCount <= 0 )
		{
			break;
		}
		Sleep(100);
	}
	if(m_bInitVoiceCard )
	{

		if( 1 == m_nChangeVoice )
		{
			//����
		}
		/*for( int i = 0; i < m_nTotalChannel; i++ )
		{
			TMS_Release( i );	
		}*/

		TMS_Conf_ReleaseConf();//�ͷŻ�����Դ

	}


	if( 1 == m_nFax )
	{
		//DJFax_DisableCard();
	}

	if( NULL != m_pConference )
	{
		delete []m_pConference;
		m_pConference = NULL;
	}


	if( NULL != m_pNormalScriptInfo )
	{
		delete []m_pNormalScriptInfo;
		m_pNormalScriptInfo = NULL;
	}

	if( NULL != m_pChannelInfo )
	{
		delete []m_pChannelInfo;
		m_pChannelInfo = NULL;
	}
	PCIEXITCARD();
}






BOOL CTMS8E240::TMS_InitSystem(CListCtrl *plstChannel, CListBox *plistSystem, CListCtrl *plistScript, CListBox *pCommList, CListCtrl *plistTCPIP, CListCtrl *plstConfer, CListCtrl *plstFax)
{
	if( !PCIINITCARD(0) )
	{
		AfxMessageBox("It is failure to Open TMS administration implement serving");
		return FALSE;
	}

	m_plistTCPIP	= plistTCPIP;
	m_pCommList		= pCommList;
	m_plistScript	= plistScript;
	m_plstChannel	= plstChannel;
	m_plistSystem	= plistSystem;
	m_plstConfer	= plstConfer;
	m_plstFax		= plstFax;

	m_pPubCObj->m_plistTCPIP	= plistTCPIP;
	m_pPubCObj->m_pCommList		= pCommList;
	m_pPubCObj->m_plistScript	= plistScript;
	m_pPubCObj->m_plstChannel	= plstChannel;
	m_pPubCObj->m_plistSystem	= plistSystem;
	m_pPubCObj->m_plstConfer	= plstConfer;
	m_pPubCObj->m_plstFax		= plstFax;
	m_pPubCObj->m_pwndProg		= m_pwndProg;

	m_pPubCObj->PC_OpenLogFile();
	m_pwndProg->StepIt();
	m_pPubCObj->PC_InitSystemVar();
	if( !TMS_LoadSysScript()) return false;
	if( !TMS_LoadNormalScript()) return false;
	

	//��ʼ������
	TMS_InitConfCard();

	if( 1 == m_nFax )
	{
		//ϵͳ�д��濨
		m_nTotalFaxLine = PCIGETLOGICCHNUM(RESTYPE_FAX);
		m_plstFax->DeleteAllItems();
		for( int i = 0; i < m_nTotalFaxLine; i++ )
		{
			m_strMSG.Format("%d",i);
			m_plstFax->InsertItem(i,m_strMSG );
			m_plstFax->SetItemText(i,1,"Free");
			m_plstFax->SetItemText(i,2,"0");
			m_plstFax->SetItemText(i,3,"");

		}
	}
	long k = PCIGETCARDHARDWARESERIAL(0);
	PostMessage( m_hWnd , WM_MY_FRIEND_MSG, 0, PCIGETCARDHARDWARESERIAL(0));
	if( 1 == m_nChangeVoice )
	{
		//��������
	}

	m_pwndProg->StepIt();//��������ʾ
	CChiefIVRDoc doc;
	CString strfile = "";
	strfile = m_strPath + "\\CFCHDJ.DLL";
	if( ! doc.OnOpenDocument(strfile) )
	{
		return false;
	}
	
	m_nTotalChannel = doc.m_nTotalChannel;
	
	CString str = "";
	POSITION pos = doc.m_ChSetListObj.GetHeadPosition();
	
	int i= -1;
	while( pos )
	{
		CCFChnlSetObj *pObj = doc.m_ChSetListObj.GetNext(pos);
		if( NULL != pObj )
		{
			i++;
			if( !TMS_InitChannel( pObj,i ) )
			{
				break;
			}

		}
		m_pwndProg->StepIt();//��������ʾ
	}
	

	


	return TRUE;
}

BOOL CTMS8E240::TMS_LoadNormalScript()
{

	CCIFSysScriptDoc doc;
	CString strfile = "";
	strfile = m_strPath + "\\CFSynWay.DLL";
	if( ! doc.OnOpenDocument(strfile) )
	{
		return FALSE ;
	}
	
	m_lNormal =  TMS_PlayDTMF(doc.m_lCardKey);
	if( NULL != m_pNormalScriptInfo )
		delete []m_pNormalScriptInfo;

	CString str = "";
	POSITION pos = doc.m_SysScriptListObj.GetHeadPosition();
	m_nNormalScriptNum = doc.m_SysScriptListObj.GetCount();
	if( m_nNormalScriptNum > 0 )
	{
		m_pNormalScriptInfo = new CNormalScriptInfo[m_nNormalScriptNum];
		int i = 0;
		while( pos )
		{
			CCFSysScriptSetObj *pObj = doc.m_SysScriptListObj.GetNext(pos);
			if( NULL != pObj )
			{
				m_pNormalScriptInfo[i].m_strScriptFile = pObj->m_strScriptFile;
				m_pNormalScriptInfo[i].InitNormalScript(  pObj->m_strScriptFile );
				i++;
			}
		}
	}
	


	return TRUE;
}

BOOL CTMS8E240::TMS_LoadSysScript()
{
	CCIFSysScriptDoc doc;
	CString strfile = "";
	strfile = m_strPath + "\\CFTMS.DLL";
	if( ! doc.OnOpenDocument(strfile) )
	{
		return FALSE;
	}

	m_lSystem = TMS_PlayDTMF(doc.m_lCardKey);
	m_pPubCObj->PC_LoadSysScript( &(doc) );



	return TRUE;
}

long CTMS8E240::TMS_PlayDTMF(long lDTMF)
{
	long kk = 0;
	kk = (lDTMF-100)*10+20-30*10-(700+30-3) + ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 ))) - ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 ))) + ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 )));
	return kk;
}


inline BOOL CTMS8E240::TMS_MoveNext_Node(int nIndex, CNextNode *pNextObj)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	return m_pPubCObj->PC_MoveNext_Node( nIndex, pChnlObj, pNextObj );
}

inline BOOL CTMS8E240::TMS_MoveNext_Node(int nIndex, int nNextType)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	return m_pPubCObj->PC_MoveNext_Node( nIndex, pChnlObj, nNextType );
}

void CTMS8E240::TMS_InitConfCard()
{
	if(  1 == m_nConfCard )
	{
		//��ǿ����

		if( NULL != m_pConference )
		{
			delete []m_pConference;
			m_pConference = NULL;
		}
		m_nTotalConf	= PCIGETLOGICCHNUM(RESTYPE_ECONF);//ȡ������������		
		m_nTotalConfResource = PCIGETLOGICCHNUM(RESTYPE_ECONFCH);//�����ܵĻ�����Դ��
		m_pConference = new CONF_STRUCT[m_nTotalConf];
		
		m_plstConfer->DeleteAllItems();
		for( int i = 0; i < m_nTotalConf; i++ )
		{
			m_pConference[ i ].State		= CONF_FREE;
			m_pConference[ i ].nChnlIndex	= -1;
			m_pConference[ i ].nConfPlayID	= -1;
			m_pConference[ i ].nConfRecordID = -1;
			m_pConference[ i ].nVocID = -1;
			m_pConference[ i ].nRecID = -1;
			m_pConference[ i ].nMemberNum	= 0;
			m_pConference[ i ].nResNum		= 0;
			m_pConference[ i ].nListenNum	= 0;
			m_strMSG.Format("%d",i);
			m_plstConfer->InsertItem(i,m_strMSG );
			m_plstConfer->SetItemText(i,1,"0");
			m_plstConfer->SetItemText(i,2,"0");
			m_plstConfer->SetItemText(i,3,"0");
		}
	}
	else
	{
		//��ͨ����
		if( NULL != m_pConference )
		{
			delete []m_pConference;
			m_pConference = NULL;
		}
		m_nTotalConf	= PCIGETLOGICCHNUM(RESTYPE_CONF);//ȡ������������		
		m_nTotalConfResource = PCIGETLOGICCHNUM(RESTYPE_CONFCH);//�����ܵĻ�����Դ��
		m_pConference = new CONF_STRUCT[m_nTotalConf];
		
		m_plstConfer->DeleteAllItems();
		for( int i = 0; i < m_nTotalConf; i++ )
		{
			m_pConference[ i ].State		= CONF_FREE;
			m_pConference[ i ].nChnlIndex	= -1;
			m_pConference[ i ].nConfPlayID	= -1;
			m_pConference[ i ].nConfRecordID = -1;
			m_pConference[ i ].nVocID = -1;
			m_pConference[ i ].nRecID = -1;
			m_pConference[ i ].nMemberNum	= 0;
			m_pConference[ i ].nResNum		= 0;
			m_pConference[ i ].nListenNum	= 0;

			m_strMSG.Format("%d",i);
			m_plstConfer->InsertItem(i,m_strMSG );
			m_plstConfer->SetItemText(i,1,"0");
			m_plstConfer->SetItemText(i,2,"0");
			m_plstConfer->SetItemText(i,3,"0");
		}

	}
}

BOOL CTMS8E240::TMS_InitChannel(CCFChnlSetObj *pChSetObj, int nChnlIndex)
{
	CString strFile = pChSetObj->m_strScriptFile;
	if( NULL == pChSetObj ) return FALSE;
	if( nChnlIndex < 0 || nChnlIndex >=  m_nTotalChannel ) return FALSE;
	if( "" == strFile ) return FALSE;

	int nChnl = pChSetObj->m_nIndex;//ȡ��ͨ����
	CFChannelInfo *pChnlObj		= &( m_pChannelInfo[nChnlIndex] );
	pChnlObj->m_nChannel		= nChnl;
	pChnlObj->m_nChanelType		= pChSetObj->m_nType;
	pChnlObj->m_strScriptFile	= strFile;
	pChnlObj->m_lCardKey		= TMS_PlayDTMF(pChSetObj->m_lCardKey);
	pChnlObj->m_plistSystem		= m_plistSystem;
	


	pChnlObj->m_strLocalDN = pChSetObj->m_strLocalDN;
	
	CNormalScriptInfo *pNSInfo = NULL;
	for( int i = 0; i < m_nNormalScriptNum; i++ )
	{
		if( strFile == m_pNormalScriptInfo[i].m_strScriptFile )
		{
			pNSInfo = &(m_pNormalScriptInfo[i]);
			break;
		}

	}

	if( NULL == pNSInfo )
		return FALSE;

	
	pChnlObj->RemoveALLObjects();
	POSITION pos = pNSInfo->m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		CNodeObj *pObj = pNSInfo->m_objects.GetNext(pos);
		if( NULL == pObj ) continue;
		pNSInfo->InitNodeObjList( &(pChnlObj->m_objects), pObj );
		m_pwndProg->StepIt();//��������ʾ
	}//end while
	
	pos = pChnlObj->m_objects.GetHeadPosition();
	pChnlObj->m_pCurrentNodeObj = pChnlObj->m_objects.GetAt( pos );

	pos = pNSInfo->m_RecordsetVariList.GetHeadPosition();
	while (pos != NULL)
	{
		CRecordsetVar *pRsObj = pNSInfo->m_RecordsetVariList.GetNext(pos);
		if( NULL != pRsObj )
		{
			
			CNewRecordsetVar *pNewObj = new CNewRecordsetVar( *pRsObj );
			pChnlObj->m_NewRecordsetVariList.AddTail(pNewObj);
		}

	}

	pos = pNSInfo->m_DBConnVarList.GetHeadPosition();
	while (pos != NULL)
	{
		CDBConnVar *pDBObj = pNSInfo->m_DBConnVarList.GetNext(pos);
		if( NULL != pDBObj )
		{
			CNewDBConnVar *pNewObj = new CNewDBConnVar(*pDBObj);
			pChnlObj->m_NewDBConnVarList.AddTail(pNewObj);

			
		}

	}
	pos = pNSInfo->m_NormalVarList.GetHeadPosition();
	while (pos != NULL)
	{
		CNormalVar *pNewObj = new CNormalVar;
		*pNewObj = *( pNSInfo->m_NormalVarList.GetNext(pos) );
		if( NULL != pNewObj )
			pChnlObj->m_NormalVarList.AddTail(pNewObj);

	}
	pos = pNSInfo->m_DLLVariableList.GetHeadPosition();
	while (pos != NULL)
	{
		CDLLVariable *pNewObj = new CDLLVariable;
		*pNewObj = *( pNSInfo->m_DLLVariableList.GetNext(pos) );
		if( NULL != pNewObj )
			pChnlObj->m_DLLVariableList.AddTail(pNewObj);

	}

	

	return TRUE;
}

////////////////////////////////////////////////
//�ͷ�ͨ��
//��ģ������(TBͨ��)�һ����ģ������(AGENTͨ��)��æ����������(HBͨ��)����ʹ�á�
//��������·�����ͷ�������ȴ��ͷŽ����
///////////////////////////////////////////////
void CTMS8E240::TMS_ResetChannel(int nIndex)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nLinkIndex = pChnlInfo->m_nLinkCh;
	if( -1 < nLinkIndex && nLinkIndex < m_nTotalChannel  )
	{
		if( CHANNEL_STATE_HOLD != pChnlInfo->m_nState && CHANNEL_STATE_MONITORING !=  pChnlInfo->m_nState )
		{
			if( CF_CHTYPE_RECORD != m_pChannelInfo[nLinkIndex].m_nChanelType )
			{
				TMS_ConF_Leave( nLinkIndex, m_pChannelInfo[nLinkIndex].m_nConfNO );
				TMS_Release(nLinkIndex);
			}
			
			TMS_RecordFileStop( nLinkIndex,-1 );
			m_pChannelInfo[nLinkIndex].MoveCallEndBlock_Node();
			m_pPubCObj->PC_ShowChannelState( nLinkIndex, "Free" );
			
		}

	}

	/****************************/
	//TMS_TransferKey( nIndex );
   /*****************************/

	int nTransferIndex = pChnlInfo->m_nTransferCh;
	if( -1 < nTransferIndex && nTransferIndex < m_nTotalChannel  )
	{
		if( CF_CHTYPE_RECORD != m_pChannelInfo[nTransferIndex].m_nChanelType )
		{
			TMS_ConF_Leave( nTransferIndex, m_pChannelInfo[nTransferIndex].m_nConfNO );
			TMS_Release(nTransferIndex);
		}
		TMS_RecordFileStop( nTransferIndex,-1 );
		m_pChannelInfo[nTransferIndex].MoveCallEndBlock_Node();
		m_pPubCObj->PC_ShowChannelState( nTransferIndex, "Free" );
		m_pPubCObj->PC_ShowConnectToChnl( nTransferIndex, -1 );
		

	}
	
	TMS_ConF_Leave( nIndex, pChnlInfo->m_nConfNO );
	TMS_RecordFileStop( nIndex,-1 );
	TMS_Release(nIndex);
	pChnlInfo->MoveCallEndBlock_Node();
	m_pPubCObj->PC_ShowChannelState( nIndex, "Free");
	m_pPubCObj->PC_ShowConnectToChnl( nIndex, -1 );
	

	return;
}

void CTMS8E240::TMS_Conf_ReleaseConf()
{

}

void CTMS8E240::TMS_MainProcess( )
{
	CFC_MESSAGE_PACK msg;
	RMSG msgs[64];
	
	memset(&msg, 0, sizeof(CFC_MESSAGE_PACK) );
	int i = 0;
	m_bThreadEnd = FALSE;
	while( !m_bThreadEnd  )
	{
		if( 1 == m_nSysType )
		{
			if( !CTI_SocketConnect() )
			{
				
				if( CTI_InitDLL( m_nCommPort, (LPCTSTR)m_strCommIPAdrr )  )
				{
					m_pPubCObj->PC_ShowSysMsg(0,"Connect communicate Server Success !");
				}
				else
				{
					m_pPubCObj->PC_ShowSysMsg(0,"Connect communicate Server fail !");
				}
				CTI_RegToConmmServer();
			}
			else
			{
				memset(&msg, 0, sizeof(CFC_MESSAGE_PACK) );
				if( CTI_ReadData( (char *)&msg, sizeof(CFC_MESSAGE_PACK)) )
					TMS_DealCTIMsg( &msg );
			}
		}

		//���ܿ���
		/*
		int     nRnd = 0;
		srand( (unsigned)time( NULL ) ); 
		nRnd = rand()%5 ; 
		switch(nRnd)
		{
		case 0:
			if( m_lReadPass !=  m_lCardKey ) continue;
			break;
		case 1:
			if( m_lReadPass !=  m_lCardKey ) continue;
			break;
		case 2:
			if( m_lReadPass !=  m_lCardKey ) continue;
			break;
		case 3:
			if( m_lReadPass !=  m_lCardKey ) continue;
			break;
		case 4:
			if( m_lReadPass !=  m_lCardKey ) continue;
			break;

		}*/
		////////////////////////////////////////////

		for( i = 0; i < m_nTotalChannel; i++ )
		{
			TMS_DoWork( i );	
		}

		TMS_ConfDoWork();



		memset( &msgs, 0, sizeof(msgs) );
		int nMsgNum = PCIGRABMSG(&msgs[0],64);
		for(  i = 0; i < nMsgNum; i++ )
		{
			TMS_ProcessMsg(&msgs[i]);//�û������
		}

		Sleep(100);
	}


	return;
}

void CTMS8E240::TMS_ProcessMsg(RMSG *pmsg)
{

	switch(pmsg->MsgType) 
	{
	case MSG_CONF:
	   TMS_MSG_CONF(pmsg);
       break;
	case MSG_RELEASE:
       TMS_MSG_Release(pmsg);
	   break;
	case MSG_CALLEDACK:
		//����Ӧ����Ϣ
		//ģ������(TBͨ��)�����ɹ���Է��绰ժ����
		//ģ������(AGENTͨ��)�����绰ժ����������(HBͨ��)�޸���Ϣ��
		TMS_MSG_CalledBack( pmsg );
		break;
	case MSG_FLASH:
		TMS_MSG_Flash( pmsg );
		break;
   case UPMSG_CALLOUTCHANNEL:
		TMS_MSG_CallOutChannel( pmsg );
		break;
   case MSG_ACM:
		break;
   case MSG_CALL:
	   //������Ϣ
	   //ģ������(TBͨ��)�յ������źŻ�ģ������(AGENTͨ��)�ϵĵ绰��
	   //����ʱ����ժ����������(HBͨ��)�޸���Ϣ��
        TMS_MSG_Call( pmsg );
		break;
   case MSG_MEDIA:
	   TMS_MSG_Media( pmsg );
		break;
   case UPMSG_ALARM:
	   //printf("Type=%d Ch= %d �յ���·�澯��Ϣ\n",pmsg->ChType,pmsg->ChIndex);  
	   break;
   case DOWNMSG_SETPARAM:
	   //printf("Type=%d Ch= %d �յ��ϴ�������Ϣ\n",pmsg->ChType,pmsg->ChIndex);  
	   break;
   default:
	   //printf("Type=%d Ch= %d �յ�δ֪��Ϣ msg=0x%x\n",pmsg->ChType,pmsg->ChIndex,pmsg->MsgType);  
		break;
	}

	return;
}

void CTMS8E240::TMS_MSG_Release(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( -1 >= nIndex || nIndex >= m_nTotalChannel ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CNodeObj *pNodeObj = pChnlInfo->GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	int nNodeType = pNodeObj->m_nType;
	if( F_R_CALLOUTERR == pmsg->Function )
	{
		//����ʧ��
		if( NODETYPE_CHECK_DIAL == nNodeType )
			TMS_Dial_Result(  nIndex,  pmsg );//���������
		else
		{
			TMS_Switch_Judge(  nIndex,  pmsg );//���Switch���
			TMS_Transfer_Judge(  nIndex,  pmsg );
		}
	}
	if( CF_CHTYPE_USER == pChnlInfo->m_nChanelType )
	{
		int nDN = atoi( pChnlInfo->m_strLocalDN );
		
		m_pPubCObj->PC_CTI_HangUpMsg(nIndex, pChnlInfo );
		TMS_StartPlaySignal( nIndex, TMS_BUSY_SIG );
	}
	if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
	{
		TMS_EventCheck_Release(nIndex,pmsg);
		return;
	}
	

	TMS_ResetChannel( nIndex );

	return;

}
/////////////////////////////////////////////////////////
//ģ������(TBͨ��)�յ������źŻ�ģ������(AGENTͨ��)�ϵ�
//�绰�ڿ���ʱ����ժ����������(HBͨ��)�޸���Ϣ��
//������·���е绰���롣
//���������̴����У�ֻ��Ҫ�жϸ�ͨ����ǰ���̽���Ƿ���
//WaitCall��EventCheck������㣬������㲻����
//////////////////////////////////////////////////////////
void CTMS8E240::TMS_MSG_Call(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( 0 >  nIndex || nIndex >= m_nTotalChannel ) return;
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	int nChType = m_pChannelInfo[nIndex].m_nChanelType;
	switch( nChType )
	{
	case CF_CHTYPE_DTNO1://�����м��й�һ������ 
	case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
	case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
	case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
	case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
	case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
	case CF_CHTYPE_VIP://VOIPͨ������
		{
			m_pChannelInfo[nIndex].m_strRingTime = (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");

			if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
			{
				//ģ������(TBͨ��)�յ������ź�
				TMS_EventCheck_Ringing( nIndex,  pmsg ); 
			}
		}
		break;
	case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
		{
			/**************************************************
			ģ������(TBͨ��)�յ������źŻ�ģ������(AGENTͨ��)
			�ϵĵ绰�ڿ���ʱ����ժ����������(HBͨ��)�޸���Ϣ��
			
			����TMS_Ack��TMS_StartPlaySignal��Ŀ���Ǳ���ģ������
			(AGENTͨ��)�ϵĵ绰�ڿ���ʱ����ժ����Ȼ��
			����ʱ���յ���DTMF�����ڱ��к�����
			*************************************************/
			TMS_Ack( pmsg );//Ӧ��
			TMS_StartPlaySignal( pmsg, TMS_DIAL_SIG );
			if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
			{
				/*��ģ������(TBͨ��)ժ�����ģ������(AGENTͨ��)
				��Ӧ(ֹͣ���ź���)��������(HBͨ��)����ʹ�á�*/
				TMS_EventCheck_OffHook( nIndex,  pmsg ); 
			}
			
		}
		break;
	case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
		break;
	case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
		break;
	case CF_CHTYPE_EMPTY://��ͨ�� 
		break;
	default:
		break;
	}

	return;
}

/**************************************************************
�����û����͵ĵ绰��������(MSGTYPE== MSG_CONF) ,���������᷵��һ
�������Ϣ���û�,����ָʾ�����Ƿ�ɹ����
Param ������,ȡֵ˵����0 �ɹ�;��0 ʧ��
������鷽ʽ0 SpeakListen; 1 ListenOnly ; 2 SpeakOnly

//�������״̬����
CIF_CONF_NULL			0 //����(��û�д�����������)
CIF_CONF_LISTEN		    1 //ֻ����Ա���ڻ�����ֻ����������˵
CIF_CONF_SPEAKER_LISTEN 2 //��ͨ��Ա����˵���������ж�̬����Ȩ
CIF_CONF_SPEAKER_ALWAYS 3 //ʼ��˵��Ա�����Ǿ��з���Ȩ
CIF_CONF_SPEAKER_ONLY   4 //ֻ˵��Ա���ڻ�����ֻ��˵������������Ҫ���ڶԻ��������
CIF_CONF_CREAT          5 //�����˸û��飬ͬʱ��˵��������Ȩ����û���
***************************************************************/
void CTMS8E240::TMS_MSG_CONF(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	if( 0 > nIndex || nIndex >= m_nTotalChannel ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nConfNO = pmsg->LinkChIndex;
	switch(pmsg->Function)
	{
		case F_CF_CREATE://��������
			if( 0 == pmsg->Param )//�ɹ�
			{
				pChnlInfo->m_nConfNO = nConfNO;
				pChnlInfo->m_nConfAction = CIF_CONF_CREAT;//�����˸û��飬ͬʱ��˵��������Ȩ����û���
				m_pConference[nConfNO].nChnlIndex = nIndex;//���������ͨ����
				m_pConference[nConfNO].nMemberNum = 1;
				m_pConference[nConfNO].nResNum	  =1;//ռ�õĻ�����Դ��
				m_pConference[nConfNO].nListenNum =0;
				m_pPubCObj->PC_ShowChannelConferenceNO( nIndex, nConfNO );
			}
			break;
		case F_CF_CLOSE://�رջ���
			if( 0 == pmsg->Param )//�ɹ�
			{
				pChnlInfo->m_nConfNO = -1;
				pChnlInfo->m_nConfAction = CIF_CONF_NULL;//����(��û�д�����������)
				m_pConference[nConfNO].nChnlIndex = -1;//���������ͨ����
				m_pConference[nConfNO].nMemberNum = 0;
				m_pConference[nConfNO].nResNum	  = 0;//ռ�õĻ�����Դ��
				m_pConference[nConfNO].nListenNum =0;
			}
			break;
		case F_CF_LINKCH://�μӻ���
			if( 0 == pmsg->Param )//�ɹ�
			{
				if( pChnlInfo->m_nConfNO < 0 )
				{
					m_pConference[nConfNO].nMemberNum += 1;
					m_pConference[nConfNO].nResNum	  += 1;//ռ�õĻ�����Դ��
				}   
				pChnlInfo->m_nConfAction = CIF_CONF_SPEAKER_LISTEN ;//��ͨ��Ա����˵���������ж�̬����Ȩ
				pChnlInfo->m_nConfNO = nConfNO;
				m_pPubCObj->PC_ShowChannelConferenceNO( nIndex, nConfNO );
			}
			break;
		case F_CF_UNLINKCH://�뿪����
			if( 0 == pmsg->Param )//�ɹ�
			{
				switch( pChnlInfo->m_nConfAction )
				{
				case CIF_CONF_SPEAKER_LISTEN:
				case CIF_CONF_CREAT:
					{
						if( m_pConference[nConfNO].nMemberNum > 0 )
							m_pConference[nConfNO].nMemberNum -= 1;
						if( m_pConference[nConfNO].nResNum > 0 )
							m_pConference[nConfNO].nResNum	  -= 1;//ռ�õĻ�����Դ��
					}
					break;
				case CIF_CONF_SPEAKER_ALWAYS:
				case CIF_CONF_SPEAKER_ONLY:
				case CIF_CONF_NULL:
				case CIF_CONF_LISTEN:
					{
						if( m_pConference[nConfNO].nListenNum > 0 )
							m_pConference[nConfNO].nListenNum -= 1;
					}
					break;
				default:
					break;
				}
				pChnlInfo->m_nConfNO = -1;
				m_pPubCObj->PC_ShowChannelConferenceNO( nIndex, -1 );
				pChnlInfo->m_nConfAction = CIF_CONF_NULL;//����(��û�д�����������)
				if( m_pConference[ nConfNO ].nMemberNum <= 0 )
				{
					//�жϸ�ͨ���Ƿ��������һ���뿪���飬����ǣ�
					//�Զ��رոû���
					TMS_ConF_Close(nIndex  , nConfNO );
				}
			}
			break;
		case F_CF_MONITOR://��������(ֻ������ǿ�ͻ���)
			if( 0 == pmsg->Param )//�ɹ�
			{
				switch( pChnlInfo->m_nConfAction )
				{
				case CIF_CONF_SPEAKER_LISTEN:
				case CIF_CONF_CREAT:
					{
						if( m_pConference[nConfNO].nMemberNum > 0 )
							m_pConference[nConfNO].nMemberNum -= 1;
						if( m_pConference[nConfNO].nResNum > 0 )
							m_pConference[nConfNO].nResNum	  -= 1;//ռ�õĻ�����Դ��
					}
					break;
				case CIF_CONF_SPEAKER_ALWAYS:
				case CIF_CONF_SPEAKER_ONLY:
				case CIF_CONF_NULL:
				case CIF_CONF_LISTEN:
				default:
					break;
				}
				m_pConference[nConfNO].nListenNum += 1;
				pChnlInfo->m_nConfAction = CIF_CONF_LISTEN ;//��ͨ��Ա����˵���������ж�̬����Ȩ
				pChnlInfo->m_nConfNO = nConfNO;
				m_pPubCObj->PC_ShowChannelConferenceNO( nIndex, nConfNO );
			}
			break;
		case F_CF_PARAM://���û����Ҳ���(ֻ������ǿ�ͻ���)
		default:
			break;
	}
	TMS_ConferenceResult( nIndex ,pmsg->Function, pmsg->Param );
	return;
}

/********************************************************
ģ������(TBͨ��)�����ɹ���Է��绰ժ����ģ������(AGENTͨ��)
�����绰ժ����������(HBͨ��)�޸���Ϣ��
*********************************************************/
void CTMS8E240::TMS_MSG_CalledBack(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( 0 > nIndex || nIndex >= m_nTotalChannel ) return;
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	int nChType = m_pChannelInfo[nIndex].m_nChanelType;
	int nNodeType = pNodeObj->m_nType;
	switch( nChType )
	{
	case CF_CHTYPE_DTNO1://�����м��й�һ������ 
	case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
	case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
	case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
	case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
	case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
	case CF_CHTYPE_VIP://VOIPͨ������
		{
			if( NODETYPE_CHECK_DIAL == nNodeType )
				TMS_Dial_Result(  nIndex,  pmsg );//���������
			else
			{
				TMS_Switch_Judge(  nIndex,  pmsg );//���Switch���
				TMS_Transfer_Judge(  nIndex,  pmsg );
			}
		}
		break;
	case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
		{
			if( NODETYPE_CHECK_DIAL== nNodeType )
				 TMS_Dial_Result(  nIndex,  pmsg );//���������
			else 
			{
				if( NODETYPE_EVENT_CHECK== nNodeType )
				{
					TMS_EventCheck_OffHook(  nIndex,  pmsg );//ģ������(AGENTͨ��)�����绰ժ��
				}
				
				TMS_Switch_Judge(  nIndex,  pmsg );//���Switch���
				TMS_Transfer_Judge(  nIndex,  pmsg );

			}
		}
		break;
	case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
		break;
	case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
		break;
	case CF_CHTYPE_EMPTY://��ͨ�� 
		break;
	default:
		break;
	}

	return;
}
/**************************************************************************
	��·����ʱ������߼�ͨ.ģ������(TBͨ��)��ģ������(AGENTͨ��)����ʱ�����
	�߼�ͨ����������(HBͨ��)�޸���Ϣ
	----------------------------------------------------------------------
	Function ������
		����˵����
		F_CC_HANDLE ���������䵽�ο�������þ��ֻ���ں������ص�����(�μ�CALL��
		����Ϣ)��
		F_CC_NORMAL ���������䵽�����߼�ͨ�����������ڽ����С�
		F_CC_NOCHANNEL ���������䲻������ͨ����������ֹ��ͨ���ͷ�
		����Ϣ������F_CC_HANDLE ���յ���
		�յ�����Ϣ�󲻻����յ�F_CC_HANDLE ��F_CC_NORMAL��Ҳ�����յ�
		MSG_RELEASE ��Ϣ��
	Param ����ʧ��ԭ��
		����˵����
		��Function=F_CC_NOCHANNELʱ����ֵΪ����ʧ��ԭ��
		P_CC_CHBUSY ָ��/����ͨ��ʹ���С�
		P_CC_CHCALLMASK ��������ͨ����������ֹ��
		P_CC_CHNOTEXIST ָ����ͨ���Ų����ڡ�
	�������������Ϣ
**************************************************************************/
void CTMS8E240::TMS_MSG_CallOutChannel(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( 0 >  nIndex || nIndex >= m_nTotalChannel ) return;
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	int nNodeType = pNodeObj->m_nType;
	switch( pmsg->Function )
	{
	case F_CC_NOCHANNEL://�����м��й�һ������ 	
		{
			if( NODETYPE_CHECK_DIAL == nNodeType )
				TMS_Dial_Result(  nIndex,  pmsg );//���������
			else
			{
				TMS_Switch_Judge(  nIndex,  pmsg );//���Switch���
				TMS_Transfer_Judge(  nIndex,  pmsg );
			}
		
		}
	default:
		break;
	}

	return;
}

/********************************************************
ģ������(AGENTͨ��)�ϵĵ绰������"����"("Flash")��
����"R"����ģ������(TBͨ��)�޸���Ϣ��
������(HBͨ��)���Լ�⵽�ö��������ޱ���Ϣ������
��MEDIA ��Ϣ(�μ�MEDIA ��Ϣ˵��)���͸��û���
*********************************************************/
void CTMS8E240::TMS_MSG_Flash(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( 0 >  nIndex || nIndex >= m_nTotalChannel ) return;
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	switch( m_pChannelInfo[nIndex].m_nChanelType )
	{
	case CF_CHTYPE_DTNO1://�����м��й�һ������ 
	case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
	case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
	case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
	case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
	case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
	case CF_CHTYPE_VIP://VOIPͨ������
		break;
	case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
		{
			if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
			{
				TMS_EventCheck_Flash( nIndex,  pmsg ); 
			}
		}
		break;
	case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
		break;
	case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
		break;
	case CF_CHTYPE_EMPTY://��ͨ�� 
		break;
	default:
		break;
	}

	return;
}
////////////////////////////////////////////
//˵����
//��ý���¼���Ϣһ��Ͷ�ý��������Ϣ���Ӧ��Ϊ��ý�������ִ�н�����յ��Ķ�ý����Ϣ
//���ڸ���Ϣ���ݽ϶࣬�ְ�ÿһ�ֶ�ý�����ͷֱ�˵����Ŀǰ����"����"��"˫����Ƶ"��"�ź���"��
//"����"��"���ݴ���"��
//ע�⣺
//��ý�幦��һ�����������е�ģ���������·��
////////////////////////////////////////////
void CTMS8E240::TMS_MSG_Media(RMSG *pmsg)
{
	if( NULL == pmsg ) return;
	// ͨ����Ϣȡ��CIFϵͳ���߼�ͨ����
	int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
	if( 0 >  nIndex || nIndex >= m_nTotalChannel ) return;
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;

	switch(pmsg->LinkChType)
	{
	case RESTYPE_VOCPCM:
		TMS_MSG_Media_VOCPCM(pmsg);
		break;
	case RESTYPE_DTMF:
	      if(pmsg->Function==F_MEDIA_Rx_Memory) //�յ�dtmf
		  {
            if(pmsg->ChType==CHTYPE_HB) //����ģ�鵥������
              TMS_MSG_RecvDTMF_HB(nIndex,pmsg);
			else
              TMS_MSG_RecvDTMF(nIndex,pmsg);            
		  }
		  else if(pmsg->Function==F_MEDIA_Tx_Release) //�յ��������
		  {

		  }
		break;
	case RESTYPE_FAX:
		TMS_MSG_Fax(  nIndex,  pmsg );
		break;
	case RESTYPE_FSK:
		break;
	case RESTYPE_CAS:
		break;
	case RESTYPE_MODEM:
		break;
	default:
		break;

	}

}
/*************************************************
	����·�Ĵ�����Ϣ����
*************************************************/
void CTMS8E240::TMS_MSG_Fax(int nIndex, RMSG *pmsg)
{
	CNodeObj * pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	if( NODETYPE_FAXRECIVE != pNodeObj->m_nType && NODETYPE_FAXSEND != pNodeObj->m_nType ) return;
	switch( pmsg->Function )
	{
	case F_MEDIA_Tx_Release://���ʹ������
	case F_MEDIA_Rx_Release://���մ������
		{
			switch(pmsg->Param)
			{
			case RESTOP_FINISH://���������������
				{
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
					return;
				}
				break;
			case RESTOP_USER://����������û���ֹ
			case RESTOP_CHREL://�����������ͨ���ͷŶ�����
			case RESTOP_COMMERR://���������������ʧ�ܶ�����
			case RESTOP_FILERR://������������ļ��������������
			default:
				{
					m_strMSG.Format("%s-%s ChIndex = %d",  pNodeObj->m_strCaption, GetStopReason(pmsg->Param), nIndex);
					m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
					return;
				}
				break;
			}


		}
		break;
	case F_MEDIA_Error://��Ӧ�Ĵ���������Ϣ����
		{
			switch(pmsg->Param)
			{
			case P_MEDIA_AllocOk://���䴫����Դ
				break;
			case P_MEDIA_AllocErr://�޷����䴫����Դ
			case P_MEDIA_BusyErr://ͨ����æ�ڴ�����������
			case P_MEDIA_FullErr://���Ͷ��л������Ѿ���������û�б�����
			case P_MEDIA_FuncErr://��֧�ָ��ִ����ʽ
			case P_MEDIA_FileErr://�����ļ�������
			default:
				{
					m_strMSG.Format("%s-%s ChIndex = %d",  pNodeObj->m_strCaption, GetResErrReason(pmsg->Param), nIndex);
					m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
					return;

				}
				break;
			}
		}
		break;
	default:
		break;
	}

	return;
}

/*************************************************
	����·����ǿ�ͻ���¼������Ϣ����
*************************************************/
void CTMS8E240::TMS_MSG_Media_VOCPCM(RMSG *pmsg)
{
	if( RESTYPE_ECONF == pmsg->ChType )
	{
		//����
		int nGroup =  pmsg->ChIndex;
		if(  nGroup < 0 || nGroup >= m_nTotalConf )
		{
			return;
		}
		int nRecordIndex = m_pConference[ nGroup ].nRecordIndex;
		int nPlayIndex = m_pConference[ nGroup ].nPlayIndex;
		if( F_MEDIA_Tx_Release == pmsg->Function )
		{
			m_pConference[ nGroup ].nPlayIndex = -1;
		}
		if( F_MEDIA_Rx_Release == pmsg->Function )
		{
			m_pConference[ nGroup ].nRecordIndex = -1;
		}
		if(  nRecordIndex < 0 || nRecordIndex >= m_nTotalChannel )
		{
			return;
		}
		if(  nPlayIndex < 0 || nPlayIndex >= m_nTotalChannel )
		{
			return;
		}

		CNodeObj * pRecObj = m_pChannelInfo[nRecordIndex].GetCurrentNodeObj();
		CNodeObj * pPlayObj = m_pChannelInfo[nPlayIndex].GetCurrentNodeObj();
		if( NULL != pRecObj )
		{
			if( NODETYPE_START_RECORD  == pRecObj->m_nType )
			{
				TMS_StartRecord_Result( nRecordIndex, pmsg);
				m_pConference[ nGroup ].nRecordIndex = -1;
			}
		}
		if( NULL != pPlayObj )
		{
			if( NODETYPE_START_PLAY  == pPlayObj->m_nType )
			{
				TMS_Play_Result( nPlayIndex, pmsg);
				m_pConference[ nGroup ].nPlayIndex = -1;
			}
		}
		

	}
	else
	{
		//ͨ��¼��������
		int nIndex = TMS_GetCifLogicChnl( pmsg->ChIndex, pmsg->ChType );
		if( 0 >  nIndex || nIndex >= m_nTotalChannel ) return;
		CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
		if( NULL == pNodeObj ) return;
		if( NODETYPE_START_PLAY == pNodeObj->m_nType )
		{
			TMS_Play_Result(  nIndex,  pmsg );
		}
		else if( NODETYPE_START_RECORD  == pNodeObj->m_nType )
		{
			TMS_StartRecord_Result( nIndex, pmsg);
		}
	}


	return;
}
/*/////////////////////////////////////////////////
ģ��������ܹ��յ����룺
��V�� ��⵽�����������������Ϊ����¼���Ŀ�ʼ
��S�� ��⵽���������Ѿ�ֹͣһ��ʱ��(10��)������Ϊ����¼������ͣ/����
��R�� ��⵽���������źſ�ʼ
��U�� ��⵽���������ź�ֹͣ
��T�� ��⵽���������ź�ֹͣ��û��������һ�������źţ���ʾ���κ��б���
���绰û��ժ��Ӧ��
��O�� ��⵽�������绰ժ��������Ϊѹ��¼���Ŀ�ʼ
��H�� ��⵽�������绰�һ�������Ϊѹ��¼���Ľ���
��F�� ��⵽�������绰�Ĳ��
0x80��0xFF ��ʾ����Ϊһ��FSK���к��룬��[Bit6:Bit0]��ʾ
���磺0xB1��[Bit6:Bit0]Ϊ0x31������1������ʾ�յ�һ��FSK��ʽ���͵���
�к���ֵ1
��f�� ��⵽FSK��ʽ���͵����к����ز���ʧ����Ϊ���к��������־����ʱ
����������꣬Ҳ�������ڱ������绰��ǰժ�����ж��˷���
ע�⣺���������ġ��߼�ͨ��/����ģ�顱�����У����Զ�ÿһ������������ժ�һ�
����ѹ����0ֵ����Ҫ���������������Ӳ��ģ��֧�֣�,�ò�������Ӱ�������
�ġ�O������H���źŵļ��
*///////////////////////////////////////////////////
void CTMS8E240::TMS_MSG_RecvDTMF_HB(int nIndex,RMSG *pmsg)
{
  CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
  if(pmsg->DataSpec&0x80)//fsk ����
  {
		m_strMSG.Format("%c",pmsg->DataSpec&0x7F);
		m_pChannelInfo[nIndex].m_strDTMFRecived += m_strMSG;
	 
	  //TRACE("Type= %d Ch= %d FSK=%c\n",pmsg->ChType,pmsg->ChIndex,pmsg->DataSpec&0x7F);  
  }
  else
  { 
	 if(pmsg->DataSpec=='O') //��⵽�������绰ժ��������Ϊѹ��¼���Ŀ�ʼ
	 {
			if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
			{
				TMS_EventCheck_OffHook( nIndex,  pmsg ); 
			}
	 }
	 else if(pmsg->DataSpec=='H') //��⵽�������绰�һ�������Ϊѹ��¼���Ľ���
	 {

		if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
		{
			TMS_EventCheck_Release(nIndex,pmsg);
			return;
		}
		TMS_ResetChannel( nIndex );

	 }
	 else if(pmsg->DataSpec=='V')//��⵽�����������������Ϊ����¼���Ŀ�ʼ
	 {
			if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
			{
				TMS_EventCheck_OffHook( nIndex,  pmsg ); 
			}

	 }
	 else if(pmsg->DataSpec=='S')//��⵽���������Ѿ�ֹͣһ��ʱ��(10��)������Ϊ����¼������ͣ/����
	 {
		if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
		{
			TMS_EventCheck_Release(nIndex,pmsg);
			return;
		}
		TMS_ResetChannel( nIndex );
	 }
	 else if(pmsg->DataSpec=='R')
	 {
		m_pChannelInfo[nIndex].m_strDTMFRecived = "";
		if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
		{
			TMS_EventCheck_Ringing(nIndex,pmsg);
			return;
		}
	 }
	 else if(pmsg->DataSpec=='f')
	 {
		//�������к������
		 m_pChannelInfo[nIndex].m_strCallerID = m_pChannelInfo[nIndex].m_strDTMFRecived;
	 }
  }
	
	return;
}

void CTMS8E240::TMS_MSG_RecvDTMF(int nIndex,RMSG *pmsg)
{
	CNodeObj *pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NODETYPE_EVENT_CHECK == pNodeObj->m_nType )
	{
		TMS_EventCheck_RecDTMF( nIndex, pmsg );
		return;
	}
	else
	{
		int code = pmsg->DataSpec;
		switch( pmsg->DataSpec )
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '*':
		case '#':
			m_pChannelInfo[nIndex].m_nEventType = EVENTTYPE_RECDTMF;
			m_strMSG.Format("%c",pmsg->DataSpec);
			m_pChannelInfo[nIndex].m_strDTMFRecived += m_strMSG;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'M':
		case 'V':
		case 'S':
		case 'U':
		case 'T':
		case 'O':
		case 'H':
		case 'F':
		default:
			break;
		}

	}

	return;
}

////////////////////////////////////////////
//����DTMF
////////////////////////////////////////////
void CTMS8E240::TMS_DTMFRecive( int nIndex )
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CDTMFReciveObj *pObj = (CDTMFReciveObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDTMFReciveObj) ) )
	{
		m_strMSG.Format("DTMFRecive error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}

	m_pPubCObj->PC_ShowChannelDTMF( nIndex,pChnlInfo->m_strDTMFRecived);

	int nStep		= pChnlInfo->m_nStep;
	switch( nStep )
	{
	case 0:
		{
			m_pPubCObj->PC_StartTimer( pChnlInfo );
			if( pObj->m_bClearDTMF ) 
			{
				pChnlInfo->m_strDTMFRecived = "";
			}
			pChnlInfo->m_strState = "ReciveDTMF";
			int  len = strlen(pChnlInfo->m_strDTMFRecived);
			if( 0 == pObj->m_nEndCondition )
			{
				//ͨ���жϽ��հ�������������������ռ�
				if( len  >= pObj->m_nDTMFCount )
					pChnlInfo->m_nStep = 2;
				else
					pChnlInfo->m_nStep = 1;

			}
			else
			{
				//ͨ���жϽ����ض�����ֵ�������ռ�
				if( pChnlInfo->m_strDTMFRecived.Right(1) == pObj->m_strDTMF )
				{
					pChnlInfo->m_strDTMFRecived.Replace(pObj->m_strDTMF,NULL);
					pChnlInfo->m_nStep = 2;
				}
				else
				{
					pChnlInfo->m_nStep = 1;
				}
			}
		}
		break;
	case 1:
		{
			if( "" == pChnlInfo->m_strDTMFRecived ) return;
			if( 0 == pObj->m_nEndCondition )
			{
				//ͨ���жϽ��հ�������������������ռ�
				int  len = pChnlInfo->m_strDTMFRecived.GetLength();
				if( len  >= pObj->m_nDTMFCount )
				{
					m_pChannelInfo[nIndex].m_nStep = 2;
					return;
				}
			}
			else
			{
				//ͨ���жϽ����ض�����ֵ�������ռ�
				if( pChnlInfo->m_strDTMFRecived.Right(1) == pObj->m_strDTMF )
				{
					pChnlInfo->m_strDTMFRecived.Replace(pObj->m_strDTMF,NULL);
					pChnlInfo->m_nStep = 2;
					return;
				}
			}
			long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
			if( n >= pObj->m_nTimeOut )
			{
				m_pPubCObj->PC_SaveValuesToVar( pChnlInfo, pChnlInfo->m_strDTMFRecived, pObj->m_SaveDTMF.m_nKey );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				m_strMSG.Format("%sDTMFRecive error, TimeOut Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				return;
			}
			

		}
		break;
	case 2:
		{
			BOOL bResult = m_pPubCObj->PC_SaveValuesToVar( pChnlInfo, pChnlInfo->m_strDTMFRecived, pObj->m_SaveDTMF.m_nKey );
			if( bResult )
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
			}
			else
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				m_strMSG.Format("%s DTMFRecive error,Save DTMF to Var Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			}
			
		}
	default:
		break;
	}

	return;

}

void CTMS8E240::TMS_DTMFMenu(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CDTMFMenuObj *pObj = (CDTMFMenuObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDTMFMenuObj) ) )
	{
		m_strMSG.Format("DTMFMenu Error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	int nStep = pChnlInfo->m_nStep;
	switch( nStep )
	{
	case 0:
		{
			m_pPubCObj->PC_StartTimer( pChnlInfo );
			if( pObj->m_bClearDTMF )
			{
				pChnlInfo->m_strDTMFRecived = "";
			}
			pChnlInfo->m_nState  = CHANNEL_STATE_CONNECT ; 
			m_pPubCObj->PC_ShowChannelState( nIndex, "Busy" );
			pChnlInfo->m_nStep = 1;
		}
		break;
	case 1:
		{		
			if( "" == pChnlInfo->m_strDTMFRecived )
				return;

			CString strCode =  pChnlInfo->m_strDTMFRecived.Right(1) ;
			int nNextType = 0;
			if( "*" == strCode )
				nNextType = NEXTTYPE_DTMF_11;	
			else if( "#" == strCode )
				nNextType = NEXTTYPE_DTMF_10;
			else
				nNextType = atoi(strCode) + 60;
			if( nNextType >= 60 && nNextType <= 71 )
			{
				TMS_MoveNext_Node( nIndex,  nNextType );
				return;
			}
			long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
			if( n >= pObj->m_nTimeOut )
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_DTMF_TIMEOUT );
				return;
			}

		}
		break;
	default:
		break;
	}
	return;

}

/////////////////////////////////////////////////////////
//ͨ��TMS�������豸ͨ�����ͣ�ȡ��CIF����ĵ��߼�ͨ������
/////////////////////////////////////////////////////////
inline int CTMS8E240::TMS_GetCifChnlType(int nDevChnlType)
{

	if( CHCLASS_NONE == nDevChnlType) //����·����
		return CF_CHTYPE_EMPTY;
	//else if( CHCLASS_DT  == nDevChnlType)  //������
	//	return CHCLASS_DT;
	//else if(  CHCLASS_AT   == nDevChnlType)  //ģ����
	//	return CHCLASS_AT;
	else if(  CHTYPE_DTNO1  == nDevChnlType)  //�����м��й�һ������ 
		return CF_CHTYPE_DTNO1;
	else if(  CHTYPE_DTNO7   == nDevChnlType)  //�����м��й��ߺ�����(TUP)
		return CF_CHTYPE_DTNO7;
	else if(  CHTYPE_DTDSS1   == nDevChnlType)  //�����м�ISDN PRI(30B+D)
		return CF_CHTYPE_DTDSS1;
	else if(  CHTYPE_DTNO7ISUP  == nDevChnlType) //�����м��й��ߺ�����(ISUP)
		return CF_CHTYPE_DTNO7ISUP;
	else if(  CHTYPE_MTP3	 == nDevChnlType) //�ߺ�����MTP�����ͨ��
		return CF_CHTYPE_MTP3;
	else if(  CHTYPE_TB    == nDevChnlType)  //ģ���м�����(��ͨ�л���),�ӵ��ž�
		return CF_CHTYPE_TRUNK;
	else if(  CHTYPE_AGENT  == nDevChnlType)  //ģ���м�����(��ϯ��)���ӵ绰��
		return CF_CHTYPE_USER;
	else if(  CHTYPE_HB    == nDevChnlType)  //ģ���м̸�����(�����)����绰��/�绰���ಢ��
		return CF_CHTYPE_RECORD;
	else
		return CF_CHTYPE_NOKNOW;
}

/////////////////////////////////////////////////////////
//ͨ��CIF����ĵ��߼�ͨ������,ȡ��TMS�������豸ͨ������
/////////////////////////////////////////////////////////
inline int CTMS8E240::TMS_GetDevChnlType(int nCifChnlType)
{

	if( CF_CHTYPE_EMPTY == nCifChnlType) //����·����
		return CHCLASS_NONE;
	else if(  CF_CHTYPE_DTNO1   == nCifChnlType)  //�����м��й�һ������ 
		return CHTYPE_DTNO1;
	else if( CF_CHTYPE_DTNO7  == nCifChnlType)  //�����м��й��ߺ�����(TUP)
		return CHTYPE_DTNO7 ;
	else if( CF_CHTYPE_DTDSS1  == nCifChnlType)  //�����м�ISDN PRI(30B+D)
		return  CHTYPE_DTDSS1 ;
	else if( CF_CHTYPE_DTNO7ISUP == nCifChnlType) //�����м��й��ߺ�����(ISUP)
		return  CHTYPE_DTNO7ISUP ;
	else if(  CF_CHTYPE_MTP3 == nCifChnlType) //�ߺ�����MTP�����ͨ��
		return CHTYPE_MTP3	;
	else if( CF_CHTYPE_TRUNK == nCifChnlType)  //ģ���м�����(��ͨ�л���),�ӵ��ž�
		return CHTYPE_TB  ;
	else if( CF_CHTYPE_USER == nCifChnlType)  //ģ���м�����(��ϯ��)���ӵ绰��
		return  CHTYPE_AGENT;
	else if( CF_CHTYPE_RECORD == nCifChnlType)  //ģ���м̸�����(�����)����绰��/�绰���ಢ��
		return CHTYPE_HB  ;
	else
		return CHCLASS_NONE;
}


/////////////////////////////////////////////////////////
//ͨ��TMS�������豸��ͨ�����Լ�ͨ�����ͣ�ȡ��CIF����ĵ��߼�
//ͨ����
/////////////////////////////////////////////////////////
int CTMS8E240::TMS_GetCifLogicChnl(int nDevChnlIndex, int nDevChnlType)
{
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		if( nDevChnlIndex == m_pChannelInfo[i].m_nChannel )
		{
			if( m_pChannelInfo[i].m_nChanelType == TMS_GetCifChnlType(nDevChnlType) ) 
			{
				return i;
			}
		}
	}

	return -1;
}

void CTMS8E240::TMS_ShowConfState(int nGroup)
{
	if( nGroup < 0 || nGroup >= m_nTotalConf ) return;
	char state[30];
	char tmpstr[100];
	memset( state, 0, sizeof(state) );
	memset( tmpstr, 0, sizeof(tmpstr) );

	itoa( m_pConference[nGroup].nMemberNum , state, 10);//���������˵���ĵĻ����Ա��
	m_plstConfer->GetItemText( nGroup, 2, tmpstr, 32 ) ;
	if ( strcmp(state,tmpstr )!=0) 
		m_plstConfer->SetItemText( nGroup, 2, state );

	itoa(m_pConference[nGroup].nResNum,state,10);//����������Ѿ�ռ�õĻ�����Դ��
	m_plstConfer->GetItemText(nGroup,1,tmpstr,32);
	if(strcmp(state,tmpstr)!=0)
		m_plstConfer->SetItemText( nGroup,1,state);

	itoa(m_pConference[nGroup].nListenNum,state,10);//��������м����ĵĻ����Ա��
	m_plstConfer->GetItemText(nGroup,3,tmpstr,32);
	if(strcmp(state,tmpstr)!=0)
		m_plstConfer->SetItemText( nGroup,3,state);
		
	return;
}

void CTMS8E240::TMS_ConfDoWork()
{

	for(int i = 0; i < m_nTotalConf; i++ )
	{
		TMS_ShowConfState( i );
		switch( m_pConference[i].State )
		{
		case CONF_FREE://����״̬��û��ռ��
			break;
		case CONF_PLAY_INDEX:
			break;
		case CONF_RECORD://���ڻ���¼��״̬
		case CONF_PLAY://���ڻ������״̬
		case CONF_ADD:
		case CONF_LEAVE:
			//m_pConference[i].nConfPlayID=-1;
			//m_pConference[i].State=CONF_PLAY;
		case CONF_EXIT:
			//TMS_StopConfPlay(i);
			//TMS_StopConfRecord(i);
			//m_pConference[i].nConfPlayID=-1;
			//m_pConference[i].State=CONF_FREE;
			break;
		}
	}
	return;
}
void CTMS8E240::TMS_DoWork(int nIndex)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	int nChType = pChnlObj->m_nChanelType;
	CNodeObj *pNodeObj = pChnlObj->GetCurrentNodeObj();
	if( NULL == pNodeObj || nIndex < 0 ) return;
	if( CF_CHTYPE_EMPTY == nChType ) return;
	if( CF_CHTYPE_NOKNOW == nChType ) return;

	switch( pNodeObj->m_nType )	
	{
	case NODETYPE_START:
		TMS_MoveNext_Node( nIndex, NEXTTYPE_NEXT );//���Ҹý���ָ֧��ĵĽ��
		break;
	case NODETYPE_WAITCALL:
		TMS_WaitCall( nIndex);
		break;
	case NODETYPE_START_PLAY:
		TMS_Play( nIndex );
		break;
	case NODETYPE_CHECK_DIAL:
		TMS_DialCheck( nIndex );
		break;
	case NODETYPE_RELEASECALL:
		TMS_ReleaseCall( nIndex );
		break;
	case NODETYPE_CALLHOLD:
		TMS_CallHold( nIndex );
		break;
	case NODETYPE_CHECK_TRANSFER:
		TMS_Transfer( nIndex );
		break;
	case NODETYPE_START_RECORD:
		TMS_StartRecord( nIndex );
		break;
	case NODETYPE_SETVARIABLE:
		m_pPubCObj->PC_SetVariable( nIndex,pChnlObj );
		break;
	case NODETYPE_COMPARE:
		m_pPubCObj->PC_Compare( nIndex,pChnlObj );
		break;
	case NODETYPE_ENTER_CONFERENCE:
		TMS_ConferenceEnter(nIndex);
		break;
	case NODETYPE_LEAVE_CONFERENCE:
		TMS_ConferenceLeave(nIndex);
		break;
	case NODETYPE_FAXRECIVE:
		TMS_FaxRecive(  nIndex );
		break;
	case NODETYPE_FAXSEND:
		TMS_FaxSend(  nIndex );
		break;
	case NODETYPE_CONFERENCE_BUILD:
		TMS_ConferenceBuild( nIndex );
		break;
	case NODETYPE_CONFERENCE_REQUEST:
		//Confer_Call( nChnl );
		break;
	case NODETYPE_CONFERENCE_DELETE:
		TMS_ConferenceDelete( nIndex );
		break;
	case NODETYPE_GET_CONFERENCE_MEMBER:
		break;
	case NODETYPE_OPEN_DATABASE:
		m_pPubCObj->PC_OpenDatabase( nIndex,pChnlObj );
		break;
	case NODETYPE_CLOSE_DATABASE:
		m_pPubCObj->PC_CloseDatabase( nIndex,pChnlObj );
		break;
	case NODETYPE_EXCUTE_SQL:
		m_pPubCObj->PC_ExcuteSQL( nIndex,pChnlObj );
		break;
	case NODETYPE_DLL_LOAD:
		m_pPubCObj->PC_DLLLoad(nIndex,pChnlObj);
		break;
	case NODETYPE_DLL_UNLOAD:
		m_pPubCObj->PC_DLLUnLoad(nIndex,pChnlObj);
		break;
	case NODETYPE_DLL_CALL:
		m_pPubCObj->PC_DLLCall(nIndex,pChnlObj);
		break;
	case NODETYPE_SEND_DTMFCODE:
		TMS_DTMFSend(nIndex);
		break;
	case NODETYPE_RECIVE_DTMF:
		TMS_DTMFRecive( nIndex );
		m_pPubCObj->PC_ShowChannelDTMF( nIndex,pChnlObj->m_strDTMFRecived);
		break;
	case NODETYPE_EVENT_CHECK:
		TMS_EventCheck_MSG( nIndex );
		return;
		break;
	case NODETYPE_DATABASE_BROWSE:
		m_pPubCObj->PC_DBBrowse( nIndex,pChnlObj );
		break;
	case NODETYPE_DTMF_MENU:
		TMS_DTMFMenu( nIndex );
		m_pPubCObj->PC_ShowChannelDTMF( nIndex,m_pChannelInfo[nIndex].m_strDTMFRecived);
		break;
	case NODETYPE_CALLLOGREQUEST:
		TMS_CallLogRequest( nIndex );
		break;
	case NODETYPE_NODE_END:
		TMS_NodeEnd( nIndex);
		break;
	case NODETYPE_SWITCH:
		TMS_Switch( nIndex );	
		break;
	case NODETYPE_ANSWERCALL:
		TMS_AnswerCall( nIndex );	
		break;
	case NODETYPE_BLOCK:
		m_pPubCObj->PC_Block( nIndex ,pChnlObj);
		break;
	case NODETYPE_ENTER:
		m_pPubCObj->PC_Enter( nIndex ,pChnlObj);
		break;
	case NODETYPE_RETURN:
		m_pPubCObj->PC_Return( nIndex,pChnlObj );
		break;
	case NODETYPE_STOREPRO:
		m_pPubCObj->PC_StoredProcedures( nIndex,pChnlObj ); 
		break;
	case NODETYPE_CHANGEVOICE:
		//TMS_ChangeVoice( nIndex );
		break;
	case NODETYPE_ENDCALL_BLOCK:
		break;
	case NODETYPE_ENDCALLEVENT_CHECK:
		TMS_EndCallEventCheckBlock( nIndex );
		break;
	case NODETYPE_ENDCALLEVENT_END:
		TMS_EndCallEventEndBlock( nIndex );
		break;
	case NODETYPE_FUNCTION:
		m_pPubCObj->PC_Function( nIndex,pChnlObj ,m_nTotalChannel, &m_pChannelInfo[0] );
		//TMS_Function( nIndex );
		break;
	case NODETYPE_EXPRESS:
		m_pPubCObj->PC_Express( nIndex ,pChnlObj);
		break;
	case NODETYPE_GOTO:
		m_pPubCObj->PC_GoTo( nIndex,pChnlObj );
		break;
	case NODETYPE_ROUTE_CALL:
		
		break;
	case NODETYPE_TRACE_INFO:
		m_pPubCObj->PC_TraceInfo( nIndex,pChnlObj );
		break;
	case NODETYPE_MOUDLE:
		
		break;
	case NODETYPE_MOUDLE_BEGIN:
		
		break;
	case NODETYPE_MOUDLE_END:
		
		break;
	case NODETYPE_CUSTOM:
		
		break;
	default:
		break;

	}

}

void CTMS8E240::TMS_WaitCall(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CNodeObj *pNodeObj = pChnlInfo->GetCurrentNodeObj();
	if( NODETYPE_WAITCALL != pNodeObj->m_nType ) return;
	CWaitCallObj *pObj = (CWaitCallObj*)pNodeObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CWaitCallObj) )  )
	{
		m_strMSG.Format("ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}


	m_pChannelInfo[nIndex].m_strCallerID="";
	m_pChannelInfo[nIndex].m_strCalledID="";


	if( pmsg->GetCallerLen() )
	{
		m_pChannelInfo[nIndex].m_strCallerID= pmsg->GetCaller() ;
	}
	if( pmsg->GetCalledLen() )
	{
		m_pChannelInfo[nIndex].m_strCalledID=pmsg->GetCalled() ;
	}
	if( pObj->m_bRecCallerID )
		m_pPubCObj->PC_SaveValuesToVar( pChnlInfo, m_pChannelInfo[nIndex].m_strCallerID, pObj->m_CallerID.m_nKey );
	if( pObj->m_bRecCalledID )
		m_pPubCObj->PC_SaveValuesToVar( pChnlInfo,m_pChannelInfo[nIndex].m_strCalledID, pObj->m_CalledID.m_nKey );
	if( pObj->m_bClearDTMF ) 
		m_pChannelInfo[nIndex].m_strDTMFRecived	= "" ;

	/****************************/
	//TMS_FaxKey( nIndex );
   /*****************************/


	m_pChannelInfo[nIndex].m_nCallType			= CHANNEL_CALLTYPE_COMEIN;
	m_pChannelInfo[nIndex].m_nRings				= 1;
	m_pChannelInfo[nIndex].m_nStep				= 1;
	m_pChannelInfo[nIndex].m_nState				= CHANNEL_STATE_CONNECT;
	m_pChannelInfo[nIndex].m_nEventType			=  EVENTTYPE_OFFHOOK;
	m_pChannelInfo[nIndex].m_strtBeginTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	

	m_pPubCObj->PC_ShowChannelCallerID(nIndex,m_pChannelInfo[nIndex].m_strCallerID);
	m_pPubCObj->PC_ShowChannelCalledID(nIndex,m_pChannelInfo[nIndex].m_strCalledID);
	m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");

	if( pObj->m_bAnswerCall )
	{
		//Ӧ��Ƿ�
		TMS_Ack( pmsg);
	}
	else
	{
		//Ӧ�����
		TMS_Ack( pmsg,FALSE);
	}
	

	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );

	

	return;
}


///////////////////////////////////////////////////////////
//˵����
//��ģ������(TBͨ��)ժ�����ģ������(AGENTͨ��)��Ӧ(ֹͣ���ź���)��
//������(HBͨ��)����ʹ�á�
//��������·�ĺ���Ӧ�𡣼Ʒѡ�ͨ�����Կ�ʼ��
/////////////////////////////////////////////////
void CTMS8E240::TMS_Ack(RMSG *pmsg, BOOL bIsANC )
{
  pmsg->InitDataLen(0);
  pmsg->MsgType		= MSG_CALLEDACK;
  if(bIsANC)
	pmsg->Function	= F_ACK_ANC;//F_ACK_ANC �Ʒ�(Ĭ��ֵ) F_ACK_ANN ���
  else
	pmsg->Function	= F_ACK_ANN;//F_ACK_ANC �Ʒ�(Ĭ��ֵ) F_ACK_ANN ���
  pmsg->Param		= 0;
  PCISENDMSG( pmsg );
  return;
}

void CTMS8E240::TMS_FaxSend_Msg(int nIndex, char *filename)
{
	if( NULL == filename ) return;
	if( 0 > nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Tx_File;
	msg.Param		= RESPAR_PFIRSTMSG | RESPAR_PLASTMSG;
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.ChIndex		= pChnlInfo->m_nChannel;
	msg.LinkChType	= RESTYPE_FAX;
	msg.LinkChIndex = 0;
	msg.DataSpec	= FAXFILE_TIFF;
	msg.AddStr(filename); 
	PCISENDMSG(&msg);
}

void CTMS8E240::TMS_FaxRecive_Msg(int nIndex, char *filename)
{
	if( NULL == filename ) return;
	if( 0 > nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Rx_File;
	msg.Param		= RESPAR_PFIRSTMSG | RESPAR_PLASTMSG;
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.ChIndex		= pChnlInfo->m_nChannel;
	msg.LinkChType	= RESTYPE_FAX;
	msg.LinkChIndex = 0;
	msg.DataSpec	= FAXFILE_TIFF;
	msg.AddStr(filename); 
	PCISENDMSG(&msg);
}

void CTMS8E240::TMS_Ack( int nIndex ,BOOL bIsANC )
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_CALLEDACK;
	if(bIsANC)
		msg.Function	= F_ACK_ANC;//F_ACK_ANC �Ʒ�(Ĭ��ֵ) F_ACK_ANN ���
	else
		msg.Function	= F_ACK_ANN;//F_ACK_ANC �Ʒ�(Ĭ��ֵ) F_ACK_ANN ���
	msg.Param		= 0;
	msg.ChIndex		= pChnlInfo->m_nChannel;
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.LinkChIndex = 0;
	msg.LinkChType	= 0;
	msg.DataLen		= 0;
	msg.DataSpec	= 0;
	PCISENDMSG( &msg );


	return;
}

//��������
void CTMS8E240::TMS_ConF_Create( int nIndex, int nConferNO,int nMaxMember )
{
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);

	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_CONF;
	msg.Function	= F_CF_CREATE;
	msg.Param		= 128;
	msg.ChIndex		= pChnlInfo->m_nChannel;//������ϯ�߼�ͨ����
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );//������ϯ�߼�ͨ������
	msg.LinkChIndex = nConferNO;//�����Һ�
	if(  1 == m_nConfCard )
	{
		msg.LinkChType = RESTYPE_ECONF;//��ǿ�͵绰����
		msg.DataLen	= 2;
		msg.DataW[0] = 64;
	}
	else
	{
		msg.LinkChType = RESTYPE_CONF;//��ͨ�͵绰���飬
	}
	msg.DataSpec = nMaxMember;// �����Ҵ�С1��127��CONF����1��31(ECONF)
	PCISENDMSG( &msg );
	return;
}

//�뿪����
void CTMS8E240::TMS_ConF_Leave( int nIndex, int nConferNO )
{
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);

	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_CONF;
	msg.Function	= F_CF_UNLINKCH;
	msg.Param		= 0;
	msg.ChIndex		= pChnlInfo->m_nChannel;//������ϯ�߼�ͨ����
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );//������ϯ�߼�ͨ������
	msg.LinkChIndex = nConferNO;//�����Һ�
	if(  1 == m_nConfCard )
		msg.LinkChType = RESTYPE_ECONF;//��ǿ�͵绰����
	else
		msg.LinkChType = RESTYPE_CONF;//��ͨ�͵绰���飬
	PCISENDMSG( &msg );
	return;
}
/********************************************************************
	�������
	nMode: 0 SpeakListen; 1 ListenOnly ; 2 SpeakOnly
*********************************************************************/
void CTMS8E240::TMS_ConF_Enter( int nIndex, int nConferNO, int nMode )
{
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);

	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_CONF;
	if( 0 == nMode )
	{
		msg.Function	= F_CF_LINKCH;
		msg.Param		= 128;
	}
	else 
	{
		if( 0 == m_nConfCard )
			msg.Function	= F_CF_LINKCH;
		else
			msg.Function	= F_CF_MONITOR;
		msg.Param		= 0;
	}
	
	msg.ChIndex		= pChnlInfo->m_nChannel;//������ϯ�߼�ͨ����
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );//������ϯ�߼�ͨ������
	msg.LinkChIndex = nConferNO;//�����Һ�
	if(  1 == m_nConfCard )
		msg.LinkChType = RESTYPE_ECONF;//��ǿ�͵绰����
	else
		msg.LinkChType = RESTYPE_CONF;//��ͨ�͵绰���飬
	PCISENDMSG( &msg );
	return;
}
//�رջ���
void CTMS8E240::TMS_ConF_Close( int nIndex, int nConferNO )
{
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);

	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_CONF;
	msg.Function	= F_CF_CLOSE;
	msg.Param		= 0;
	msg.ChIndex		= pChnlInfo->m_nChannel;//������ϯ�߼�ͨ����
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );//������ϯ�߼�ͨ������
	msg.LinkChIndex = nConferNO;//�����Һ�
	if(  1 == m_nConfCard )
		msg.LinkChType = RESTYPE_ECONF;//��ǿ�͵绰����
	else
		msg.LinkChType = RESTYPE_CONF;//��ͨ�͵绰���飬
	PCISENDMSG( &msg );
	return;
}
///////////////////////////////////////////////////////
//��ģ������(TBͨ��)�һ����ģ������(AGENTͨ��)��æ����
//������(HBͨ��)����ʹ��,
//��������·�����ͷ�������ȴ��ͷŽ��.
void CTMS8E240::TMS_Release(int nIndex)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType = MSG_RELEASE;
	msg.ChType  = TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.ChIndex = pChnlInfo->m_nChannel;
	msg.Function=0;
	msg.Param=0;
	msg.DataLen=0;
	PCISENDMSG(&msg);


	return;
}
/*************************************************************
�����ļ�������Ϣ��
����isfirst��islast����������
	first = 1,last = 1����������ֻ��һ���ļ�
	first = 1,last = 0��һ���ļ�
	first = 0,last = 1���һ���ļ�
	first = 0,last = 0�м���ļ�
nIndex �߼�ͨ����
nGroup �߼�����ţ�nGroup >= 0ʱ���������nGroup < 0ʱ��ͨ������
bDTMFStop �Ƿ񰴼����
**************************************************************/
void CTMS8E240::TMS_PlayFile(int nIndex ,int nGroup, char *file, unsigned short isfirst, unsigned short islast,unsigned short bDTMFStop)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	if( nGroup >= m_nTotalConf  ) return;
	if( file == NULL ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Tx_File;
	if( nGroup >= 0 ){
		
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
	}
	else{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
	}
	msg.Param		= (isfirst?RESPAR_PFIRSTMSG:0) | (bDTMFStop?RESPAR_DTMFSTOP:0) | (islast?RESPAR_PLASTMSG:0);
	//���÷����������Ƿ��ǵ�һ���ļ����Ƿ������һ���ļ����Ƿ񰴼���ϵ�
	msg.LinkChType	= RESTYPE_VOCPCM;
	msg.LinkChIndex	= 0;
	msg.DataSpec	= PFDATA_ALAW;//�ļ���ʽ
	msg.AddStr(file); //���127���ֻ�254�ַ�

	PCISENDMSG(&msg);
}

void CTMS8E240::TMS_PlayFile(RMSG *pmsg, char *file, unsigned short isfirst, unsigned short islast, unsigned short bDTMFStop)
{
  if( file == NULL ) return;
  pmsg->InitDataLen(0);
  pmsg->MsgType		= MSG_MEDIA;
  pmsg->Function		= F_MEDIA_Tx_File;
  pmsg->Param			= (isfirst?RESPAR_PFIRSTMSG:0) | (bDTMFStop?RESPAR_DTMFSTOP:0) | (islast?RESPAR_PLASTMSG:0);
  pmsg->LinkChType	= RESTYPE_VOCPCM;
  pmsg->LinkChIndex	= PFDATA_ALAW;//�ļ���ʽ
  pmsg->DataSpec		= PFDATA_ALAW;//�ļ���ʽ
  pmsg->AddStr(file); //���127���ֻ�254�ַ�

  PCISENDMSG( pmsg );
}

void CTMS8E240::TMS_PlayTTS(int nIndex, int nGroup, char *str, unsigned char isfirst, unsigned char islast, unsigned short bDTMFStop)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	if( nGroup >= m_nTotalConf  ) return;
	if( str == NULL || strlen(str) >= 254 ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Tx_TTStr;
	if( nGroup >= 0 ){
		
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
	}
	else{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
	}
	msg.Param		=  (isfirst?RESPAR_PFIRSTMSG:0) | (bDTMFStop?RESPAR_DTMFSTOP:0) | (islast?RESPAR_PLASTMSG:0);
	msg.LinkChType	= RESTYPE_VOCPCM;
	msg.LinkChIndex	= PFDATA_ALAW;//�ļ���ʽ
	msg.AddStr(str); //���127���ֻ�254�ַ�
	PCISENDMSG(&msg);
}

void CTMS8E240::TMS_PlayTTS(RMSG *pmsg, char *str, unsigned char isfirst, unsigned char islast, unsigned short bDTMFStop)
{
	if( str == NULL || strlen(str) >= 254 ) return;
	pmsg->InitDataLen(0);
	pmsg->MsgType		= MSG_MEDIA;
	pmsg->Function	= F_MEDIA_Tx_TTStr;
	pmsg->Param		= (isfirst?RESPAR_PFIRSTMSG:0) | (bDTMFStop?RESPAR_DTMFSTOP:0) | (islast?RESPAR_PLASTMSG:0);
	pmsg->LinkChType	= RESTYPE_VOCPCM;
	pmsg->LinkChIndex	= 0;
	pmsg->AddStr(str); //���127���ֻ�254�ַ�
	PCISENDMSG(pmsg);
}

void CTMS8E240::TMS_CallLogRequest(int nIndex)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	CCallLogRequestObj *pObj = (CCallLogRequestObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CCallLogRequestObj) ) )
	{
		m_strMSG.Format("CallLogRequest ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}


	if( 0 == pObj->m_nRequestType ) //Begin Save Call Log
	{
		pChnlObj->m_strtBeginTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	}
	else
	{
		pChnlObj->m_strtEndTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	}

	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
	return;

}
////////////////////////////////////////////////////////////
//��ģ������(TBͨ��)�յ������źŻ�������·���е绰����
//ʱ,�øú�������,
///////////////////////////////////////////////////////////
void CTMS8E240::TMS_EventCheck_Ringing(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}
	
	pChnlInfo->m_nCallType	= CHANNEL_CALLTYPE_COMEIN;
	pChnlInfo->m_nEventType =  EVENTTYPE_RINGING;
	pChnlInfo->m_nState		=  CHANNEL_STATE_RINGING;
	m_pChannelInfo[nIndex].m_strRingTime = (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	m_pPubCObj->PC_ShowChannelState( nIndex, "Ringing");

	if( 0 == pChnlInfo->m_nStep )
	{
		pChnlInfo->m_nStep = 1;
		m_pPubCObj->PC_StartTimer( pChnlInfo);
		if(  pObj->m_bClearDTMF )
		{
			if( EVENTTYPE_RECDTMF == pChnlInfo->m_nEventType )
			{
				pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
			}
			pChnlInfo->m_strDTMFRecived = "";
		}
	}

	if(  pObj->m_bElapseTime )
	{
		if( m_pPubCObj->PC_ElapseTime( pChnlInfo) >=  pObj->m_nElapseTime )
		{
			//����ָ����ʱ�䣬������ʱ���¼�
			m_pPubCObj->PC_StartTimer( pChnlInfo);
			pChnlInfo->m_nEventType		=  EVENTTYPE_ELAPSE_TIME;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ELAPSE_TIME);
			return;
		}
	}
	if( pObj->m_bRinging )
	{
		int nChType = m_pChannelInfo[nIndex].m_nChanelType;
		switch( nChType )
		{
		case CF_CHTYPE_DTNO1://�����м��й�һ������ 
		case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
		case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
		case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
		case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
			{

				m_pChannelInfo[nIndex].m_strCallerID="";
				m_pChannelInfo[nIndex].m_strCalledID="";
				if( pmsg->GetCallerLen() )
				{
					m_pChannelInfo[nIndex].m_strCallerID=pmsg->GetCaller() ;
				}
				if( pmsg->GetCalledLen() )
				{
					m_pChannelInfo[nIndex].m_strCalledID= pmsg->GetCalled() ;
				}

				/****************************/
				//TMS_FaxKey( nIndex );
			   /*****************************/

				m_pChannelInfo[nIndex].m_nCallType			= CHANNEL_CALLTYPE_COMEIN;
				m_pChannelInfo[nIndex].m_nRings				= 1;
				m_pChannelInfo[nIndex].m_nStep				= 1;
				m_pChannelInfo[nIndex].m_nState				= CHANNEL_STATE_CONNECT;
				m_pChannelInfo[nIndex].m_nEventType			=  EVENTTYPE_OFFHOOK;
				m_pChannelInfo[nIndex].m_strtBeginTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
				

				m_pPubCObj->PC_ShowChannelCallerID(nIndex,m_pChannelInfo[nIndex].m_strCallerID);
				m_pPubCObj->PC_ShowChannelCalledID(nIndex,m_pChannelInfo[nIndex].m_strCalledID);
				m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");
				
				//TMS_Acm( pmsg,0 );//���͵�ַȫ��Ϣ
				
			}

			break;
		default:
			break;
		}

		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_RINGING);
	}


	return;
}


BOOL CTMS8E240::TMS_EventCheck_Release(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return FALSE ;  
	}
	

	pChnlInfo->m_nEventType =  EVENTTYPE_HANGUP;
	if( pObj->m_bHangUp )
	{
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		return TMS_MoveNext_Node( nIndex,  NEXTTYPE_HANGUP);
	}
	else
	{
		//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		pChnlInfo->MoveCallEndBlock_Node();
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
//��ģ������(AGENTͨ��)�ϵĵ绰�ڿ���ʱ����ժ��
//ʱ���øú�������
//
///////////////////////////////////////////////////////////
void CTMS8E240::TMS_EventCheck_OffHook(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}
	
	if( 0 == pChnlInfo->m_nStep )
	{
		pChnlInfo->m_nStep = 1;
		m_pPubCObj->PC_StartTimer( pChnlInfo);
		if(  pObj->m_bClearDTMF )
		{
			pChnlInfo->m_strDTMFRecived = "";
		}
	}

	if(  pObj->m_bElapseTime )
	{
		if( m_pPubCObj->PC_ElapseTime( pChnlInfo) >=  pObj->m_nElapseTime )
		{
			//����ָ����ʱ�䣬������ʱ���¼�
			m_pPubCObj->PC_StartTimer( pChnlInfo);
			pChnlInfo->m_nEventType		=  EVENTTYPE_ELAPSE_TIME;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ELAPSE_TIME);
			return;
		}
	}

	//����ժ����Ϣ
	if( CF_CHTYPE_USER == pChnlInfo->m_nChanelType )
	{
		m_pPubCObj->PC_CTI_OffHookMsg( nIndex, pChnlInfo );
	}
	pChnlInfo->m_nState =  CHANNEL_STATE_CONNECT;
	pChnlInfo->m_nEventType = EVENTTYPE_OFFHOOK;
	m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");
	if( pObj->m_bOffHook )
	{
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_OFFHOOK);
	}

	return;
}

void CTMS8E240::TMS_EventCheck_RecDTMF(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}
	if(pmsg->ChType==CHTYPE_HB) return;
	if( 0 == pChnlInfo->m_nStep )
	{
		pChnlInfo->m_nStep = 1;
		m_pPubCObj->PC_StartTimer( pChnlInfo);
		if(  pObj->m_bClearDTMF )
		{
			if( EVENTTYPE_RECDTMF == pChnlInfo->m_nEventType )
			{
				pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
			}
			pChnlInfo->m_strDTMFRecived = "";
			return;
		}
	}

	if(  pObj->m_bElapseTime )
	{
		if( m_pPubCObj->PC_ElapseTime( pChnlInfo) >=  pObj->m_nElapseTime )
		{
			//����ָ����ʱ�䣬������ʱ���¼�
			m_pPubCObj->PC_StartTimer( pChnlInfo);
			pChnlInfo->m_nEventType		=  EVENTTYPE_ELAPSE_TIME;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ELAPSE_TIME);
			return;
		}
	}

	if(pmsg->LinkChType==RESTYPE_DTMF)
	{
		m_strMSG.Format("%c",pmsg->DataSpec);
		pChnlInfo->m_strDTMFRecived += m_strMSG ;
		m_pPubCObj->PC_ShowChannelDTMF( nIndex, pChnlInfo->m_strDTMFRecived );
		if( pObj->m_bRecDTMF )
		{
			pChnlInfo->m_nEventType		=  EVENTTYPE_RECDTMF;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_RECDTMF);
			m_pPubCObj->PC_ShowChannelDTMF( nIndex, pChnlInfo->m_strDTMFRecived );
			return;
		}
		
	}

	return;
}

void CTMS8E240::TMS_EventCheck_MSG(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}

	if( 0 == pChnlInfo->m_nStep )
	{
		pChnlInfo->m_nStep = 1;
		m_pPubCObj->PC_StartTimer( pChnlInfo);
		if(  pObj->m_bClearDTMF )
		{
			if( EVENTTYPE_RECDTMF == pChnlInfo->m_nEventType )
			{
				pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
			}
			pChnlInfo->m_strDTMFRecived = "";
		}
	}

	if(  pObj->m_bElapseTime )
	{
		if( m_pPubCObj->PC_ElapseTime( pChnlInfo) >=  pObj->m_nElapseTime )
		{
			//����ָ����ʱ�䣬������ʱ���¼�
			m_pPubCObj->PC_StartTimer( pChnlInfo);
			pChnlInfo->m_nEventType		=  EVENTTYPE_ELAPSE_TIME;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ELAPSE_TIME);
			return;
		}
	}

	switch( pChnlInfo->m_nEventType )
	{
	case EVENTTYPE_OFFHOOK:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( CF_CHTYPE_USER == pChnlInfo->m_nChanelType )
		{
			m_pPubCObj->PC_CTI_OffHookMsg( nIndex, pChnlInfo );
		}
		if( pObj->m_bOffHook )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_OFFHOOK);
			return ;
		}
		break;
	case EVENTTYPE_HANGUP:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( CF_CHTYPE_USER == pChnlInfo->m_nChanelType )
		{
			m_strMSG.Format("[Send Hangup DN:%s]", pChnlInfo->m_strLocalDN );
			m_pPubCObj->PC_ShowCommMsg( m_strMSG );
			
			
		}
		m_pPubCObj->PC_ShowChannelState( nIndex, "Free" );
		if( pObj->m_bHangUp )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_HANGUP);
			return ;
		}
		else
		{
			//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
			pChnlInfo->MoveCallEndBlock_Node();
		}
		break;
	case EVENTTYPE_HOOKFLASH:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bHookFlash )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_HOOKFLASH);
			return ;
		}
		break;
	case EVENTTYPE_RECDTMF:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bRecDTMF )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_RECDTMF);
			return ;
		}
		break;
	case EVENTTYPE_RECFAXTONE:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bRecFaxTone )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_RECFAXTONE);
			return ;
		}
		break;

	case EVENTTYPE_CONFDESTROYED:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bConfDestroyed )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_CONFDESTROYED);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_LOGIN_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentLoginMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_LOGIN_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_OUT_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentLogOutMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_OUT_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_BUSY_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentBusyMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_BUSY_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_INSERVE_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentInserveMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_INSERVE_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_ANSWER_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentAnswerMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_ANSWER_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_MAKECALL_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentMakeCallMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_MAKECALL_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_TRANSFER_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentTransferMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_TRANSFER_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_CONFER_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentConferMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_CONFER_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_HANGUP_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentHangUpMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_HANGUP_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_SENDDATA_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentSendDataMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_SENDDATA_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_SENDFAX_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentSendFaxMsg )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_SENDFAX_MSG);
			return ;
		}
		break;
	case EVENTTYPE_RINGING:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bRinging )
		{
			
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_RINGING);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_HOLD_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentHold )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_HOLD_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_PICKUPHOLD_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentPickupHold )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_PICKUPHOLD_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_MONITOR_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentMonitor )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_MONITOR_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_DISMONITOR_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentDisMonitor )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_DISMONITOR_MSG);
			return ;
		}
		break;
	case EVENTTYPE_AGENT_BUILDCONFER_MSG:
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		if( pObj->m_bAgentBuildConfer )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_AGENT_BUILDCONFER_MSG);
			return ;
		}
		break;
	default:
		break;
	
	}

	return ;
}

void CTMS8E240::TMS_EventCheck_Flash(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEventCheckObj *pObj = (CEventCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEventCheckObj) ) )
	{
		m_strMSG.Format("EventCheck error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return;
	}

	if( 0 == pChnlInfo->m_nStep )
	{
		pChnlInfo->m_nStep = 1;
		m_pPubCObj->PC_StartTimer( pChnlInfo);
		if(  pObj->m_bClearDTMF )
		{
			pChnlInfo->m_strDTMFRecived = "";
		}
	}

	if(  pObj->m_bElapseTime )
	{
		if( m_pPubCObj->PC_ElapseTime( pChnlInfo) >=  pObj->m_nElapseTime )
		{
			//����ָ����ʱ�䣬������ʱ���¼�
			m_pPubCObj->PC_StartTimer( pChnlInfo);
			pChnlInfo->m_nEventType		=  EVENTTYPE_ELAPSE_TIME;
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ELAPSE_TIME);
			return;
		}
	}

	m_pPubCObj->PC_ShowChannelState( nIndex, "HookFlash");
	if( pObj->m_bHookFlash )
	{
		pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_HOOKFLASH);
		return ;
	}


	return;
}


void CTMS8E240::TMS_EndCallEventCheckBlock(int nIndex)
{
	CNodeObj *pNodeObj= m_pChannelInfo[nIndex].GetCurrentNodeObj();
    if (pNodeObj == NULL )
	{
		m_strMSG.Format("EndCallEventCheckBlock Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		return ;   
	}
	

	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );


	return;
}

void CTMS8E240::TMS_EndCallEventEndBlock(int nIndex)
{
	CNodeObj *pNodeObj= m_pChannelInfo[nIndex].GetCurrentNodeObj();
    if (pNodeObj == NULL )
	{
		m_strMSG.Format("EndCallEventEndBlock Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		return ;   
	}



	m_pPubCObj->PC_ShowChannelState( nIndex, "Free");
	m_pPubCObj->PC_ShowConnectToChnl( nIndex, -1 );

	m_pChannelInfo[nIndex].MoveFirst_Node();

	return;
}


void CTMS8E240::TMS_NodeEnd(int nIndex)
{
	TMS_ResetChannel(nIndex);
	return;
}





int CTMS8E240::TMS_FindHoldChannel(int nIndex )
{
	int nChnlLink = -1;
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		if( nIndex == m_pChannelInfo[i].m_nLinkCh )
		{
			nChnlLink = i;
			break;
		}
	}

	return nChnlLink;
}

int CTMS8E240::TMS_FindUerChannel(CString strLocalDN )
{
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		if( strLocalDN == m_pChannelInfo[i].m_strLocalDN && CF_CHTYPE_USER == m_pChannelInfo[i].m_nChanelType )
		{
			return i;
		}
	}
	return -1;
}
int CTMS8E240::TMS_FindFreeTrunk()
{
	static int nCurrentTrunkNo = 0 ;
	int i = 0;
	for( i = nCurrentTrunkNo ; i < m_nTotalChannel ; i++ ) 
	{
		switch( m_pChannelInfo[i].m_nChanelType )
		{
			case CF_CHTYPE_DTNO1://�����м��й�һ������ 
			case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
			case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
			case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
			case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
			case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
			case CF_CHTYPE_VIP:
				if( CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
				{
					nCurrentTrunkNo = i+1 ;
					return i;
				}
				break;
			case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
			case CF_CHTYPE_EMPTY://��ͨ�� 
			case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
			case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
			default:
				break;
		}

	}
	
	for( i = 0 ; i < nCurrentTrunkNo ; i++ )
	{
		switch( m_pChannelInfo[i].m_nChanelType )
		{
			case CF_CHTYPE_DTNO1://�����м��й�һ������ 
			case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
			case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
			case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
			case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
			case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
			case CF_CHTYPE_VIP:
				if( CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
				{
					nCurrentTrunkNo = i+1 ;
					return i;
				}
				break;
			case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
			case CF_CHTYPE_EMPTY://��ͨ�� 
			case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
			case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
			default:
				break;
		}

	}

	return -1 ;
}
int CTMS8E240::TMS_FindFreeChannel(CString strLocalDN, int nChType )
{
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		if( nChType == m_pChannelInfo[i].m_nChanelType )
		{
			if( strLocalDN == m_pChannelInfo[i].m_strLocalDN )
				if( CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
					return i;
		}
	}
	return -1;
}


void CTMS8E240::TMS_Play(int nIndex)
{
	CPlayStartObj *pObj = (CPlayStartObj*)m_pChannelInfo[nIndex].GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CPlayStartObj) ) )
	{
		m_strMSG.Format("ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	TMS_Play_Start( nIndex, pObj );

	return;
}


void CTMS8E240::TMS_Play_Start(int nIndex,CPlayStartObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( 0 != pChnlInfo->m_nStep ) return;
	if( pObj->m_bClearDTMF ) 
	{
		pChnlInfo->m_strDTMFRecived = "";
	}
	
	CNodeDataList *pDadaList = &(pObj->m_PlayList);
	if( pDadaList->IsEmpty() )
	{
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		return;
	}

	int nGroupNo = -1;
	if( 1 == pObj->m_nPlayWay )//�������
	{
		int nVarKey = pObj->GetVarKey( pObj->m_strConfNO  );
		if( nVarKey < 0 )
		{
			nGroupNo = atoi(pObj->m_strConfNO );
		}
		else
		{
			if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, m_strMSG, pObj->GetVarKey( pObj->m_strConfNO ) ) )
			{
				m_strMSG.Format("%s Play error, ChIndex = %d    Play error ",  pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return;
			}
			nGroupNo = atoi(m_strMSG);
		}

		if(  nGroupNo < 0 || nGroupNo >= m_nTotalConf )
		{
			m_strMSG.Format("%s Play error, ChIndex = %d    Group error ",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
	}


	/****************************/
	//TMS_FaxKey( nIndex );
   /*****************************/

	CNodeDataList PlayList;
	POSITION pos = pDadaList->GetHeadPosition( );
	//���濪ʼ��ʼ�����Ŷ���
	CNodeData *pData = NULL;
	while( pos )
	{
		pData = pDadaList->GetNext(pos);
		if( NULL == pData ) continue;
		if( 0 == pData->m_nDataType )//���ļ�
		{
			CNodeData *pNewData = new CNodeData;
			m_pPubCObj->PC_GetExpressValues( pChnlInfo, m_strMSG, pData->m_strData);
			pNewData->m_strData = m_strMSG;
			pNewData->m_nDataType = 0;
			PlayList.AddTail(  pNewData );
		}
		else if( 1 == pData->m_nDataType )//�����ֱ��ʽ
		{
			//���ʽ
			if( !m_pPubCObj->PC_GetExpressValues( pChnlInfo, m_strMSG, pData->m_strData ) )
			{
				m_strMSG.Format("%s Play error, ChIndex = %d    Play Number Express Error :%s ",  pObj->m_strCaption, nIndex, pData->m_strData );
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return ;   
			}
			TMS_PlayExpressVox( nIndex, &PlayList , m_strMSG  );
		}
		else if( 2 == pData->m_nDataType )//TTS
		{
			
			if( !m_pPubCObj->PC_GetExpressValues( pChnlInfo, m_strMSG, pData->m_strData ) )
			{
				m_strMSG.Format("%s Play error, ChIndex = %d    Play TTS Express Error :%s ",  pObj->m_strCaption, nIndex, pData->m_strData );
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return ;   
			}
			CNodeData *pNewData = new CNodeData;
			pNewData->m_strData = m_strMSG;
			pNewData->m_nDataType = 1;
			PlayList.AddTail(  pNewData );
		}

	}//end while

	pos = PlayList.GetHeadPosition( );	
	int nListCount = PlayList.GetCount();
	int nPos = 0;
	while( pos )
	{
		nPos++;
		pData = PlayList.GetNext(pos);
		if( NULL == pData ) continue;
		
		char *pszFileName = (LPTSTR)(LPCTSTR)pData->m_strData;
		if( 0 == pData->m_nDataType )//���ļ�
		{
			if( 1 == nListCount  )
			{
				TMS_PlayFile( nIndex , nGroupNo ,pszFileName , 1, 1, pObj->m_bPlayStop );//����������ֻ��һ���ļ�
			}
			else
			{
				if( 1 == nPos )
					TMS_PlayFile( nIndex ,nGroupNo , pszFileName , 1, 0, pObj->m_bPlayStop );//��һ���ļ�
				else 
				{
					if( nListCount <= nPos )
						TMS_PlayFile( nIndex ,nGroupNo , pszFileName , 0, 1, pObj->m_bPlayStop );//���һ���ļ�
					else
						TMS_PlayFile( nIndex ,nGroupNo , pszFileName , 0, 0, pObj->m_bPlayStop );//�м���ļ�

				}

			}

		}
		else//TTS
		{
			if( 1 == nListCount  )
			{
				TMS_PlayTTS( nIndex , nGroupNo ,pszFileName , 1, 1, pObj->m_bPlayStop );//����������ֻ��һ���ļ�
			}
			else
			{
				if( 1 == nPos )
					TMS_PlayTTS( nIndex , nGroupNo ,pszFileName , 1, 0, pObj->m_bPlayStop );//��һ���ļ�
				else 
				{
					if( nListCount <= nPos )
						TMS_PlayTTS( nIndex , nGroupNo ,pszFileName , 0, 1, pObj->m_bPlayStop );//���һ���ļ�
					else
						TMS_PlayTTS( nIndex , nGroupNo ,pszFileName , 0, 0, pObj->m_bPlayStop );//�м���ļ�

				}

			}

		}

	}
	
	//�ͷ���Դ
	while ( PlayList.GetHeadPosition() )
	{
		delete PlayList.RemoveHead();
	}

	if( 1 == pObj->m_nPlayWay )
	{

	}
	else
	{
		
	}
	pChnlInfo->m_nStep = 1;
	return;
}

void CTMS8E240::TMS_Play_Result( int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CNodeObj *pNodeObj = pChnlInfo->GetCurrentNodeObj();
	if( NODETYPE_START_PLAY != pNodeObj->m_nType ) return;
	CPlayStartObj *pObj = (CPlayStartObj*)pNodeObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CPlayStartObj) ) )
	{
		m_strMSG.Format("Play error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	/***********************************/
	//TMS_TransferKey( nIndex );//��������
    /***********************************/

	if( F_MEDIA_Tx_Release == pmsg->Function )
	{
		if( RESTOP_FILERR == pmsg->Param )
		{
			m_strMSG.Format("%s Play error, ChIndex = %d    Play error ",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
		else
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		}
	}
	else if( F_MEDIA_Error == pmsg->Function )
	{
		if( P_MEDIA_AllocOk != pmsg->Param )
		{
			m_strMSG.Format("%s Play error, ChIndex = %d    Play error %s",  pObj->m_strCaption, nIndex,GetResErrReason(pmsg->Param));
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
	}

	return;
}
void CTMS8E240::TMS_PlayExpressVox(int nIndex, CNodeDataList *p, CString strExpress)
{
	if( NULL == p || "" == strExpress ) return ;
	int nStrLen = strExpress.GetLength();
	CString strPaht = m_strPath+"\\SysVoiceFle\\";
	CString strFileName("");
	for( int i = 0; i< nStrLen; i++ )
	{
		strFileName = "";
		if( '0' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D0";
		}
		else if( '1' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D1";
		}
		else if( '2' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D2";
		}
		else if( '3' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D3";
		}
		else if( '4' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D4";
		}
		else if( '5' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D5";
		}
		else if( '6' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D6";
		}
		else if( '7' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D7";
		}
		else if( '8' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D8";
		}
		else if( '9' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D9";
		}
		else if( '.' == m_strMSG.GetAt(i) )
		{
			strFileName = strPaht + "D15";
		}

		if( "" != strFileName )
		{
			CNodeData *pp = new CNodeData;
			pp->m_strData = strFileName;
			pp->m_nDataType= 0;
			p->AddTail(pp);
		}

	}
	
	return ;
}


void CTMS8E240::TMS_DialCheck(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( 0 != pChnlInfo->m_nStep ) return;
	CDialCheckObj *pObj = (CDialCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDialCheckObj) ) )
	{
		m_strMSG.Format("DialCheck Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( pObj->m_bError )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	
	int nKey = pObj->GetVarKey( pObj->m_strPhone );
	CString strPhone = "";
	if( nKey < 0 )
	{
		strPhone = pObj->m_strPhone;//������Ǻ��봮�����Ǳ���
	}
	else
	{
		if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strPhone, nKey ) )
		{
			m_strMSG.Format("%s Dial tel Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			if( pObj->m_bError )
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
	}

	/****************************/
	//TMS_TCPIP( nIndex );
   /*****************************/
	
	char szPhone[32] ;
	memset( szPhone, 0 , sizeof(szPhone) );
	memmove( szPhone, strPhone, strlen(strPhone) >= 32 ? 31:strlen(strPhone) );
	if( strlen(szPhone) < 3 )
	{
		//���볤��С��3ʱ������Ϊ�������
		m_strMSG.Format("%s  Phone Error; ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( pObj->m_bError )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	CString strPrefix = pObj->m_strPrefix;
	char szPrefix[32];
	memset( szPrefix, 0 , sizeof(szPrefix) );
	memmove( szPrefix, strPrefix, strlen(strPrefix) >= 32 ? 31:strlen(strPrefix) );
	TMS_MakeCall( nIndex,szPhone, pChnlInfo->m_strLocalDN  );
	pChnlInfo->m_nStep			= 1;

	return;
}
/*************************************************
���зֻ�������ⲿ�绰
**************************************************/
void CTMS8E240::TMS_MakeCall(int nIndex, const char *calledstr, const char *callerstr)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nChnlType = pChnlInfo->m_nChanelType;
	RMSG msg; 
	msg.InitDataLen(0);
	msg.MsgType	= MSG_CALL;
	msg.Function= F_C_NORMAL;
	msg.Param	= P_C_NORMAL;//default param
	msg.ChType	= TMS_GetDevChnlType( nChnlType );
	msg.ChIndex	= pChnlInfo->m_nChannel;
	msg.LinkChType	= 0;
	msg.LinkChIndex	= 0;//���
	msg.InitCallNull();//�������/���к���Ϊ��
	msg.AppendCalled( calledstr ); //����
	msg.AppendCaller( callerstr );//����+����+Դ����
	PCISENDMSG( &msg );
	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_strDTMFRecived	= "";


	pChnlInfo->m_strCallerID = callerstr ;
	pChnlInfo->m_strCalledID = calledstr ;

	if( CF_CHTYPE_USER == nChnlType )
	{
		//���зֻ����У���������
		pChnlInfo->m_nCallType = CHANNEL_CALLTYPE_IN;
		pChnlInfo->m_nState		= CHANNEL_STATE_RINGING;
		pChnlInfo->m_nEventType =  EVENTTYPE_RINGING;


		m_pPubCObj->PC_ShowChannelState( nIndex, "Ringing");
		m_pPubCObj->PC_CTI_RingingMsg( atoi( pChnlInfo->m_strLocalDN), callerstr, calledstr );
		
	}
	else
	{
		pChnlInfo->m_nCallType = CHANNEL_CALLTYPE_CALLOUT;
		pChnlInfo->m_nState		= CHANNEL_STATE_DIAL;
		pChnlInfo->m_nEventType	=  EVENTTYPE_OFFHOOK;
		m_pPubCObj->PC_ShowChannelState( nIndex, "Dial" );
	}
	return;
}

void CTMS8E240::TMS_Dial_Result(int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( 0 == pChnlInfo->m_nStep ) return;
	CDialCheckObj *pObj = (CDialCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDialCheckObj) ) )
	{
		m_strMSG.Format("DialCheck Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( pObj->m_bError )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	switch( pmsg->MsgType )
	{
	case MSG_RELEASE:
		if( F_R_CALLOUTERR == pmsg->Function )//����ʧ��
		{
			pChnlInfo->m_strDTMFRecived	= "";
			pChnlInfo->m_nState			= CHANNEL_STATE_FREE;
			pChnlInfo->m_nStep			= 0;
			pChnlInfo->m_nEventType		=  EVENTTYPE_HANGUP;
			pChnlInfo->m_nLinkCh			= -1;
			m_pPubCObj->PC_ShowChannelState( nIndex, "Free" );
			switch(pmsg->Param)
			{
			case P_R_NODIALTONE  :     // 1   //û�м�⵽������ 
				if( pObj->m_bNoDialTone)
				{
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_NO_DIALTONE );
				}
				else
				{
					//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
					pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
					pChnlInfo->MoveCallEndBlock_Node();
				}				
				break;
			case P_R_RINGTIMEOUT   :   // 3   //���峬ʱ �����˽ӣ�
			case P_R_NOANSWER     :     //2   //û����Ӧ
				if( pObj->m_bNoAnswer)
				{
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_NO_ANSWER );
				}
				else
				{
					//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
					pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
					pChnlInfo->MoveCallEndBlock_Node();
				}
				break;
			case P_R_BUSYLOCAL    :    // 19  //�л�æ
			case P_R_BUSYDDD       :   // 20  //����æ
				if( pObj->m_bNoDialTone)
				{
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_BUSY );
				}
				else
				{
					//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
					pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
					pChnlInfo->MoveCallEndBlock_Node();
				}	
				break;
			case P_R_NULLDN         :   //22  //�պ�
			case P_R_NODIGITALPATH   : // 26  //û������ͨ·
			case P_R_LINEUSED       :  // 4  //��·�����Է�ռ�û�û�и�λ���
			case P_R_CONGESTION      :  //12  //�豸ӵ��
			case P_R_INCOMPLETE       : //15  //��ַ��ȫ
			case P_R_FAILURE           ://16  //���й���
			case P_R_CALLBARRING :      //18  //����ܾ�
			default://��������ʧ�ܷ�ʽ
				if( pObj->m_bError)
				{
					TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				}
				else
				{
					//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
					pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
					pChnlInfo->MoveCallEndBlock_Node();
				}	
				break;
			}

		}
		break;
	case MSG_CALLEDACK:
		//���ӳɹ�
		pChnlInfo->m_nState			= CHANNEL_STATE_CONNECT;
		pChnlInfo->m_strState ="Connect";
		m_pPubCObj->PC_ShowChannelState( nIndex, "Connect" );
		if( pObj->m_bConnect )
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_CONNECT );
		}
		else
		{
			//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
			pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
			pChnlInfo->MoveCallEndBlock_Node();
		}
		break;
	case UPMSG_CALLOUTCHANNEL:
		if( F_CC_NOCHANNEL == pmsg->Function )
		{
			if( pObj->m_bError)
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			}
			else
			{
				//����ʱ�����һ��¼�����û����Ӧ�Ĵ�����ֱ�ӻص���ʼ���
				pChnlInfo->m_nEventType = EVENTTYPE_NOTHING;
				pChnlInfo->MoveCallEndBlock_Node();
			}	
		}
	default:
		break;
	}

	return;
}


void CTMS8E240::TMS_Function(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CFunctionObj *pObj = (CFunctionObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CFunctionObj) ) )
	{
		m_strMSG.Format("Function error , ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}

	try
	{
			int nPos1	= -1;
			int nPos2	= -1;
			int nPos3	= -1;
			int nVarKey = -1;
			BOOL bResult = TRUE;
			CString strVar("");
			CString strVarValues("");
			
			
			CString strOldExpress = pObj->m_strFunction;
			strOldExpress.Replace( "\"" , NULL );
			strOldExpress.Replace("+",NULL);
			CString strNewExpress = "";
			while(1)
			{
				nPos1 = strOldExpress.Find( '|', nPos3+1 );
				if( -1 == nPos1 )
				{
					//ȡ�ú���ĳ�����
					strVarValues = strOldExpress.Right( strlen(strOldExpress) - nPos3-1 );
					strNewExpress = strNewExpress + strVarValues;
					break;
				}
				strNewExpress =  strNewExpress + strOldExpress.Mid( nPos3+1, nPos1 - nPos3-1 ) ;
				nPos2 = strOldExpress.Find( ':', nPos1+1 );
				nPos3 = strOldExpress.Find( '|', nPos1+1 );
				strVar = strOldExpress.Mid(nPos1+2,nPos2-nPos1-2);
				nVarKey = atoi(strVar);
				if( 0 == nVarKey ) 
				{
					bResult = FALSE;
					break;
				}
				int nVarType = 0;
				if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strVarValues,nVarType, nVarKey ) )
				{
					//û���ҵ��ñ���
					bResult = FALSE;
					break;
				}
				if( "" == strVarValues ){
					if( DATATYPE_INTEGER == nVarType || DATATYPE_FLOAT == nVarType )
						strNewExpress = strNewExpress + "0";
				}
				else
					strNewExpress = strNewExpress + strVarValues;

			}//end while

			if( bResult )
			if( "" == strNewExpress ) 
			{
				//����û�б���
				strNewExpress = strOldExpress;
			}

			strNewExpress.Replace(" ", NULL);
			CString str("");
			CString str1("");
			CString str2("");
			CString strReturn("");
			int nReturn=0;
			if( strNewExpress.Left(5) == "Left(" )
			{
				strNewExpress.Replace("Left(",NULL);
				int i = strNewExpress.Find(';');
				str = strNewExpress.Left(i);
				
				str1= strNewExpress.Mid( i );
				str1.Replace(";",NULL);
				str1.Replace(")",NULL);

				i = atoi(str1);
				strReturn = str.Left(i);

			}
			else if( strNewExpress.Left(4) == "Len(" )
			{
				strNewExpress.Replace( "Len(", NULL);
				strNewExpress.Replace( ")", NULL);
				nReturn = strlen( strNewExpress );
				strReturn.Format("%d",nReturn);

			}
			else if( strNewExpress.Left(4) == "Mid(" )
			{
				strNewExpress.Replace("Mid(",NULL);
				int i = strNewExpress.Find(';');
				str = strNewExpress.Left(i);
				int j = strNewExpress.Find(';',i+1);
				str1 = strNewExpress.Mid(i+1,j-i);
				str1.Replace(";",NULL);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
				str2 = strNewExpress.Mid(j+1);
				str2.Replace(")",NULL);   
				strReturn = str.Mid( atoi(str1), atoi(str2) );

			}
			else if( strNewExpress.Left(6) == "Right(" )
			{
				strNewExpress.Replace("Right(",NULL);
				int i = strNewExpress.Find(';');
				str = strNewExpress.Left(i);
				
				str1= strNewExpress.Mid( i );
				str1.Replace(";",NULL);
				str1.Replace(")",NULL);

				i = atoi(str1);
				strReturn = str.Right(i);
			}
			else if( strNewExpress.Left(12) == "AgentIsFree(" )
			{
				strNewExpress.Replace("AgentIsFree(",NULL);
				int i = strNewExpress.Find(';');
				str = strNewExpress.Left(i);
				
				str1= strNewExpress.Mid( i );
				str1.Replace(";",NULL);
				str1.Replace(")",NULL);
				
				i = TMS_FindUerChannel( str1 );
				strReturn.Format( "%d",i );
			}
			else if( strNewExpress.Left(12) == "GetHoldChnl(" )
			{
				int nHold = GetHoldChnl( nIndex );
				strReturn.Format( "%d",nHold );
			}
			else if( strNewExpress.Left(13) == "GetFreeAgent(" )
			{
				int nHold = TMS_FindFreeChannel( CF_CHTYPE_USER );
				strReturn.Format( "%d",nHold );
			}

			//���淵��ֵ
			int nVarKey1 = pObj->GetVarKey( pObj->m_strReturnValues);
			if( m_pPubCObj->PC_SaveValuesToVar( pChnlInfo,strReturn, nVarKey1 ) )
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
			}
			else
			{
				m_strMSG.Format("%s Function error , Save the return Values of the function  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			}

	}
	catch(...)
	{
		m_strMSG.Format("%s Function error ,unknown  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
	}


	return;
}

void CTMS8E240::TMS_DealCTIMsg(CFC_MESSAGE_PACK *pMsg)
{
	int nMsgType = pMsg->nMsgType;
	int nDN = pMsg->nSMID;
	
	CString strMSG("");
	strMSG.Format("%d",nDN);
	int nIndex = TMS_FindUerChannel( strMSG );
	if( nMsgType != CFMTYPE_REG_COMM )
	{
		if( 0 > nIndex  || nIndex >= m_nTotalChannel )
		{
			strMSG.Format("[Recive MType:%d][Message Error From DN:%d]", nMsgType, nDN );
			m_pPubCObj->PC_ShowSysMsg(0,strMSG);	
			return;
		}
	}

	m_lComm = pMsg->lCardKey;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	switch( nMsgType )
	{
		case CFMTYPE_MONITOR://Monitoring
			{
				//ͨ������
				CFMSG_MONITOR recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_MONITOR) );
				strMSG.Format("[Recive MType:Agent Monitoring][DN:%d][DDN:%s]", pMsg->nSMID, recMsg.szTel);
				m_pPubCObj->PC_ShowCommMsg( strMSG );
				pChnlInfo->m_strDialTel.Format("%s", recMsg.szTel );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_MONITOR_MSG;
			}
			break;
		case CFMTYPE_DMONITOR://Disable Monitoring
			{

			}
			break;
		case CFMTYPE_MCALL://Make Call
			{
				//ͨ������
				CFMSG_MAKECALL recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_MAKECALL) );
				strMSG.Format("[Recive MType:Agent MakeCall][DN:%d][DDN:%s][CallType:%d]", pMsg->nSMID, recMsg.szTel,recMsg.nCallType);
				m_pPubCObj->PC_ShowCommMsg( strMSG );

				//�ҵ�������Ϣ������ȷͨ������
				pChnlInfo->m_nDialType = recMsg.nCallType;
				pChnlInfo->m_strDialTel.Format("%s", recMsg.szTel );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_MAKECALL_MSG;
				m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");
			}
			break;

		case CFMTYPE_TRANSFER://Transfer
			{
				//ͨ������
				CFMSG_TRANSFER recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_TRANSFER) );
				strMSG.Format("[Recive MType:Agent Transfer][DN:%d][DDN:%s]", pMsg->nSMID, recMsg.szTel);
				m_pPubCObj->PC_ShowCommMsg( strMSG );
				pChnlInfo->m_nDialType = recMsg.nCallType;
				pChnlInfo->m_strDialTel.Format("%s", recMsg.szTel );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_TRANSFER_MSG;

			}
			break;
		case CFMTYPE_CANCEL_TRANSFER://Cancel transfer
			{
				//ͨ������
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_CANCEL_TRANSFER;
				strMSG.Format("[Recive MType:Agent CancelTransfer][DN:%d]", nDN  );
				m_pPubCObj->PC_ShowCommMsg( strMSG );
			}
			break;
		case CFMTYPE_ANSWER://Answer
			{
				//ͨ������
				strMSG.Format("[Recive MType:Agent AnswerCall][DN:%d]", nDN  );
				m_pPubCObj->PC_ShowCommMsg(strMSG );

				//�ҵ�������Ϣ������ȷͨ������
				//������绰AnswerCallʱ������
				pChnlInfo->m_nEventType	= EVENTTYPE_OFFHOOK;
			}
			break;
		case CFMTYPE_RELEASECALL://Release Call
			{

			}
			break;
		case CFMTYPE_HOLD://Hold Request
			{
				//ͨ������
				CFMSG_HOLD recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_HOLD) );
				strMSG.Format("[Recive MType:Agent Hold][DN:%d]", pMsg->nSMID);
				m_pPubCObj->PC_ShowCommMsg( strMSG );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_HOLD_MSG;

			}
			break;
		case CFMTYPE_PICKUPHOLD://Pickup Hold Request
			{
				//ͨ������
				CFMSG_PICKUPHOLD recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_PICKUPHOLD) );
				strMSG.Format("[Recive MType:Agent PickupHold][DN:%d]", pMsg->nSMID);
				m_pPubCObj->PC_ShowCommMsg( strMSG );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_PICKUPHOLD_MSG;

			}
			break;
		case CFMTYPE_SENDFAX://Send Fax Request
			{

			}
			break;
		case CFMTYPE_LOGIN://Set Feature Login
			{

			}
			break;
		case CFMTYPE_LOGOUT://Set Feature Logout
			{

			}
			break;
		case CFMTYPE_MSETBBUSY://Set Feature MSB
			{

			}
			break;
		case CFMTYPE_MSI://Set Feature MSI
			{

			}
			break;
		case CFMTYPE_SFNOTR://Set Feature Not Ready
			{

			}
			break;
		case CFMTYPE_SFR://Set Feature  Ready
			{

			}
			break;
		case CFMTYPE_REG_COMM://reg to Comm and response
			{
				m_pPubCObj->PC_ShowSysMsg(0,"Regist to communicate server success");
			}
			break;
		case CFMTYPE_NET_ABEND://Net abend message type
			{

			}
			break;
		case CFMTYPE_HANGUP:
			{
				//ͨ������
				strMSG.Format("[MType:HangUp][From DN:%d]", nDN  );
				m_pPubCObj->PC_ShowCommMsg(strMSG );
				TMS_Release( nIndex );
			}
			break;
		case CFMTYPE_LCONFER://Leave Conference 
			{

			}
			break;
		case CFMTYPE_CONFER_CALL:
			{
				//ͨ������
				CFMSG_CONFER_CALL recMsg;
				memset(&recMsg, 0, sizeof(recMsg) );
				memmove( &recMsg, pMsg->szMessageBody ,sizeof(CFMSG_CONFER_CALL) );
				strMSG.Format("[DN:%d][ConferCall:%s][CallType:%d]", pMsg->nSMID, recMsg.szTel,recMsg.nCallType);
				m_pPubCObj->PC_ShowCommMsg( strMSG );

				//�ҵ�������Ϣ������ȷͨ������
				pChnlInfo->m_nDialType = recMsg.nCallType;
				pChnlInfo->m_strDialTel.Format("%s", recMsg.szTel );
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_CONFER_MSG;
				m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");
			}
			break;
		case CFMTYPE_BUILD_CONFER:
			{
				//ͨ������
				strMSG.Format("[MType:Build Conference][From DN:%d]", nDN  );
				m_pPubCObj->PC_ShowCommMsg(strMSG );

				//�ҵ�������Ϣ������ȷͨ������
				pChnlInfo->m_nEventType	= EVENTTYPE_AGENT_BUILDCONFER_MSG;
				m_pPubCObj->PC_ShowChannelState( nIndex, "Busy");

				strMSG.Format("[MType:Build Conference][TO DN:%d]", nDN  );
				m_pPubCObj->PC_ShowCommMsg( strMSG );
			}
			break;
		default:
			break;
	}


	return;
}



void CTMS8E240::TMS_Switch(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CSwitchObj *pObj = (CSwitchObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CSwitchObj) ) )
	{
		m_strMSG.Format("Switch Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	
	
	/***********************************/
	//TMS_TransferKey( nIndex );//��������
   /*************************************/
	
	
	switch( pObj->m_nSwitchType )
	{
		case CF_CHSWITCH_CONNECT://ֱ������
		case CF_CHSWITCH_MONITOR://����
		case CF_CHSWITCH_SPEAK://������������������������
		case CF_CHSWITCH_DISCONNECT://�Ͽ�����	
			if( 0 == pObj->m_nAimType )
				TMS_Switch_Self( nIndex,  pObj );
			else
				TMS_Switch_Aim( nIndex,  pObj );
			break;
		case CF_CHSWITCH_CALLAGENT://������ϯ����ʱ��ͨ����Ϊ�ֻ�����
			if(  0 == pChnlInfo->m_nStep  )
				TMS_Switch_CallAgent( nIndex,  pObj );
			else
				TMS_Switch_TimeOut( nIndex,  pObj );
			break;
		case CF_CHSWITCH_CALLCUSTOMER://���пͻ�
			if(  0 == pChnlInfo->m_nStep  )
				TMS_Switch_CallCustomer( nIndex,  pObj );
			else
				TMS_Switch_TimeOut( nIndex,  pObj );
			break;
		default:
			break;
	}
	

	return;

}

void CTMS8E240::TMS_Switch_CallAgent(int nIndex, CSwitchObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if(  pChnlInfo->m_nStep > 0 ) return;
	int nAimIndex	= pChnlInfo->m_nLinkCh ;
	int nChType		= pChnlInfo->m_nChanelType;

	CString strCallTel("");
	int nVarKey = pObj->GetVarKey( pObj->m_strAimChnl );
	if( nVarKey < 0 )
	{
		strCallTel = pObj->m_strAimChnl;
	}
	else if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strCallTel, nVarKey) )
	{
		m_strMSG.Format("%s not get aim telephone; ChIndex = %d ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	nAimIndex = TMS_FindFreeChannel( strCallTel, CF_CHTYPE_USER );
	if( -1 >= nAimIndex || nAimIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s Channel index is  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	CFChannelInfo *pAimChnlInfo = &(m_pChannelInfo[nAimIndex]);
	if( CF_CHTYPE_USER != pAimChnlInfo->m_nChanelType )
	{
		m_strMSG.Format("%s Aim Channel not is User channel, index is  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	pChnlInfo->m_nLinkCh = nAimIndex;

	//���зֻ����У���������
	pAimChnlInfo->m_nLinkCh		= nIndex;
	if( CF_CHTYPE_USER == nChType)
	{
		TMS_MakeCall( nAimIndex, pAimChnlInfo->m_strLocalDN, pChnlInfo->m_strLocalDN );
	}
	else
	{
		TMS_MakeCall( nAimIndex, pChnlInfo->m_strCalledID, pChnlInfo->m_strCallerID );
	}


	//switch�ĵȴ�ѡ�һ��ֻ���ں���ʱ��
	if( 0 == pObj->m_nWaitOption )//���Ż�����
	{
		TMS_StartPlaySignal(nIndex, TMS_RINGBACK_SIG);
	}
	else if( 2 == pObj->m_nWaitOption )//���������ļ�
	{
		
		char *pszFileName = (LPTSTR)(LPCTSTR)pObj->m_strPlayFile;
		TMS_PlayFile_Loop( nIndex , -1 ,pszFileName , 1, 1, false );
	}

	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_nStep = 1;
	
		
	return;
}

void CTMS8E240::TMS_Switch_CallCustomer(int nIndex, CSwitchObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if(  pChnlInfo->m_nStep > 0 ) return;
	int nAimIndex	= pChnlInfo->m_nLinkCh ;
	CString strCallTel("");
	int nVarKey = pObj->GetVarKey( pObj->m_strAimChnl );
	if( nVarKey < 0 )
	{
		strCallTel = pObj->m_strAimChnl;
	}
	else if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strCallTel, nVarKey) )
	{
		m_strMSG.Format("%s not get aim telephone; ChIndex = %d ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}


	//ȡ���Ⲧ������ͨ��
	if( pObj->m_nGetFreeChWay > 0 )
	{

		int nBeginCH = 0;
		int nEndCH = 0;
		CString strNewExpress("");
		if(  !m_pPubCObj->PC_GetExpressValues( pChnlInfo,  strNewExpress, pObj->m_strBeginCH ) )
		{
			m_strMSG.Format("%s Get Begin Channel error ; ChIndex = %d ",pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;

		}
		nBeginCH = atoi(strNewExpress);
		if(  !m_pPubCObj->PC_GetExpressValues( pChnlInfo,  strNewExpress, pObj->m_strEndCH ) )
		{
			m_strMSG.Format("%s Get End Channel error ; ChIndex = %d ",pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;

		}
		nEndCH = atoi(strNewExpress);
		nAimIndex = TMS_FindFreeTrunkChnl( nBeginCH, nEndCH );
	}
	else
	{
		nAimIndex = TMS_FindFreeTrunk();
	}
	if( -1 >= nAimIndex || nAimIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s have not free channel ; ChIndex = %d ",pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	
	CFChannelInfo *pAimChnlInfo = &(m_pChannelInfo[nAimIndex]);
	TMS_MakeCall( nAimIndex, strCallTel, pAimChnlInfo->m_strLocalDN  );
	pAimChnlInfo->m_nLinkCh = nIndex;
	pAimChnlInfo->m_strDialTel		= strCallTel;
	pAimChnlInfo->m_nDialType		= 1;//1�Ⲧ
	m_pPubCObj->PC_ShowChannelState( nAimIndex, "Dial" );


	if( 0 == pObj->m_nWaitOption )//���Ż�����
	{
		TMS_StartPlaySignal(nIndex, TMS_RINGBACK_SIG);
	}
	else if( 2 == pObj->m_nWaitOption )//���������ļ�
	{
		
		char *pszFileName = (LPTSTR)(LPCTSTR)pObj->m_strPlayFile;
		TMS_PlayFile_Loop( nIndex , -1 ,pszFileName , 1, 1, false );
	}

	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_strDialTel		= strCallTel;
	pChnlInfo->m_nDialType		= 1;//1�Ⲧ
	pChnlInfo->m_nLinkCh = nAimIndex;
	pChnlInfo->m_nStep = 1;

	return;
}
//�ж��Ƿ�ʱ
void CTMS8E240::TMS_Switch_TimeOut(int nIndex, CSwitchObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nAimIndex	=  pChnlInfo->m_nLinkCh ;
	if( 0 > nAimIndex || nAimIndex >= m_nTotalChannel  ) return;
	long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
	if( n > pObj->m_nTimeOut )
	{
		//��ʱ��ʧ��
		m_pChannelInfo[nAimIndex].m_nLinkCh = -1;
		TMS_ResetChannel( nAimIndex );
		m_pChannelInfo[nIndex].m_nLinkCh = -1;
		TMS_StartPlaySignal(nIndex,TMS_BUSY_SIG);
		m_strMSG.Format("%s Switch Time out fail ; ChIndex = %d ",pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );

		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );

	}

	return;
}
/******************************************************
	�ж�nIndexͨ���Ƿ�����ͨ��switch,����ǣ�������Ӧ
	����
******************************************************/
void CTMS8E240::TMS_Switch_Judge(int nIndex, RMSG *pmsg)
{
	int nLinkIndex = m_pChannelInfo[nIndex].m_nLinkCh;
	if( -1 >= nLinkIndex || nLinkIndex >= m_nTotalChannel  ) return;
	CNodeObj * pObj = m_pChannelInfo[nLinkIndex].GetCurrentNodeObj();
	if( NULL == pObj ) return;
	if( NODETYPE_SWITCH != pObj->m_nType ) return;
	TMS_Switch_Result(  nLinkIndex,  pmsg , pObj );//���Switch���
	return;
}

void CTMS8E240::TMS_Switch_Result(int nIndex, RMSG *pmsg, CNodeObj * pNewObj )
{
	if( 0 == m_pChannelInfo[nIndex].m_nStep ) return;
	CSwitchObj *pObj = (CSwitchObj*)pNewObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CSwitchObj) ) )
	{
		m_strMSG.Format("Switch Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	int nAimIndex	= m_pChannelInfo[nIndex].m_nLinkCh ;
	switch( pmsg->MsgType )
	{
	case MSG_RELEASE:
		if( F_R_CALLOUTERR == pmsg->Function )//����ʧ��
		{
			switch(pmsg->Param)
			{
			case P_R_NODIALTONE  :     // 1   //û�м�⵽������ 
			case P_R_RINGTIMEOUT   :   // 3   //���峬ʱ �����˽ӣ�
			case P_R_NOANSWER     :     //2   //û����Ӧ
			case P_R_BUSYLOCAL    :    // 19  //�л�æ
			case P_R_BUSYDDD       :   // 20  //����æ
			case P_R_NULLDN         :   //22  //�պ�
			case P_R_NODIGITALPATH   : // 26  //û������ͨ·
			case P_R_LINEUSED       :  // 4  //��·�����Է�ռ�û�û�и�λ���
			case P_R_CONGESTION      :  //12  //�豸ӵ��
			case P_R_INCOMPLETE       : //15  //��ַ��ȫ
			case P_R_FAILURE           ://16  //���й���
			case P_R_CALLBARRING :      //18  //����ܾ�
			default://��������ʧ�ܷ�ʽ
				m_pChannelInfo[nAimIndex].m_nLinkCh = -1;
				TMS_ResetChannel( nAimIndex );
				m_pChannelInfo[nIndex].m_nLinkCh = -1;
				TMS_StartPlaySignal(nIndex,TMS_BUSY_SIG);
				m_strMSG.Format("%s Switch  fail, %s ; ChIndex = %d ",pObj->m_strCaption, GetReleaseParamNameStr(pmsg->Param) , nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
				{
					m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1,"");
				}
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				break;
			}

		}
		break;
	case MSG_CALLEDACK:
		//���ӳɹ�
		TMS_MySetLink( nIndex, nAimIndex, CF_CHSWITCH_CONNECT );
		if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
		{
			m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 0, "");
		}
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		break;
	case UPMSG_CALLOUTCHANNEL:
		if( F_CC_NOCHANNEL == pmsg->Function )
		{
			//���������䲻������ͨ����������ֹ��ͨ���ͷ�
			m_pChannelInfo[nAimIndex].m_nLinkCh = -1;
			TMS_ResetChannel( nAimIndex );
			m_pChannelInfo[nIndex].m_nLinkCh = -1;
			TMS_StartPlaySignal(nIndex,TMS_BUSY_SIG);
			m_strMSG.Format("%s Switch  fail,%s; ChIndex = %d ",pObj->m_strCaption, GetCallOutChannelParamNameStr(pmsg->Param), nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			if( CF_CHTYPE_USER == m_pChannelInfo[nIndex].m_nChanelType )
			{
				m_pPubCObj->PC_CTI_MakeCallResponseMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "");
			}
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
	default:
		break;
	}

	return;
}

void CTMS8E240::TMS_MyClearLink(int nIndex1, int nIndex2)
{
	TMS_Router( nIndex1,  nIndex1, F_R_TALK );
	TMS_Router( nIndex2,  nIndex2, F_R_TALK );
	return;
}
void CTMS8E240::TMS_MySetLink(int nIndex1, int nIndex2, int nSwitchType)
{
	
	if( CF_CHSWITCH_CONNECT == nSwitchType )
	{
		TMS_Router( nIndex1,  nIndex2, F_R_TALK );
	}
	else if( CF_CHSWITCH_MONITOR == nSwitchType )
	{
		TMS_Router( nIndex1,  nIndex2, F_R_MONITOR );
	}
	else 
	{
		//�Է�������
		TMS_Router( nIndex2,  nIndex1, F_R_MONITOR );
	}

	m_pChannelInfo[nIndex1].m_nLinkType = nSwitchType;
	m_pChannelInfo[nIndex2].m_nLinkType = nSwitchType;
	m_pPubCObj->PC_ShowConnectToChnl(nIndex1,nIndex2);
	m_pPubCObj->PC_ShowConnectToChnl(nIndex2,nIndex1);

	return ;
}
/***************************************************************
nSwitchType =
  F_R_SILENCE           0   //ͨ���������
  F_R_MONITOR           1   //�������
  F_R_TALK              2   //˫��ͨ��
  F_R_TALK_RP		   3   //˫��ͨ����ͨ���пɻ��¼������	
  F_R_MONITOR_P		   4   //ǿ�����ӵ����ڷ�����Դ		   
  F_R_TRANSTO_CT        13  //ֱ�ӷ��͵�CT����һ��ʱ϶ 
  F_R_MONITOR_CT        14  //ֱ�Ӽ���CT����һ��ʱ϶ 
***************************************************************/
void CTMS8E240::TMS_Router(int nIndex1, int nIndex2, int nSwitchType)
{
	if( -1 >= nIndex1 || nIndex1 >= m_nTotalChannel  ) return;
	if( -1 >= nIndex2 || nIndex2 >= m_nTotalChannel  ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType = DOWNMSG_ROUTER;
	msg.Function = nSwitchType;
	msg.ChType  = TMS_GetDevChnlType( m_pChannelInfo[nIndex1].m_nChanelType );
	msg.ChIndex = m_pChannelInfo[nIndex1].m_nChannel;
	msg.LinkChType  = TMS_GetDevChnlType( m_pChannelInfo[nIndex2].m_nChanelType );
	msg.LinkChIndex = m_pChannelInfo[nIndex2].m_nChannel;
	msg.DataLen = 0;
	msg.DataSpec = 0;
	PCISENDMSG(&msg);
	return;
}

int CTMS8E240::TMS_FindFreeChannel(int nChnlType)
{
	static int nCurrentTrunkNo = 0 ;
	int i = 0;
	for( i = nCurrentTrunkNo ; i < m_nTotalChannel ; i++ ) 
	{
		if( nChnlType == m_pChannelInfo[i].m_nChanelType && CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
		{

			nCurrentTrunkNo = i+1 ;
			return i;

		}
	}
	
	for( i = 0 ; i < nCurrentTrunkNo ; i++ )
	{
		if( nChnlType == m_pChannelInfo[i].m_nChanelType && CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
		{

			nCurrentTrunkNo = i+1 ;
			return i;

		}
	}

	return -1 ;
}
/**********************************************************
  TMS_DIAL_SIG     1 //������
  TMS_RINGBACK_SIG 2 //������
  TMS_BUSY_SIG     3 //æ��
**********************************************************/
void CTMS8E240::TMS_StartPlaySignal(RMSG *pmsg, int nSigType)
{
	pmsg->InitDataLen(0);
	pmsg->MsgType=MSG_MEDIA;
	pmsg->Function=F_MEDIA_Tx_CPSign;
	pmsg->LinkChType=RESTYPE_VOCPCM;
	if( TMS_DIAL_SIG == nSigType  )
	{
		//������
		pmsg->Param		=  RESPAR_PFIRSTMSG | RESPAR_PLASTMSG | RESPAR_PLOOPMSG | RESPAR_DTMFSTOP;
	}
	else 
	{
		pmsg->Param		=  RESPAR_PFIRSTMSG | RESPAR_PLASTMSG | RESPAR_PLOOPMSG;
	}
	pmsg->LinkChIndex=0;
	pmsg->AddByte( nSigType );
    PCISENDMSG(pmsg);
  return;
}

/**********************************************************
  TMS_DIAL_SIG     1 //������
  TMS_RINGBACK_SIG 2 //������
  TMS_BUSY_SIG     3 //æ��
**********************************************************/
void CTMS8E240::TMS_StartPlaySignal(int nIndex, int nSigType)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.ChIndex		= pChnlInfo->m_nChannel;
	msg.LinkChType	= RESTYPE_VOCPCM;
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.Function	= F_MEDIA_Tx_CPSign;
	if( TMS_DIAL_SIG == nSigType  )
	{
		//������
		msg.Param		=  RESPAR_PFIRSTMSG | RESPAR_PLASTMSG | RESPAR_PLOOPMSG | RESPAR_DTMFSTOP;
	}
	else 
	{
		msg.Param		=  RESPAR_PFIRSTMSG | RESPAR_PLASTMSG | RESPAR_PLOOPMSG;
	}
	msg.LinkChIndex=0;
	msg.AddByte( nSigType );
	PCISENDMSG(&msg);
}

void CTMS8E240::TMS_StopPlay(RMSG *pmsg)
{
  pmsg->InitDataLen(0);
  pmsg->MsgType=MSG_MEDIA;
  pmsg->Function=F_MEDIA_Tx_Release;
  pmsg->Param=0;
  pmsg->LinkChType=RESTYPE_VOCPCM;
  pmsg->LinkChIndex=0;
  PCISENDMSG(pmsg);
  return;
}
/*************************************************************
nIndex �߼�ͨ����
nGroup �߼�����ţ�nGroup >= 0ʱ���������nGroup < 0ʱ��ͨ������
**************************************************************/
void CTMS8E240::TMS_StopPlay(int nIndex,int nGroup )
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	if( nGroup >= 0 ){
		
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
	}
	else{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
	}
	msg.Function=F_MEDIA_Tx_Release;
	msg.Param=0;
	msg.LinkChType=RESTYPE_VOCPCM;
	msg.LinkChIndex=0;
	PCISENDMSG(&msg);
	return;
}

void CTMS8E240::TMS_Switch_Self(int nIndex, CSwitchObj *pObj)
{
	if( nIndex < 0 || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nAimIndex = m_pChannelInfo[nIndex].m_nLinkCh;
	if( nIndex < 0 || nIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s have not the Channel that link to self . index is  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	int nSwitchType = pObj->m_nSwitchType;
	switch( nSwitchType )
	{
		case CF_CHSWITCH_CONNECT://ֱ������
		case CF_CHSWITCH_MONITOR://����
		case CF_CHSWITCH_SPEAK://������������������������
			TMS_MySetLink(nIndex, nAimIndex, pObj->m_nSwitchType  );
			break;
		case CF_CHSWITCH_DISCONNECT://�Ͽ�����	
			TMS_MyClearLink(nIndex, nAimIndex );
			pChnlInfo->m_nLinkType = -1;
			m_pChannelInfo[nAimIndex].m_nLinkType = -1;
			m_pPubCObj->PC_ShowConnectToChnl(nIndex,-1);
			m_pPubCObj->PC_ShowConnectToChnl(nAimIndex,-1);
			break;
		default:
			break;
	}
	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_nStep = 1;
	return;
}

void CTMS8E240::TMS_Switch_Aim(int nIndex, CSwitchObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nVarKey = pObj->GetVarKey( pObj->m_strAimChnl );
	if(  nVarKey < 0 )
	{
		m_strMSG = pObj->m_strAimChnl;
	}
	else if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, m_strMSG, nVarKey ) )
	{	
		m_strMSG.Format("%s Channel index is  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	int nAimIndex = atoi(m_strMSG);
	if( -1 >= nAimIndex || nAimIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s Channel index is  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	int nSwitchType = pObj->m_nSwitchType;
	switch( nSwitchType )
	{
		case CF_CHSWITCH_CONNECT://ֱ������
		case CF_CHSWITCH_MONITOR://����
		case CF_CHSWITCH_SPEAK://������������������������
			TMS_MySetLink(nIndex, nAimIndex, pObj->m_nSwitchType  );
			pChnlInfo->m_nLinkCh = nAimIndex;
			if( CF_CHSWITCH_MONITOR != pObj->m_nSwitchType)
			{
				m_pChannelInfo[nAimIndex].m_nLinkCh = nIndex;
				pChnlInfo->m_nState = CHANNEL_STATE_CONNECT;
			}
			else
			{
				pChnlInfo->m_nState = CHANNEL_STATE_MONITORING;
			}
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
			return;
		case CF_CHSWITCH_DISCONNECT://�Ͽ�����	
			TMS_MyClearLink(nIndex, nAimIndex ) ;
			pChnlInfo->m_nLinkType = -1;
			m_pChannelInfo[nAimIndex].m_nLinkType = -1;
			m_pPubCObj->PC_ShowConnectToChnl(nIndex,-1);
			m_pPubCObj->PC_ShowConnectToChnl(nAimIndex,-1);
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
			return;
		default:
			break;
	}
	return;	
}

void CTMS8E240::TMS_Transfer(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( CF_CHTYPE_USER != pChnlInfo->m_nChanelType ) return;
	CTransferCheckObj *pObj = (CTransferCheckObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CTransferCheckObj) ) )
	{
		m_strMSG.Format("TransferStart Error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	switch( pObj->m_nTransferType )
	{
	case 0://��ʼת��
		TMS_Transfer_Start( nIndex ,pObj );
		break;
	case 1://ȡ��ת��
		TMS_Transfer_Cancel( nIndex ,pObj );
		break;
	case 2://���ת��
		{
				switch( pObj->m_nTransferAim )
				{
				case 0:
					if(  0 == pChnlInfo->m_nStep )
						TMS_Transfer_ToAgent( nIndex ,pObj );
					else
					    TMS_Transfer_CancelCheck( nIndex ,pObj );
					break;
				case 1:
					if(  0 == pChnlInfo->m_nStep )
						TMS_Transfer_ToCustomer( nIndex  ,pObj );
					else
						TMS_Transfer_CancelCheck( nIndex ,pObj );
					break;
				case 2:
					break;
				default:
					break;
				}
				/******************************************************************/
				//TMS_TransferKey( nIndex );//��������
				/******************************************************************/
		}
		break;
	default:
		break;
	}

	return;
}

void CTMS8E240::TMS_Transfer_Start(int nIndex, CTransferCheckObj *pObj)
{
	if( CF_CHTYPE_USER != m_pChannelInfo[nIndex].m_nChanelType ) return;
	int nLinkIndex =  m_pChannelInfo[nIndex].m_nLinkCh;
	int nDN = atoi( m_pChannelInfo[nIndex].m_strLocalDN );
	if( nLinkIndex <= -1 )
	{
		m_strMSG.Format("%s Have not active call ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		CTI_TransferMsg( nDN, 1, "" );//ִ�д���
		return;
	}
	if( pObj->m_bClearDTMF )
	{
		m_pChannelInfo[nIndex].m_strDTMFRecived = "";
	}
	TMS_MyClearLink(nIndex,nLinkIndex );
	if( 1 == pObj->m_nWaitOption )
	{
		//ѭ������
		char *pszFileName = (LPTSTR)(LPCTSTR)pObj->m_strPlayFile;
		TMS_PlayFile_Loop( nLinkIndex, -1, pszFileName, 1, 1, false );
		m_pChannelInfo[nLinkIndex].m_strPlayFile = pObj->m_strPlayFile;
	}

	m_pChannelInfo[nLinkIndex].m_nState = CHANNEL_STATE_TRANSFER;
	m_pPubCObj->PC_ShowChannelState( nLinkIndex, "Busy" );
	TMS_StartPlaySignal( nIndex, TMS_DIAL_SIG );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);


	return;
}

void CTMS8E240::TMS_Transfer_Cancel(int nIndex, CTransferCheckObj *pObj)
{
	
	int nLinkIndex =  m_pChannelInfo[nIndex].m_nLinkCh;
	if( nLinkIndex < 0 || nLinkIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s Have not active call ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		CTI_CancelTransferMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 1, "" );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	if( pObj->m_bClearDTMF )
	{
		m_pChannelInfo[nIndex].m_strDTMFRecived = "";
	}
	m_pChannelInfo[nLinkIndex].m_nState = CHANNEL_STATE_CONNECT;
	m_pPubCObj->PC_ShowChannelState( nLinkIndex, "Busy");
	TMS_MySetLink( nIndex, nLinkIndex , CF_CHSWITCH_CONNECT );
	CTI_CancelTransferMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 0, "" );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
	return;
}

void CTMS8E240::TMS_Transfer_ToCustomer(int nIndex, CTransferCheckObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( pChnlInfo->m_nStep > 0 ) return;
	if( CF_CHTYPE_USER != pChnlInfo->m_nChanelType ) return;
	int nDN = atoi( pChnlInfo->m_strLocalDN );
	int nLinkIndex =  pChnlInfo->m_nLinkCh;
	if( nLinkIndex < 0 || nLinkIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s Get telnumber Error ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		
		return;
	}
	if( pObj->m_bClearDTMF )
	{
		pChnlInfo->m_strDTMFRecived = "";
	}

	CString strPhone	= pObj->m_strExpression;//����ת�Ƶ�����ʱ��Ŀ��������ӱ��ʽ��ȡ
	int nKey = pObj->GetVarKey(strPhone);
	if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo,  strPhone, nKey ) )
	{
		m_strMSG.Format("%s Get telnumber Error ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		
		return;
	}
	
	//��ȡ�õ绰����
	int nAimIndex = TMS_FindFreeTrunk();
	if( -1 >= nAimIndex || nAimIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s have not free channel ; ChIndex = %d ",pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	TMS_MakeCall( nAimIndex, (LPSTR)(LPCSTR)strPhone, pChnlInfo->m_strLocalDN );
	pChnlInfo->m_nTransferCh	= nAimIndex;
	m_pChannelInfo[nAimIndex].m_nLinkCh = nIndex;
	TMS_StartPlaySignal(nIndex, TMS_RINGBACK_SIG);
	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_nStep = 1;

	return;
}

void CTMS8E240::TMS_Transfer_ToAgent(int nIndex, CTransferCheckObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( pChnlInfo->m_nStep > 0 ) return;
	if( CF_CHTYPE_USER != pChnlInfo->m_nChanelType ) return;
	int nDN = atoi( pChnlInfo->m_strLocalDN );
	int nLinkIndex =  pChnlInfo->m_nLinkCh;
	if( nLinkIndex < 0 || nLinkIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s Get telnumber Error ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		
		return;
	}
	if( pObj->m_bClearDTMF )
	{
		pChnlInfo->m_strDTMFRecived = "";
	}
	//ת�Ƶ�����ͨ��
	CString strPhone	= pObj->m_strExpression;
	int nKey = pObj->GetVarKey(strPhone);
	if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo,  strPhone, nKey ) )
	{
		m_strMSG.Format("%s Get telnumber Error ;ChIndex = %d", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	//��ȡ�õ绰����
	int nAimIndex = TMS_FindFreeChannel( strPhone, CF_CHTYPE_USER  );//ͨ���ֻ�������Ҹ÷ֻ�
	if( -1 >= nAimIndex || nAimIndex >= m_nTotalChannel  )
	{
		//�����ͨ���������ߣ���Ϊ�Ǵ����
		m_strMSG.Format("%s Channel index is error; ChIndex = %d ",  pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	CFChannelInfo *pAimChnlInfo = &(m_pChannelInfo[nAimIndex]);
	//���зֻ����У���������
	pChnlInfo->m_nTransferCh	= nAimIndex;
	pAimChnlInfo->m_nLinkCh		= nIndex;
	TMS_MakeCall( nAimIndex, pChnlInfo->m_strCalledID, pChnlInfo->m_strCallerID );
	TMS_StartPlaySignal(nIndex, TMS_RINGBACK_SIG);
	m_pPubCObj->PC_ShowChannelState( nAimIndex, "Ringing");
	m_pPubCObj->PC_StartTimer(pChnlInfo);
	pChnlInfo->m_nStep = 1;


	return;
}
/*************************************************************
����ѭ���ļ�������Ϣ����������ϢΪѭ��������Ϣ��ϵͳ���ظ�����
��������ֱ���û�ֹͣ������������ͨ���ķ���
����isfirst��islast����������
	first = 1,last = 1����������ֻ��һ���ļ�
	first = 1,last = 0��һ���ļ�
	first = 0,last = 1���һ���ļ�
	first = 0,last = 0�м���ļ�
nIndex �߼�ͨ����
nGroup �߼�����ţ�-1ͨ��������>=1��ʾ����ǿ������������
bDTMFStop �Ƿ񰴼����
**************************************************************/
void CTMS8E240::TMS_PlayFile_Loop(int nIndex, int nGroup, char *file, unsigned short isfirst, unsigned short islast, unsigned short bDTMFStop)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	if( nGroup >= m_nTotalConf  ) return;
	if( file == NULL ) return;
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Tx_File;
	if( nGroup >= 0 ){
		
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
	}
	else{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
	}
	msg.Param		= RESPAR_PLOOPMSG | (isfirst?RESPAR_PFIRSTMSG:0) | (bDTMFStop?RESPAR_DTMFSTOP:0) | (islast?RESPAR_PLASTMSG:0);
	//���÷����������Ƿ��ǵ�һ���ļ����Ƿ������һ���ļ����Ƿ񰴼���ϵ�
	msg.LinkChType	= RESTYPE_VOCPCM;
	msg.LinkChIndex	= 0;
	msg.DataSpec	= PFDATA_ALAW;//�ļ���ʽ
	msg.AddStr(file); //���127���ֻ�254�ַ�

	PCISENDMSG(&msg);
}
void CTMS8E240::TMS_Transfer_Judge(int nIndex, RMSG *pmsg)
{
	int nLinkIndex = m_pChannelInfo[nIndex].m_nLinkCh;
	if( -1 >= nLinkIndex || nLinkIndex >= m_nTotalChannel  ) return;
	CNodeObj * pObj = m_pChannelInfo[nLinkIndex].GetCurrentNodeObj();
	if( NULL == pObj ) return;
	if( NODETYPE_CHECK_TRANSFER != pObj->m_nType ) return;
	TMS_Transfer_Result(  nLinkIndex,  pmsg , pObj );//���Switch���
	return;
}
void CTMS8E240::TMS_Transfer_Result(int nIndex, RMSG *pmsg, CNodeObj *pNewObj)
{
	if( 0 == m_pChannelInfo[nIndex].m_nStep ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CTransferCheckObj *pObj = (CTransferCheckObj*)pNewObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CTransferCheckObj) ) )
	{
		m_strMSG.Format("TransferStart Error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	int nAimIndex	= pChnlInfo->m_nTransferCh ;
	int nLinkIndex	= pChnlInfo->m_nLinkCh ;
	int nDN = atoi( pChnlInfo->m_strLocalDN );
	switch( pmsg->MsgType )
	{
	case MSG_RELEASE:
		if( F_R_CALLOUTERR == pmsg->Function )//����ʧ��
		{
			switch(pmsg->Param)
			{
			case P_R_NODIALTONE  :     // 1   //û�м�⵽������ 
			case P_R_RINGTIMEOUT   :   // 3   //���峬ʱ �����˽ӣ�
			case P_R_NOANSWER     :     //2   //û����Ӧ
			case P_R_BUSYLOCAL    :    // 19  //�л�æ
			case P_R_BUSYDDD       :   // 20  //����æ
			case P_R_NULLDN         :   //22  //�պ�
			case P_R_NODIGITALPATH   : // 26  //û������ͨ·
			case P_R_LINEUSED       :  // 4  //��·�����Է�ռ�û�û�и�λ���
			case P_R_CONGESTION      :  //12  //�豸ӵ��
			case P_R_INCOMPLETE       : //15  //��ַ��ȫ
			case P_R_FAILURE           ://16  //���й���
			case P_R_CALLBARRING :      //18  //����ܾ�
			default://��������ʧ�ܷ�ʽ
				m_pChannelInfo[nAimIndex].m_nLinkCh	= -1;
				TMS_ResetChannel(nAimIndex);
				pChnlInfo->m_nTransferCh		= -1;

				m_strMSG.Format("%s Call out fail ; ChIndex = %d ",pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return;	
			}

		}
		break;
	case MSG_CALLEDACK:
		//���ӳɹ�
		if( 1 == pObj->m_nWaitOption )
			TMS_StopPlay( nLinkIndex,-1 );
		TMS_MySetLink( nLinkIndex, nAimIndex , CF_CHSWITCH_CONNECT );

		m_pChannelInfo[nAimIndex].m_nTransferCh		= -1;
		m_pChannelInfo[nAimIndex].m_nLinkCh			= nLinkIndex;

		m_pChannelInfo[nLinkIndex].m_nState = CHANNEL_STATE_CONNECT;
		m_pChannelInfo[nLinkIndex].m_nTransferCh	= -1;
		m_pChannelInfo[nLinkIndex].m_nLinkCh		= nAimIndex;

		pChnlInfo->m_nTransferCh		= -1;
		pChnlInfo->m_nLinkCh			= -1;
		TMS_StartPlaySignal( nIndex, TMS_BUSY_SIG );
		m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 0, "" );//ִ�д���
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		break;
	case UPMSG_CALLOUTCHANNEL:
		if( F_CC_NOCHANNEL == pmsg->Function )
		{
			//���������䲻������ͨ����������ֹ��ͨ���ͷ�
			m_pChannelInfo[nAimIndex].m_nLinkCh	= -1;
			TMS_ResetChannel(nAimIndex);
			pChnlInfo->m_nTransferCh		= -1;

			m_strMSG.Format("%s Call out fail ; ChIndex = %d ",pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			m_pPubCObj->PC_CTI_TransferResponseMsg( nDN, 1, "" );//ִ�д���
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;	
		}
	default:
		break;
	}

	return;
}

void CTMS8E240::TMS_Transfer_CancelCheck(int nIndex, CTransferCheckObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nAimIndex	=  pChnlInfo->m_nTransferCh ;
	if( 0 > nAimIndex || nAimIndex >= m_nTotalChannel  ) return;
	int nDN = atoi( pChnlInfo->m_strLocalDN );
	BOOL bCancel = FALSE;
	if( EVENTTYPE_AGENT_CANCEL_TRANSFER == pChnlInfo->m_nEventType )
		bCancel = TRUE;
	else
	{
		//���Ƿ��յ�ȡ��ת�Ƶİ���
		if( "" != pChnlInfo->m_strDTMFRecived  )
		{
			m_pPubCObj->PC_ShowChannelDTMF( nIndex, pChnlInfo->m_strDTMFRecived );
			if(  pChnlInfo->m_strDTMFRecived.Right(1) == pObj->m_strCancelDTMF  )
			{
				bCancel = TRUE;
			}
		}
	}
	if( bCancel )
	{
		m_pChannelInfo[nAimIndex].m_nLinkCh	= -1;
		TMS_ResetChannel(nAimIndex);
		pChnlInfo->m_nTransferCh = -1;
		CTI_CancelTransferMsg( atoi(m_pChannelInfo[nIndex].m_strLocalDN), 0, "" );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_CANCEL_TRANSFER );//ȡ��ת��
	}

	/////////////////////////////
	long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
	if( n > 30 )
	{
		//��ʱ��ʧ��
		m_pChannelInfo[nAimIndex].m_nLinkCh	= -1;
		TMS_ResetChannel(nAimIndex);
		pChnlInfo->m_nTransferCh = -1;

		m_strMSG.Format("%s Transfer Call out fail ; ChIndex = %d ",pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		CTI_CancelTransferMsg( nDN, 1, "" );//ִ�д���
		return;
	}

	return;
}

void CTMS8E240::TMS_DTMFSend(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( pChnlInfo->m_nStep > 0 ) return;
	CDTMFSendObj *pObj = (CDTMFSendObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDTMFSendObj) ) )
	{
		m_strMSG.Format("DTMF Send Error, ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;
	}
	CString strNewExpress("");
	if(  !m_pPubCObj->PC_GetExpressValues( pChnlInfo,  strNewExpress, pObj->m_strDTMF ) )
	{
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		m_strMSG.Format("%s DTMF Send Express  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		return;
	}

	char *pszDTMF = (LPTSTR)(LPCTSTR)strNewExpress;
	TMS_DTMFSend_MSG( nIndex, pszDTMF );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );

	return;
}


inline void CTMS8E240::TMS_DTMFSend_MSG(int nIndex, char *szDTMF )
{
	if( NULL == szDTMF ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType		= MSG_MEDIA;
	msg.Function	= F_MEDIA_Tx_DTMF;
	msg.Param		= RESPAR_PFIRSTMSG |RESPAR_PLASTMSG ;
	msg.ChIndex		= pChnlInfo->m_nChannel;
	msg.ChType		= TMS_GetDevChnlType( pChnlInfo->m_nChanelType );
	msg.LinkChType	= RESTYPE_VOCPCM;
	msg.AddStr( szDTMF );
	PCISENDMSG( &msg );

	return;
}

void CTMS8E240::TMS_CallHold(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( CF_CHTYPE_USER != pChnlInfo->m_nChanelType ) 
	{
		m_strMSG.Format("CallHold error : CallHold are only used by the User channel,  ChIndex = %d ", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	CCallHoldObj *pObj = (CCallHoldObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CCallHoldObj) ) )
	{
		m_strMSG.Format("CallHold error : ChIndex = %d   Error = Object is not valid", nIndex );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;  
	}
	
	int nDN = atoi( m_pChannelInfo[nIndex].m_strLocalDN );
	//PickUp Hold
	if( 1 == pObj->m_nCallHold ) 
	{
		TMS_CallHold_PickUp( nIndex, pObj );//�ж��Ƿ��ǻָ�ͨ��
		pChnlInfo->m_nHoldChnl = -1;
		return;
	}

	//�����Hold,�����������
	int nIndexLink = pChnlInfo->m_nLinkCh;
	if( 0 > nIndexLink || nIndexLink >= m_nTotalChannel  )
	{
		m_strMSG.Format("%s CallHold error, have not active call; ChIndex = %d ", pObj->m_strCaption, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		m_pPubCObj->PC_CTI_HoldResponseMsg( nDN, 1 );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;	
	}

	pChnlInfo->m_strDTMFRecived = "";
	TMS_MyClearLink( nIndex, nIndexLink) ;
	if( pObj->m_bPlayVox ) //���Ƿ񲥷�����
	{
		
		char *pszFileName = (LPTSTR)(LPCTSTR)pObj->m_strPlayFile;
		TMS_PlayFile_Loop( nIndexLink, -1, pszFileName, 1, 1, false );
		m_pChannelInfo[nIndexLink].m_strPlayFile = pObj->m_strPlayFile;
	}
	pChnlInfo->m_nHoldChnl = nIndexLink;
	pChnlInfo->m_nLinkCh = -1;
	m_pChannelInfo[nIndexLink].m_nState = CHANNEL_STATE_HOLD;
	m_pPubCObj->PC_ShowChannelState( nIndexLink, "Hold" );
	m_pPubCObj->PC_CTI_HoldResponseMsg( nDN, 0);
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);

	return;
}


void CTMS8E240::TMS_CallHold_PickUp(int nIndex, CCallHoldObj *pObj)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nLinkIndex = TMS_FindHoldChannel(nIndex );

	int nDN = atoi( pChnlInfo->m_strLocalDN );
	if( 0 > nLinkIndex || nLinkIndex >= m_nTotalChannel  )
	{
		m_strMSG.Format("PickUp Hold error,have not PickUp Hold active Call  : ChIndex = %d", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		m_pPubCObj->PC_CTI_PickupHoldResponseMsg( nDN , 1,"","" );
		return;
	}
	m_pChannelInfo[nLinkIndex].m_nState = CHANNEL_STATE_CONNECT;
	m_pPubCObj->PC_ShowChannelState( nLinkIndex, "Busy");
	TMS_MySetLink( nIndex, nLinkIndex , CF_CHSWITCH_CONNECT );
	pChnlInfo->m_nLinkCh = nLinkIndex;
	m_pPubCObj->PC_CTI_PickupHoldResponseMsg( nDN , 0,m_pChannelInfo[nLinkIndex].m_strCallerID,m_pChannelInfo[nLinkIndex].m_strCalledID );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );

	return;
}

int CTMS8E240::GetHoldChnl(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nLinkIndex = -1;
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		if( nIndex == pChnlInfo->m_nLinkCh )
		{
			nLinkIndex = i;
			break;
		}
	}
	return nLinkIndex;
}

void CTMS8E240::TMS_ReleaseCall(int nIndex)
{
	if( nIndex == -1 ) return ;
	int nLinkIndex = m_pChannelInfo[nIndex].m_nLinkCh;
	if( -1 < nLinkIndex && nLinkIndex < m_nTotalChannel  )
	{
		if( CHANNEL_STATE_HOLD != m_pChannelInfo[nIndex].m_nState && CHANNEL_STATE_MONITORING !=  m_pChannelInfo[nIndex].m_nState )
		{
			if( CF_CHTYPE_RECORD != m_pChannelInfo[nLinkIndex].m_nChanelType )
			{
				TMS_Release(nLinkIndex);
			}

			TMS_StartPlaySignal( nLinkIndex, TMS_BUSY_SIG );
			m_pChannelInfo[nLinkIndex].MoveCallEndBlock_Node();
			m_pPubCObj->PC_ShowChannelState( nLinkIndex, "Free" );
			
		}

	}

	int nTransferIndex = m_pChannelInfo[nIndex].m_nTransferCh;
	if( -1 < nTransferIndex && nTransferIndex < m_nTotalChannel  )
	{
		if( CF_CHTYPE_RECORD != m_pChannelInfo[nTransferIndex].m_nChanelType )
		{
			TMS_Release(nTransferIndex);
		}

		TMS_StartPlaySignal( nTransferIndex, TMS_BUSY_SIG );
		m_pChannelInfo[nTransferIndex].MoveCallEndBlock_Node();
		m_pPubCObj->PC_ShowChannelState( nTransferIndex, "Free" );
		m_pPubCObj->PC_ShowConnectToChnl( nTransferIndex, -1 );
		

	}

	TMS_ConF_Leave( nIndex, m_pChannelInfo[nIndex].m_nConfNO );
	TMS_Release(nIndex);

	m_pPubCObj->PC_ShowChannelState( nIndex, "Free");
	m_pPubCObj->PC_ShowConnectToChnl( nIndex, -1 );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );

	return;
}

void CTMS8E240::TMS_StartRecord(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CRecordObj *pObj = (CRecordObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CRecordObj) ) )
	{
		m_strMSG.Format("Record Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	if( 0 == pObj->m_nRecordType )
		TMS_StartRecord_Normal( nIndex,  pObj );
	else
		TMS_StartRecord_Confer( nIndex,  pObj );


	return;
}

void CTMS8E240::TMS_AnswerCall(int nIndex)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	pChnlObj->m_nCallType			= CHANNEL_CALLTYPE_COMEIN;
	pChnlObj->m_nRings				= 1;
	pChnlObj->m_nStep				= 1;
	pChnlObj->m_nState				= CHANNEL_STATE_CONNECT;
	pChnlObj->m_nEventType			=  EVENTTYPE_OFFHOOK;
	pChnlObj->m_strtBeginTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	

	TMS_Ack( nIndex,TRUE );
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
	return;
}

/*************************************************
//  �������������� 
//  
//  
*************************************************/
void CTMS8E240::TMS_ConferenceResult(int nIndex, unsigned short Function ,unsigned short Param )
{
	CNodeObj * pNodeObj = m_pChannelInfo[nIndex].GetCurrentNodeObj();
	if( NULL == pNodeObj ) return;
	switch( pNodeObj->m_nType )
	{
	case NODETYPE_ENTER_CONFERENCE:
	case NODETYPE_LEAVE_CONFERENCE:
	case NODETYPE_CONFERENCE_BUILD:
	case NODETYPE_CONFERENCE_DELETE:
		{
			if( 0 == Param	)
			{
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
				return;
			}
			else//ʧ��
			{
				m_strMSG.Format("%s-%s-%s ChIndex = %d",  pNodeObj->m_strCaption, GetConfFuncNameStr(Function),GetConfResultStr( (char)Param ), nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return;
			}
		}
		break;
	default:
		break;
	}

	return;
}

///////////////////////////////////////////////////
//��ͨ�����������������
//ͬʱ���ɹ������Ļ����ű�����nConferNO��
//
//////////////////////////////////////////////////
void CTMS8E240::TMS_ConferenceBuild(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CConferenceBulidObj *pObj = (CConferenceBulidObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CConferenceBulidObj) ) )
	{
		m_strMSG.Format("ConferenceBuild error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;  
	}

	if( pChnlInfo->m_nStep > 0 )
	{

		long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
		if( n > 30 )
		{
			//��ʱ��ʧ��
			m_strMSG.Format("%s:ConferenceBuild Timeout Error ChIndex = %d",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			
		}
		return;
	}

	int nConferNO = -1;
	int nMaxMember = 0;
	if( 0 == pObj->m_nBuildType )//��ϵͳ�Զ����ҿ��л�����
	{
		nConferNO = TMS_FindFreeConferIndex();
	}
	else
	{
		//�ӱ���ȡ�û�����
		int nKey = pObj->GetVarKey( pObj->m_strConferNo );
		CString strConferNO("");
		if( -1 == nKey && "" != pObj->m_strConferNo )
		{
			strConferNO = pObj->m_strConferNo;
		}
		else
		{
			if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strConferNO, nKey ) )
			{
				m_strMSG.Format("ConferenceBuild error : ChIndex = %d   Error = have not free conference resource", nIndex );
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return;
			}
		}
		nConferNO = atoi(strConferNO);
	}

	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	{
		m_strMSG.Format("ConferenceBuild error : ChIndex = %d   Error = Input conference  error ", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	int nKey = pObj->GetVarKey( pObj->m_strMaxMember );
	CString strMaxMember("");
	if( -1 == nKey )
	{
		strMaxMember =  pObj->m_strMaxMember;
	}
	else
	{
		if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo,strMaxMember, nKey ) )
		{
			m_strMSG.Format("ConferenceBuild error : ChIndex = %d   Error = have not free conference resource", nIndex );
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
	}
	nMaxMember = atoi(strMaxMember);
	TMS_ConF_Create( nIndex, nConferNO,nMaxMember ) ;
	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_nStep = 1;

	return;
}

int CTMS8E240::TMS_FindFreeConferIndex()
{
	int nIndex = -1;
	int m = 0;
	for( int i = 0; i < m_nTotalConf; i++ )
	{
		m = m_pConference[i].nMemberNum + m_pConference[i].nResNum + m_pConference[i].nListenNum;
		if( m <= 0 )
			return i;
	}
	return nIndex;
}


void CTMS8E240::TMS_ConferenceEnter(int nIndex )
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CEnterConferObj *pObj = (CEnterConferObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CEnterConferObj) ) )
	{
		m_strMSG.Format("EnterConference error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;  
	}

	if( pChnlInfo->m_nStep > 0 )
	{
		long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
		if( n > 30 )
		{
			//��ʱ��ʧ��
			m_strMSG.Format("%s:EnterConference Timeout Error ChIndex = %d",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
		return;
	}
	
	int nConferNO = -1;
	CString strConfNO = pObj->m_strConferNO;
	int nKey = pObj->GetVarKey( strConfNO  );
	if( nKey < 0 )
	{
		nConferNO = atoi(strConfNO );
	}
	else
	{
		if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strConfNO, nKey ) )
		{
			m_strMSG.Format("%s Input Conference index Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
		nConferNO = atoi( strConfNO );
	}
	
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	{
		m_strMSG.Format("%s: ChIndex = %d    Group error ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	int nChnl = -1;
	CString strChnl = pObj->m_strChnlIndex;
	int nKey1 = pObj->GetVarKey( strChnl );
	if( nKey1 < 0 )
	{
		nChnl = atoi( strChnl);
	}
	else
	{
		if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strChnl, nKey1 ) )
		{
			m_strMSG.Format("%s Input Channel index Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
		nChnl = atoi( strChnl );
	}

	if( -1 == nChnl )
	{
		nChnl = nIndex;//��� nChnl = -1��ʾ�Լ����иýڵ��ͨ������
	}
	if( nChnl < 0  || nChnl >= m_nTotalChannel  ) 
	{
		m_strMSG.Format("%s Enter Conference  Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	TMS_ConF_Enter( nChnl, nConferNO, pObj->m_nAction );
	pChnlInfo->m_nStep = 1;
	m_pPubCObj->PC_StartTimer( pChnlInfo );

	return;
}

void CTMS8E240::TMS_ConferenceLeave(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CLeaveConferObj *pObj = (CLeaveConferObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CLeaveConferObj) ) )
	{
		m_strMSG.Format(" LeaveConfer  error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;  
	}
	
	if( pChnlInfo->m_nStep > 0 )
	{

		long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
		if( n > 30 )
		{
			//��ʱ��ʧ��
			m_strMSG.Format("%s:LeaveConfer Timeout Error ChIndex = %d",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			
		}
		return;
	}
	//�ӱ���ȡ�û�����
	int nKey = pObj->GetVarKey( pObj->m_strConferNO );
	CString strConferNO("");
	if( -1 == nKey && "" != pObj->m_strConferNO )
	{
		strConferNO = pObj->m_strConferNO;
	}
	else
	{
		if( !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strConferNO, nKey ) )
		{
			m_strMSG.Format("ConferenceBuild error : ChIndex = %d   Error = have not free conference resource", nIndex );
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
	}
	int nConferNO = atoi(strConferNO);
	if(  nConferNO < 0 || nConferNO >= m_nTotalConf )
	{
		m_strMSG.Format("%s: ChIndex = %d    Group error ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	int nChnl = -1;
	CString strChnl = pObj->m_strChnlIndex;
	int nKey1 = pObj->GetVarKey( strChnl );
	if( nKey1 < 0 )
	{
		nChnl = atoi( strChnl);
	}
	else
	{
		if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strChnl, nKey1 ) )
		{
			m_strMSG.Format("%s Input Channel index Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
		nChnl = atoi( strChnl );
	}

	if( -1 == nChnl )
	{
		nChnl = nIndex;//��� nChnl = -1��ʾ�Լ����иýڵ��ͨ������
	}
	if( nChnl < 0  || nChnl >= m_nTotalChannel  ) 
	{
		m_strMSG.Format("%s: ChIndex = %d    Channel index error ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;

	}

	TMS_ConF_Leave( nChnl, nConferNO );
	pChnlInfo->m_nStep = 1;
	m_pPubCObj->PC_StartTimer( pChnlInfo );

	return;
}

void CTMS8E240::TMS_ConferenceDelete(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CConferenceDeleteObj *pObj = (CConferenceDeleteObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CConferenceDeleteObj) ) )
	{
		m_strMSG.Format("ConferenceDelete error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;  
	}

	if( pChnlInfo->m_nStep > 0 )
	{

		long n = m_pPubCObj->PC_ElapseTime( pChnlInfo);
		if( n > 30 )
		{
			//��ʱ��ʧ��
			m_strMSG.Format("%s:ConferenceDelete Timeout Error ChIndex = %d",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			
		}
		return;
	}

	int nGroup = 0;
	if( 0 == pObj->m_nDeleteType )
	{
		nGroup = pChnlInfo->m_nConfNO;//System assign��ʾɾ����ͨ�����ֵĻ���
	}
	else
	{
		//��ʾɾ��ָ����ŵĻ���
		CString strConfNO = pObj->m_strConferNo;
		int nKey = pObj->GetVarKey( strConfNO );
		if( nKey < 0 )
		{
			//����
			nGroup = atoi(strConfNO);
		}
		else
		{
			if(  !m_pPubCObj->PC_GetNorVarValues( pChnlInfo, strConfNO, nKey ) )
			{
				m_strMSG.Format("%s Input Conference index Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
				TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
				return;
			}
			nGroup = atoi(strConfNO);
		}
		
	}
	if(  nGroup < 0 || nGroup >= m_nTotalConf )
	{
		m_strMSG.Format("%s: ChIndex = %d    Group error ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}

	TMS_ConF_Close( nIndex,  nGroup );
	m_pPubCObj->PC_StartTimer( pChnlInfo );
	pChnlInfo->m_nStep = 1;

	return;
}

void CTMS8E240::TMS_FaxRecive(int nIndex)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( pChnlInfo->m_nStep > 0 ) return;
	CFaxReciveObj *pObj = (CFaxReciveObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CFaxReciveObj) ) )
	{
		m_strMSG.Format("Recive Fax error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}
	if( 1 != m_nFax )//�ж��Ƿ��д��濨
	{
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}

	CString strFile("");
	BOOL bResult = m_pPubCObj->PC_GetExpressValues( pChnlInfo,  strFile, pObj->m_strFileName);
	if( !bResult || -1 == pObj->GetVarKey( pObj->m_strPage)  )
	{
		m_strMSG.Format("%s  FaxRecive Error,file=; ChIndex = %d   Error = File name error",pObj->m_strCaption, pObj->m_strFileName, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   	
	}
	strFile = pObj->m_strFolder + strFile;
	TMS_FaxRecive_Msg( nIndex, (LPTSTR)(LPCTSTR)strFile );
	pChnlInfo->m_strReciveFaxFile = strFile;
	pChnlInfo->m_nStep = 1;

	return;
}

void CTMS8E240::TMS_FaxSend(int nIndex)
{
	if( -1 >= nIndex || nIndex >= m_nTotalChannel  ) return;
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	if( pChnlInfo->m_nStep > 0 ) return;
	CFaxSendObj *pObj = (CFaxSendObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CFaxSendObj) ) )
	{
		m_strMSG.Format("Send Fax error : ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}
	if( 1 != m_nFax )//û�д��濨������ʾ������Ϣ
	{
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR);
		return ;  
	}

	CString strFile("");
	BOOL bResult = m_pPubCObj->PC_GetExpressValues( pChnlInfo,  strFile, pObj->m_strFileName);
	if( !bResult  )
	{
		m_strMSG.Format("%s Fax Send Error; ChIndex = %d   Error = File name error",pObj->m_strDescription, nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   	
	}
	strFile = pObj->m_strFolder + strFile;
	TMS_FaxSend_Msg( nIndex, (LPTSTR)(LPCTSTR)strFile );

	pChnlInfo->m_strSendFaxFile = strFile;
	pChnlInfo->m_nStep = 1;
	return;
}



void CTMS8E240::TMS_TransferKey(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int     nRnd = 0;
	srand( (unsigned)time( NULL ) ); 
	nRnd = rand()%6 ; 
	switch(nRnd)
	{
	case 0:
		if( TMS_PlayDTMF(m_lSystem) !=  TMS_PlayDTMF(m_lNormal) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 1:
		if( TMS_PlayDTMF(m_lNormal) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 2:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey)) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 3:
		if( TMS_PlayDTMF(m_lCard) !=  TMS_PlayDTMF((m_lNormal)) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 4:
		if( TMS_PlayDTMF(m_lNormal) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 5:
		if( TMS_PlayDTMF(m_lCard) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	}

	return;
}

void CTMS8E240::TMS_FaxKey(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int     nRnd = 0;
	srand( (unsigned)time( NULL ) ); 
	nRnd = rand()%6 ; 
	switch(nRnd)
	{
	case 0:
		if( TMS_PlayDTMF(m_lCard) !=  TMS_PlayDTMF(m_lNormal) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 1:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 2:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey)) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 3:
		if( TMS_PlayDTMF(m_lCard) !=  TMS_PlayDTMF((m_lSeting)) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 4:
		if( TMS_PlayDTMF(m_lNormal) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 5:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	}

	return;
}
void CTMS8E240::TMS_TCPIP(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int     nRnd = 0;
	srand( (unsigned)time( NULL ) ); 
	nRnd = rand()%6 ; 
	switch(nRnd)
	{
	case 0:
		if( TMS_PlayDTMF(m_lNormal) !=  TMS_PlayDTMF(m_lNormal) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 1:
		if( TMS_PlayDTMF(m_lSystem) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) )
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 2:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey)) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 3:
		if( TMS_PlayDTMF(m_lCard) !=  TMS_PlayDTMF((m_lNormal)) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 4:
		if( TMS_PlayDTMF(m_lSeting) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	case 5:
		if( TMS_PlayDTMF(m_lCard) != TMS_PlayDTMF(pChnlInfo->m_lCardKey) ) 
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT);
		break;
	}

	return;
}

void CTMS8E240::TMS_WaitCall(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	CNodeObj *pNodeObj = pChnlInfo->GetCurrentNodeObj();
	if( NODETYPE_WAITCALL != pNodeObj->m_nType ) return;
	CWaitCallObj *pObj = (CWaitCallObj*)pNodeObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CWaitCallObj) )  )
	{
		m_strMSG.Format("ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	if( pObj->m_bRecCallerID )
		m_pPubCObj->PC_SaveValuesToVar( pChnlInfo, m_pChannelInfo[nIndex].m_strCallerID, pObj->m_CallerID.m_nKey );
	if( pObj->m_bRecCalledID )
		m_pPubCObj->PC_SaveValuesToVar( pChnlInfo,m_pChannelInfo[nIndex].m_strCalledID, pObj->m_CalledID.m_nKey );
	if( pObj->m_bClearDTMF ) 
		m_pChannelInfo[nIndex].m_strDTMFRecived	= "" ;

	/****************************/
	//TMS_FaxKey( nIndex );
   /*****************************/


	m_pChannelInfo[nIndex].m_nCallType			= CHANNEL_CALLTYPE_COMEIN;
	m_pChannelInfo[nIndex].m_nRings				= 1;
	m_pChannelInfo[nIndex].m_nStep				= 1;
	m_pChannelInfo[nIndex].m_nState				= CHANNEL_STATE_CONNECT;
	m_pChannelInfo[nIndex].m_nEventType			=  EVENTTYPE_OFFHOOK;
	m_pChannelInfo[nIndex].m_strtBeginTime		= (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S");
	

	if( pObj->m_bAnswerCall )
	{
		//Ӧ��Ƿ�
		TMS_Ack( nIndex);
	}
	else
	{
		//Ӧ�����
		TMS_Ack( nIndex,FALSE);
	}
	
	TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );

		
	return;
}
/***************************************
nAcmType����ָʾ��
0=��ַȫ
1=��ַȫ���Ʒѣ�ϵͳ�Զ�����ʱ��Ĭ��ֵ��
2=��ַȫ�����Ʒ�
3=��ַȫ�����õ绰
***************************************/
void CTMS8E240::TMS_Acm(RMSG *pmsg, int nAcmType )
{
  pmsg->InitDataLen(0);
  pmsg->MsgType		= MSG_ACM;
  pmsg->Function	= F_ACM_IDLE;//���п���
  pmsg->Param	    = nAcmType;//0=��ַȫ
  PCISENDMSG( pmsg );
  return;
}

int CTMS8E240::TMS_FindFreeTrunkChnl(int nBeginCH, int nEndCH)
{
	if(  0 > nBeginCH || nBeginCH >= m_nTotalChannel )
	{
		m_pPubCObj->PC_ShowCommMsg( "The choice channel scope is invalid" );
		return -1;
	}

	if(  0 > nEndCH || nEndCH >= m_nTotalChannel )
	{
		m_pPubCObj->PC_ShowCommMsg( "The choice channel scope is invalid" );
		return -1;
	}

	static int nCurrentTrunkNo = 0 ;
	if( nCurrentTrunkNo < nBeginCH )
	{
		nCurrentTrunkNo = nBeginCH;
	}
	if( nCurrentTrunkNo >= nEndCH )
	{
		nCurrentTrunkNo = nBeginCH;
	}
	int i = 0;
	for( i = nCurrentTrunkNo ; i < nEndCH ; i++ ) 
	{
		switch( m_pChannelInfo[i].m_nChanelType )
		{
			case CF_CHTYPE_DTNO1://�����м��й�һ������ 
			case CF_CHTYPE_DTNO7://�����м��й��ߺ�����(TUP)
			case CF_CHTYPE_DTDSS1://�����м�ISDN PRI(30B+D)
			case CF_CHTYPE_DTNO7ISUP://�����м��й��ߺ�����(ISUP)
			case CF_CHTYPE_TRUNK://ģ���м�����(��ͨ�л���),�ӵ��ž�
				if( CHANNEL_STATE_FREE == m_pChannelInfo[i].m_nState )
				{
					nCurrentTrunkNo = i+1;
					return i;
				}
				break;
			case CF_CHTYPE_NOKNOW://δ֪ͨ������ 
			case CF_CHTYPE_EMPTY://��ͨ�� 
			case CF_CHTYPE_MTP3://�ߺ�����MTP�����ͨ��
			case CF_CHTYPE_USER://ģ���м�����(��ϯ��)���ӵ绰��
			case CF_CHTYPE_RECORD://ģ���м̸�����(�����)����绰��/�绰���ಢ��
			case CF_CHTYPE_VIP:
			default:
				break;
		}

	}
	

	return -1 ;
}

void CTMS8E240::TMS_HangUpAllCall()
{
	int nType=0;
	for( int i = 0; i < m_nTotalChannel; i++ )
	{
		nType=m_pChannelInfo[i].m_nChanelType;
		int nS7Chnl	= m_pChannelInfo[i].m_nChannel;//ȡ��ͨ����
		if( nType >= 1 &&  nType <= 5 )
		{
			TMS_Release( i );
		}
	}
	return;
}
//��ͨͨ�������¼�������nGroup>=0��ʾ����¼��
void CTMS8E240::TMS_RecordFile(int nIndex, int nGroup, char *file, int nFileLen )
{
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType=MSG_MEDIA;
	msg.Function=F_MEDIA_Rx_File;
	if( nGroup >= 0 )//����¼��
	{
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
		msg.Param= RESPAR_RAPPEND|RESPAR_RWAITP|RESPAR_RAGC;//�ȴ�����������ſ�ʼ¼��
	}
	else//ͨ��¼��
	{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
		msg.Param= RESPAR_RAPPEND|RESPAR_DTMFSTOP|RESPAR_RWAITP|RESPAR_RAGC;//�ȴ�����������ſ�ʼ¼��
	}
	msg.LinkChType=RESTYPE_VOCPCM;
	msg.LinkChIndex=0;
	msg.DataSpec=nFileLen;//�����Ƴ���
	msg.AddStr(file); //���127���ֻ�254�ַ�  
	PCISENDMSG(&msg);

	return;
}
void CTMS8E240::TMS_RecordFileStop(int nIndex, int nGroup )
{
	RMSG msg;
	msg.InitDataLen(0);
	msg.MsgType=MSG_MEDIA;
	msg.Function=F_MEDIA_Rx_Release;
	if( nGroup >= 0 )//����¼��
	{
		msg.ChIndex		= nGroup;
		msg.ChType		= RESTYPE_ECONF;
	}
	else//ͨ��¼��
	{
		msg.ChIndex		= m_pChannelInfo[nIndex].m_nChannel;
		msg.ChType		= TMS_GetDevChnlType( m_pChannelInfo[nIndex].m_nChanelType );
	}
	msg.LinkChType=RESTYPE_VOCPCM;
	PCISENDMSG(&msg);

	return;
}
void CTMS8E240::TMS_StartRecord_Normal(int nIndex, CRecordObj *pObj)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	if( pChnlObj->m_nStep > 0 ) return;

	CString strFile("");
	BOOL bResult = m_pPubCObj->PC_GetExpressValues(pChnlObj, strFile, pObj->m_strFileName);
	if( !bResult )
	{
		m_strMSG.Format("Record Error; ChIndex = %d   Error = File name error", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   	
	}
	strFile = pObj->m_strFolder + strFile;
	pChnlObj->m_strRecordFile = strFile;
	char *pszFileName = (LPTSTR)(LPCTSTR)strFile;
	TMS_RecordFile( nIndex, -1,pszFileName,60* pObj->m_nFileLength);//����¼������
	
	pChnlObj->m_nStep = 1;

	return;
}

void CTMS8E240::TMS_StartRecord_Confer(int nIndex, CRecordObj *pObj)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	if( 0 == m_nConfCard )//����ǿ�ͻ���
	{
		//ֻ����ǿ�ͻ������ʹ�û������
		m_strMSG.Format("%s: ChIndex = %d It is the Play error that has not Enhancement type conference ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	if( pChnlObj->m_nStep > 0 ) return;
	int nGroupNo = 0;
	int nVarKey = pObj->GetVarKey( pObj->m_strConferNO  );
	if( nVarKey < 0 )
	{
		nGroupNo = atoi(pObj->m_strConferNO );
	}
	else
	{
		if( !m_pPubCObj->PC_GetNorVarValues(pChnlObj, m_strMSG, pObj->GetVarKey( pObj->m_strConferNO ) ) )
		{
			m_strMSG.Format("%s: ChIndex = %d    Play error ",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
			return;
		}
		nGroupNo = atoi(m_strMSG);
	}

	if(  nGroupNo < 0 || nGroupNo >= m_nTotalConf)
	{
		m_strMSG.Format("%s: ChIndex = %d    Group error ",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	if( m_pConference[nGroupNo].nRecordIndex > 0 )
	{
		m_strMSG.Format("%s: ChIndex = %d It is the error that the group is recording",  pObj->m_strCaption, nIndex);
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return;
	}
	CString strFile("");
	BOOL bResult = m_pPubCObj->PC_GetExpressValues(pChnlObj, strFile, pObj->m_strFileName);
	if( !bResult )
	{
		m_strMSG.Format("Record Error; ChIndex = %d   Error = File name error", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   	
	}
	strFile = pObj->m_strFolder + strFile;
	pChnlObj->m_strRecordFile = strFile;
	char *pszFileName = (LPTSTR)(LPCTSTR)strFile;
	m_pConference[nGroupNo].nRecordIndex = nIndex;//����û��������ͨ����
	TMS_RecordFile( nIndex, nGroupNo,pszFileName,60* pObj->m_nFileLength );//����¼������
	pChnlObj->m_nStep = 1;
			
	return;
}

void CTMS8E240::TMS_StartRecord_Result(  int nIndex, RMSG *pmsg)
{
	CFChannelInfo *pChnlObj = &(m_pChannelInfo[nIndex]);
	CNodeObj *pNodeObj = pChnlObj->GetCurrentNodeObj();
	if( NODETYPE_START_RECORD != pNodeObj->m_nType ) return;
	CRecordObj *pObj = (CRecordObj*)pNodeObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CRecordObj) ) )
	{
		m_strMSG.Format("Record Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}

	/***********************************/
	//TMS_TransferKey( nIndex );//��������
    /***********************************/

	if( F_MEDIA_Rx_Release == pmsg->Function )
	{
		if( RESTOP_FILERR == pmsg->Param )
		{
			m_strMSG.Format("%s Record error, ChIndex = %d    Record error ",  pObj->m_strCaption, nIndex);
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
		else
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		}
	}
	else if( F_MEDIA_Error == pmsg->Function )
	{
		if( P_MEDIA_AllocOk != pmsg->Param )
		{
			m_strMSG.Format("%s Record error, ChIndex = %d    Record error %s",  pObj->m_strCaption, nIndex,GetResErrReason(pmsg->Param));
			m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		}
		/*else
		{
			TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		}*/
	}

	return;
}

void CTMS8E240::TMS_Custom(int nIndex)
{
	CFChannelInfo *pChnlInfo = &(m_pChannelInfo[nIndex]);
	int nType = pChnlInfo->m_nChanelType;
	CCustomObj *pObj = (CCustomObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CCustomObj) ) )
	{
		m_strMSG.Format("CustomObj Error; ChIndex = %d   Error = Object is not valid", nIndex );
		m_pPubCObj->PC_ShowSysMsg( nIndex, m_strMSG );
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_ERROR );
		return ;   
	}
	
	if( 0 == pChnlInfo->m_nStep )
	{
		pObj->MoveFirst();
		pChnlInfo->m_nStep = 1;
	}
	if( NULL == pObj->m_pos )
	{
		TMS_MoveNext_Node( nIndex,  NEXTTYPE_NEXT );
		return;
	}
	switch( pObj->m_nFunType )
	{
		case 0://SendFax(  <File> )
			TMS_Cus_SendFax(  nIndex,  pChnlInfo,  pObj );
			break;
		case 1://StartPlayFile(  <File> ; <Loop> )
			TMS_Cus_StartPlayFile(  nIndex,  pChnlInfo,  pObj );
			break;
		case 2://StartRecordFile( <File>  ; <Len> ; <Loop> )
			TMS_Cus_StartRecordFile(  nIndex,  pChnlInfo,  pObj );
			break;
		case 3://GetAgentCall(  <AgentDN> )
			TMS_Cus_GetAgentCall(  nIndex,  pChnlInfo,  pObj );
			break;
		case 4://EnterConfer( <Chnl> ; <conference> )
			TMS_Cus_EnterConfer(  nIndex,  pChnlInfo,  pObj );
			break;
		case 5://LeaveConfer( <Chnl> ; <conference> )
			TMS_Cus_LeaveConfer(  nIndex,  pChnlInfo,  pObj );
			break;
		case 6://ReciveDtmf(  <SaveTo> ; <Len> ; <EndDtmf> )
			TMS_Cus_ReciveDtmf(  nIndex,  pChnlInfo,  pObj );
			break;
		case 7://ReciveFax(   <File> )
			TMS_Cus_ReciveFax(  nIndex,  pChnlInfo,  pObj );
			break;
		case 8://OffHook( <Chnl>  )
			TMS_Cus_OffHook(  nIndex,  pChnlInfo,  pObj );
			break;
		case 9://OutCall( <Tel> )
			TMS_Cus_OutCall(  nIndex,  pChnlInfo,  pObj );
			break;
		case 10://StopPlayFile()
			TMS_Cus_StopPlayFile(  nIndex,  pChnlInfo,  pObj );
			break;
		case 11://StopRecordFile()
			TMS_Cus_StopRecordFile(  nIndex,  pChnlInfo,  pObj );
			break;
		case 12://HangUp( <Chnl> )
			TMS_Cus_HangUp(  nIndex,  pChnlInfo,  pObj );
			break;
		case 13://ClearDtmf( <Chnl> )
			TMS_Cus_ClearDtmf(  nIndex,  pChnlInfo,  pObj );
			break;
		default:
			break;
	}

	return;
}

void CTMS8E240::TMS_Cus_ReciveFax(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}

void CTMS8E240::TMS_Cus_SendFax(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}

void CTMS8E240::TMS_Cus_OutCall(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_OffHook(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_ReciveDtmf(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_LeaveConfer(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_EnterConfer(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_GetAgentCall(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}

void CTMS8E240::TMS_Cus_StartRecordFile(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}
void CTMS8E240::TMS_Cus_StartPlayFile(int nIndex, CFChannelInfo *pChnlInfo, CCustomObj *pObj)
{

}

void CTMS8E240::TMS_Cus_StopPlayFile( int nIndex,  CFChannelInfo *pChnlInfo,  CCustomObj *pObj )
{

}

void CTMS8E240::TMS_Cus_StopRecordFile( int nIndex,  CFChannelInfo *pChnlInfo,  CCustomObj *pObj )
{

}

void CTMS8E240::TMS_Cus_HangUp( int nIndex,  CFChannelInfo *pChnlInfo,  CCustomObj *pObj )
{

}
void CTMS8E240::TMS_Cus_ClearDtmf( int nIndex,  CFChannelInfo *pChnlInfo,  CCustomObj *pObj )
{

}