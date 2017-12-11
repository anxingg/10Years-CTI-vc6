// PublicCardObj.cpp: implementation of the CPublicCardObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxmt.h>
#include "chiefivr.h"
#include "CIF5188.h"
#include "PublicCardObj.h"
#include "CalculateExpress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CChiefIVRApp theApp;
extern CPublicCardObj *g_pPubObj;
extern CCriticalSection g_Section;



UINT ExcuteStorePro_Progress( LPVOID pParam )
{
	CoInitialize(NULL);
	CString strError("");
	CNewStoreProObj* pObj = ( CNewStoreProObj*)pParam;
	if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewStoreProObj) ) )
	{
		CoUninitialize();
		return 1;   // if pObject is not valid
	}
	CNewDBConnVar *pDBVar			= pObj->m_pDB;
	CNewRecordsetVar *pRecsetVar	= pObj->m_pRS;
	CFChannelInfo *pChnlInfo		= pObj->m_pChnlInfo;
	if( NULL == pRecsetVar || NULL == pDBVar || NULL == pChnlInfo )
	{
		pObj->m_strError = ("It is error that the object is NULL!");
		CoUninitialize();
		return 2;   // if pObject is not valid
	}

	_CommandPtr pCommand = pRecsetVar->m_pCommand;
	if( NULL == pCommand )
	{
		pCommand.CreateInstance("ADODB.Command");
		pRecsetVar->m_pCommand = pCommand;
	}

	try
	{
		//pDBVar->m_pConnection->Cancel();
		//�������ͬ������ִ���������� Command ����
		//����ĳ�� Command �����Ǵ���������Ĵ洢���̣�
		//��ʱ�ᷢ������Ҫִ�и��� Command ����
		//��ʹ�ö��������ӻ��������� Command ��������ӶϿ���
		pCommand->ActiveConnection = pDBVar->m_pConnection;
		pCommand->CommandText = (_bstr_t )pObj->m_strStoreProName;//(_bstr_t )(LPCSTR) pSProVar->m_strStoreProName;//�洢������
		pCommand->CommandType = adCmdStoredProc;
		HRESULT hr = pCommand->Parameters->Refresh();
		if( FAILED(hr) )
		{
			pObj->m_strError = "It is error to excute the refreshing parameters of the Stored procedure!";
			CoUninitialize();
			return 3;
		}
		
		CString strVarValues("");
		CNodeDataList *pDadaList = &(pObj->m_NodeDataList);
		int nScriptParamNum = pDadaList->GetCount();//�õ��ű���������
		int nStoredProParamNum = pCommand->Parameters->Count;//�õ��ô洢���̵�ʵ�ʲ�������
		if( nStoredProParamNum != nScriptParamNum + 1 )
		{
			pObj->m_strError = "It is unequal  with the reality parameters number of the  stored procedure!";
			CoUninitialize();
			return 4;
		}


		long n = 1;//��������
		POSITION pos = pDadaList->GetHeadPosition( );
		CNodeData *pData = NULL;
		while( pos )
		{
			pData = pDadaList->GetNext(pos);
			if( NULL != pData )
			{
				int nDirection = pCommand->Parameters->GetItem(n)->Direction;
				int nVarType   = pCommand->Parameters->GetItem(n)->Type;
				int nVarKey	   = pObj->GetVarKey( pData->m_strName );
				if( -1 == nVarKey )
				{
					if( adParamInput == nDirection )
					{
						//��������Ϊ����ʱ�������ǳ�������������ϵͳ����
						strVarValues = pData->m_strName;
					}
					else if( adParamInputOutput == nDirection )
					{
						//���Ϊ�������ʱ����Ϊ�Ǵ���
						pObj->m_strError = "The Stored procedure parameter direction is wrong!";
						CoUninitialize();
						return 5;
					}
				}
				else
				{
					if( !g_pPubObj->PC_GetNorVarValues( pChnlInfo,strVarValues, nVarKey ) )
					{
						//��ȡ�ñ���ʧ��
						pObj->m_strError = "The value fetching that parameter variable is wrong";
						CoUninitialize();
						return 6;
					}
				}
				if( adParamInputOutput == nDirection || adParamInput == nDirection )
				{
					pCommand->Parameters->GetItem(n)->Value = (_variant_t)strVarValues;
				}
				n++;
			}

		}//end while
		

		if( pObj->m_bReturnRecordSet )
		{
			//���ؼ�¼������ʱ�洢���̲��ܷ������������ֵ
			//Ҳ����˵Execute()����Ӧ�ÿ�����ֱ����¼���ص��Ժ�Ż���ȷ����.
			//����Ҫô���ؼ�¼����Ҫôȡ���ز���ֵ
			//ʹ�� Command �����ѯ���ݿⲢ���� Recordset �����еļ�¼��
			//�Ա�ִ�д��������������ݿ�ṹ��ȡ�����ṩ�ߵĹ��ܣ�
			//ĳЩ Command ���ϡ����������Ա�����ʱ���ܻ��������
			pRecsetVar->m_pRecordset = pCommand->Execute(NULL,NULL,adCmdStoredProc);
		}
		else
		{
			pCommand->Execute(NULL,NULL,adCmdStoredProc);
			//��ȡ�洢���̵ĵĽ��
			int nReturnVarKey	   = pObj->GetVarKey( pObj->m_strReturn );
			if( -1 == nReturnVarKey )
			{
				//ȡ�ô洢���̵ķ���ֵ
				pObj->m_strError = "The return value getting the Stored procedure is wrong";
				CoUninitialize();
				return 7;
			}
			
			//��һ��������Ϊ �洢���̵ķ���ֵ
			n = 0;
			strVarValues = (char*)(_bstr_t)pCommand->Parameters->GetItem( n )->Value;
			g_pPubObj->PC_SaveValuesToVar( pChnlInfo,strVarValues, nReturnVarKey );
			//pChnlInfo->SaveToNorVar( strVarValues, nReturnVarKey );

		}

		//��ȡ���������ֵ
		if( !pObj->m_bReturnRecordSet )
		{
			//����ȡ��¼����ʱ�򣬿���ȡ�����������ֵ
			pos = pDadaList->GetHeadPosition( );
			n = 1;
			while( pos )
			{
				pData = pDadaList->GetNext(pos);
				if( NULL != pData )
				{
					int nDirection = pCommand->Parameters->GetItem(n)->Direction;
					if( adParamInputOutput == nDirection || adParamOutput == nDirection  )
					{
						//ֻ��Ҫ���������Ҫ����
						strVarValues = (char*)(_bstr_t)pCommand->Parameters->GetItem(n)->Value;
						int nVarKey = pObj->GetVarKey( pData->m_strName );
						g_pPubObj->PC_SaveValuesToVar( pChnlInfo,strVarValues, nVarKey );
						//pChnlInfo->SaveToNorVar( strVarValues, nVarKey );
					}
					n++;
				}
			}//end while
		}

		CoUninitialize();
		return 0;	
	}
	catch(_com_error e)///��׽�쳣
	{
		pChnlInfo->PrintComError(e);
		pChnlInfo->PrintProviderError( pDBVar->m_pConnection );
		pChnlInfo->PrintADOConnectString( pDBVar->m_pConnection );
		pChnlInfo->PrintADOVersion( pDBVar->m_pConnection );

		CoUninitialize();
		return 8;
	} 
	catch(...)///��׽�쳣
	{
		pObj->m_strError ="Excute Stored procedure catch(...) error";
		CoUninitialize();
		return 0;
	}
	return 0;
}

//////////////////////////////////////////////////////////////
//�ر���ʾ�����߳���ʹ��ADOʱ���������̺߳��������CoInitialize
//			��CoUninitialize�������Ա��ʼ��Com�ӿڣ�����Ͳ���
//			���߳���ʹ��ADO,�����û��CoInitialize����ʾ��
//
////////////////////////////////////////////////////////////
UINT ExcuteSQL_Progress( LPVOID pParam )
{
	CoInitialize(NULL);
	CString strError("");
	CNewSQLExcuteObj* pObj = ( CNewSQLExcuteObj*)pParam;
	if (pObj == NULL )
	{
		CoUninitialize();
		return 1;   // if pObject is not valid
	}
	CNewDBConnVar *pDB = pObj->m_pDB;
	CNewRecordsetVar *pRS = pObj->m_pRS;
	if( NULL == pDB || NULL == pRS )
	{
		pObj->m_strError = "it is error that the object is NULL!";
		CoUninitialize();
		return 2;
	}
	try
	{
		
		_variant_t RecordsAffected;
		pRS->m_pRecordset = pDB->m_pConnection->Execute( (_bstr_t)pObj->m_str,&RecordsAffected,adCmdText);
		CoUninitialize();
		return 0;	
	}
	catch(_com_error e)///��׽�쳣
	{

		
		CString ErrStr="";
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
		ErrStr.Format("[Code = %08lx][Code meaning = %s][Source = %s][Description = %s]",
		   e.Error(),e.ErrorMessage(),(LPCSTR) bstrSource,(LPCSTR) bstrDescription );
		pObj->m_strError = ErrStr;
        if( NULL != pDB )
		{
			pDB->PrintProviderError(strError, pDB->m_pConnection );
			pObj->m_strError = pObj->m_strError + "\n" + strError;
		}

		
		CoUninitialize();
		return 3;
	} 
	catch(...)///��׽�쳣
	{
		pObj->m_strError = "Excute SQL catch(...) error";
		CoUninitialize();
		return 4;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPublicCardObj::CPublicCardObj()
{
	g_pPubObj		= this;
	m_pwndProg		= NULL;
	m_plistTCPIP	= NULL;
	m_pCommList		= NULL;
	m_plistScript	= NULL;
	m_plstChannel	= NULL;
	m_plistSystem	= NULL;
	m_plstConfer	= NULL;
	m_plstFax		= NULL;
	m_plstXmsEvent	= NULL;
	m_hWnd			= NULL;

	
	m_pSysDBConnVarList = &(m_SysDBConnVarList);
	m_pSysDLLVarList	= &(m_SysDLLVarList);
	m_pSysNormalVarList = &(m_SysNormalVarList);
	m_pVipCallSetObjList = &(m_VipCallSetObjList);
}

CPublicCardObj::~CPublicCardObj()
{
	while ( m_SysDBConnVarList.GetHeadPosition() )
	{
		delete m_SysDBConnVarList.RemoveHead();
	}

	while ( m_SysDLLVarList.GetHeadPosition() )
	{
		delete m_SysDLLVarList.RemoveHead();
	}
	while ( m_SysNormalVarList.GetHeadPosition() )
	{
		delete m_SysNormalVarList.RemoveHead();
	}
	while ( m_VipCallSetObjList.GetHeadPosition() )
	{
		delete m_VipCallSetObjList.RemoveHead();
	}
	if( m_nIsSaveSysLog > 0  )
	{

		m_ErrorLogFile.Close();
	}
}


BOOL CPublicCardObj::PC_GetNorVarValues(CFChannelInfo *pInfo, CString &strValues, int nVarKey)
{
	if( NULL == pInfo ) return FALSE;
	if( nVarKey < 0 ) return FALSE;
	if( g_pD08a->CompareXiaoYu(nVarKey, 51) )
	//if( nVarKey <= 50 )
	{
		//����
		return pInfo->GetContantValues( strValues, nVarKey );
	}
	else if( nVarKey > 50 && nVarKey < 100 )
	{
		//ϵͳ����
		return PC_GetSysNorVarValues( strValues, nVarKey );

	}
	else if( nVarKey >= 100 && nVarKey < 4000 )
	{
		//��ͨ����
		return pInfo->GetNorVarValues( strValues, nVarKey );
	}
	else
	{
		//ģ���������
		return pInfo->GetModuleParamValues( strValues, nVarKey );
	}
}


BOOL CPublicCardObj::PC_GetNorVarValues( CFChannelInfo *pInfo,CString &strValues, int &nVarType, int nVarKey)
{
	if( NULL == pInfo ) return FALSE;
	if( nVarKey < 0 ) return FALSE;
	if( g_pD08a->CompareXiaoYu(nVarKey, 51) )
	//if( nVarKey <= 50 )
	{
		//����
		return pInfo->GetContantValues( strValues,nVarType, nVarKey );
	}
	else if( nVarKey > 50 && nVarKey < 100 )
	{
		//ϵͳ����
		return PC_GetSysNorVarValues( strValues,nVarType, nVarKey );
	}
	else if( nVarKey >= 100 && nVarKey < 4000 )
	{
		//��ͨ����
		return pInfo->GetNorVarValues( strValues, nVarType,nVarKey );
	}
	else
	{
		//ģ���������
		return pInfo->GetModuleParamValues( strValues, nVarKey );
	}
}

BOOL CPublicCardObj::PC_GetSysNorVarValues(CString &strValues, int nVarKey)
{
	POSITION pos  = m_pSysNormalVarList->GetHeadPosition();
	while ( pos )
	{
		CNormalVar * pVar = m_pSysNormalVarList->GetNext(pos);
		if( pVar != NULL )
		{
			if( nVarKey == pVar->m_nKey )
			{
				g_Section.Lock();
				strValues = pVar->m_strVariable;
				g_Section.Unlock();
				return TRUE ;
			}
		}
	}

	return FALSE;
}


BOOL CPublicCardObj::PC_GetSysNorVarValues(CString &strValues, int &nVarType, int nVarKey)
{
	POSITION pos  = m_pSysNormalVarList->GetHeadPosition();
	while ( pos )
	{
		CNormalVar * pVar = m_pSysNormalVarList->GetNext(pos);
		if( pVar != NULL )
		{
			if( nVarKey == pVar->m_nKey )
			{
				g_Section.Lock();
				strValues = pVar->m_strVariable;
				nVarType = pVar->m_nVarDataType;
				g_Section.Unlock();
				return TRUE ;
			}
		}
	}

	return FALSE;
}

BOOL CPublicCardObj::PC_GetExpressValues( CFChannelInfo *pInfo, CString &strNewExpress, CString strOldExpress)
{
	if( NULL == pInfo ) return FALSE;
	int nPos1	= -1;
	int nPos2	= -1;
	int nPos3	= -1;
	int nVarKey = -1;

	BOOL bResult = TRUE;
	CString strVar("");
	CString strVarValues("");
	
	CString strTemp = strOldExpress;
	strTemp.Replace("\"",NULL);
	strTemp.Replace("+",NULL);
	strTemp.Replace( " " , NULL );
	strNewExpress = "";
	while(1)
	{
		nPos1 = strTemp.Find( '|', nPos3+1 );
		if( -1 == nPos1 )
		{
			//ȡ�ú���ĳ�����
			strVarValues = strTemp.Right( strlen(strTemp) - nPos3-1 );
			strNewExpress = strNewExpress + strVarValues;
			break;
		}
		strNewExpress =  strNewExpress + strTemp.Mid( nPos3+1, nPos1 - nPos3-1 );
		nPos2 = strTemp.Find( ':', nPos1+1 );
		nPos3 = strTemp.Find( '|', nPos1+1 );
		strVar = strTemp.Mid(nPos1+2,nPos2-nPos1-2);
		nVarKey = atoi(strVar);
		if( 0 == nVarKey ) 
		{
			bResult = FALSE;
			break;
		}
		if( !PC_GetNorVarValues( pInfo, strVarValues, nVarKey ) )
		{
			//û���ҵ��ñ���
			bResult = FALSE;
			break;
		}
		strNewExpress = strNewExpress + strVarValues;

	}//end while

	if( FALSE == bResult && "" == strNewExpress  )
	{
		//����û�б���
		strNewExpress = strTemp;
	}

	return bResult;
}


BOOL CPublicCardObj::PC_SaveValuesToVar( CFChannelInfo *pInfo , CString strValues, int nVarKey)
{
	if( NULL == pInfo ) return FALSE;
	if( nVarKey < 0 ) return FALSE;
	if( g_pD08a->CompareXiaoYu( nVarKey, 51 ) )
	//if( nVarKey <= 50 )
	{
		//����
		return FALSE;
	}
	else if( nVarKey > 50 && nVarKey < 100 )
	{
		//ϵͳ����
		return PC_SaveValuesToSysVar( strValues, nVarKey );

	}
	else if( nVarKey >= 100 && nVarKey < 4000 )
	{
		//��ͨ����
		return pInfo->SaveToNorVar(strValues, nVarKey );
	}
	else
	{
		//ģ���������
		return pInfo->SaveToModuleParam( strValues, nVarKey );
	}


}






long  CPublicCardObj::PC_PlayDTMF(long lDTMF)
{
	long kk = 0;
	kk = (lDTMF-100)*10+20-30*10-(700+30-3) + ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 ))) - ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 ))) + ((12*2 - (12-12) + (10*10- 10*10 ))-(12*2 - (12-12) + (10*10- 10*10 )));
	return kk;
}

BOOL CPublicCardObj::PC_LoadSysScript( CCIFSysScriptDoc *pDoc )
{

	POSITION pos = pDoc->m_SysScriptListObj.GetHeadPosition();
	while( pos )
	{
		CCFSysScriptSetObj *pObj = pDoc->m_SysScriptListObj.GetNext(pos);
		if( NULL != pObj )
		{
			if( !PC_InitSysScript(pObj) )
			{
				break ;
			}	
		}
	}

	return TRUE;
}


void CPublicCardObj::PC_InitSystemVar()
{
	int i = 0;
	for( i = 51; i < 59; i++ )
	{
		CNewDBConnVar *pNewVar =  new CNewDBConnVar;
		pNewVar->m_strName.Format("#%d:SysDatabase%d", i, i-50 );
		pNewVar->m_nKey = i;
		pNewVar->m_nVarDataType = DATATYPE_DBCONNECT;
		m_pSysDBConnVarList->AddTail( pNewVar );

	}

	for( i = 59; i < 67; i++ )
	{
		CDLLVariable *pNewVar =  new CDLLVariable;
		pNewVar->m_strName.Format("#%d:SysDLLHandle%d", i, i-58 );
		pNewVar->m_nKey = i;
		pNewVar->m_nVarDataType = DATATYPE_DLL;
		m_pSysDLLVarList->AddTail( pNewVar );

	}

	//String system variable
	for( i = 67; i < 75; i++ )
	{
		CNormalVar *pNewVar =  new CNormalVar;
		pNewVar->m_strName.Format("#%d:SysString%d", i, i-66 );
		pNewVar->m_nKey = i;
		pNewVar->m_nVarDataType = DATATYPE_STRING;
		m_pSysNormalVarList->AddTail( pNewVar );
	}
	
	//Double system variable
	for( i = 75; i < 79; i++ )
	{
		CNormalVar *pNewVar =  new CNormalVar;
		pNewVar->m_strName.Format("#%d:SysDouble%d", i, i-74 );
		pNewVar->m_nKey = i;
		pNewVar->m_nVarDataType = DATATYPE_FLOAT;
		m_pSysNormalVarList->AddTail( pNewVar );
	}

	//Integer system variable
	for( i = 79; i < 87; i++ )
	{
		CNormalVar *pNewVar =  new CNormalVar;
		pNewVar->m_strName.Format("#%d:SysInteger%d", i, i-78 );
		pNewVar->m_nKey = i;
		pNewVar->m_nVarDataType = DATATYPE_INTEGER;
		m_pSysNormalVarList->AddTail( pNewVar );
	}


	return;
}


BOOL  CPublicCardObj::PC_InitSysScript(CCFSysScriptSetObj *pObj)
{
	CString strFile = pObj->m_strScriptFile;
	if( "" == strFile ) return FALSE;

	CCFDocument doc;
	if( ! doc.OnOpenDocument( strFile ) ) 
	{
		return FALSE;
	}
	
	POSITION pos = doc.m_objects.GetHeadPosition();
	CNodeObj *pFirstObj = NULL;
	while (pos != NULL)
	{
		pFirstObj = doc.m_objects.GetNext(pos);
		if( NULL == pFirstObj ) continue;
		break;
	}//end while

	if( NULL == pFirstObj ) return FALSE;
	CNodeObj *pCurObj = pFirstObj;
	while( 1 )
	{
		int nType = pCurObj->m_nType;
		if( NODETYPE_START == nType )
		{
			pCurObj = PC_MoveNextObj( NEXTTYPE_NEXT, pCurObj, &doc );
			if( NULL == pCurObj ) break;
		}
		else if( NODETYPE_NODE_END == nType )
		{
			break;
		}
		else if( NODETYPE_OPEN_DATABASE == nType )
		{
			pCurObj = PC_OpenDatabase(pCurObj, &doc );
			if( NULL == pCurObj ) break;
		}
		else if( NODETYPE_EXCUTE_SQL == nType )
		{
			CNewSQLExcuteObj * pNewObj = new CNewSQLExcuteObj( *(CSQLExcuteObj *)pCurObj );
			pCurObj = PC_ExcuteSQL(pNewObj, &doc );
			delete pNewObj;
			pNewObj = NULL;
			if( NULL == pCurObj ) break;
		}
		else if( NODETYPE_DLL_LOAD == nType )
		{
			pCurObj = PC_DLLLoad(pCurObj, &doc );
			if( NULL == pCurObj ) break;
		}
		else 
		{
			break;
		}
	}//end while

	
	return TRUE;
}



CNodeObj * CPublicCardObj::PC_MoveNextObj(int nNextType, CNodeObj *pCurObj, CCFDocument *pDoc)
{
	if( NULL == pCurObj || pDoc == NULL )  return NULL;
	CNextNode *pNextObj = pCurObj->FindNextNodeObj( nNextType );
	if( NULL == pNextObj ) return NULL;
	
	POSITION pos = pDoc->m_objects.GetHeadPosition();
	while (pos != NULL)
	{
		CNodeObj *pObj = pDoc->m_objects.GetNext(pos);
		if( NULL == pObj ) break;
		if( pObj->m_nKey == pNextObj->m_nNodeObjKey )
		{
			return pObj;
		}
	}//end while

	return NULL;
}

CNodeObj * CPublicCardObj::PC_OpenDatabase(CNodeObj *pCurObj, CCFDocument *pDoc)
{
	CString strMSG("");
	CDBOpenObj *pObj = (CDBOpenObj*)pCurObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDBOpenObj) ) )
	{
		strMSG.Format("OpenDatabase Error, Error = Object is not valid" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );   
	}
	
	int nKey = pObj->m_nDBKey;
	if( -1 == nKey )
	{
		strMSG.Format("OpenDatabase  %s Error variable! ", pObj->m_strCaption );
		PC_ShowSysMsg(0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}

	if( g_pD08a->CompareVarKey(0, pObj->m_nDBType) )
	//if( 0 == pObj->m_nDBType ) //��ͨ����������ݿ�
	{
		strMSG.Format("OpenDatabase Error = The system script file can not use the normal var!" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	//��ϵͳ��������ݿ�
	CNewDBConnVar *pDBVar = PC_FindSystemDBVar( nKey );
	if( NULL == pDBVar )
	{
		strMSG.Format("OpenDatabase  %s Error = not have the var", pObj->m_strCaption);
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	
	CString strError="";
	BOOL bResult = pDBVar->OpenDataBase( pObj->m_strConn, strError);
	if( !bResult )
	{
		strMSG.Format("OpenDatabase %s Error = %s  ; Connstr = %s ", pObj->m_strCaption, strError, pDBVar->m_strConn );
		PC_ShowSysMsg( 0, strMSG );
		return  PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	else
	{
		return  PC_MoveNextObj(NEXTTYPE_NEXT,pCurObj,pDoc );
	}

	return NULL;
}

void CPublicCardObj::PC_DLLLoad(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CDLLLoadObj *pObj = (CDLLLoadObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDLLLoadObj) ) )
	{
		strMSG.Format("DLL Load error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	CDLLVariable *pDLLVar = PC_FindDLLVariable( pChnlObj, pObj->m_nDLLKey );
	if( NULL == pDLLVar ) 
	{
		strMSG.Format("%s not have  the DLL Variable ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	pDLLVar->m_strDLLFileName = pObj->m_strDLLFile;
	CString strFile	= pDLLVar->m_strDLLFileName;
	if( NULL != pDLLVar->m_hDLL )
	{
		if( !FreeLibrary( pDLLVar->m_hDLL ) )
		{
			strMSG.Format("%s Free the old DLL fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
			return ; 
		}
		pDLLVar->m_hDLL = NULL;
	}
	pDLLVar->m_hDLL = LoadLibrary( strFile );
	if( NULL == pDLLVar->m_hDLL )
	{
		strMSG.Format("%s Load DLL fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ; 
	}
	else
	{
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT );
	}

	return;
}

void CPublicCardObj::PC_DLLUnLoad(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CDLLUnLoadObj *pObj = (CDLLUnLoadObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDLLUnLoadObj) ) )
	{
		strMSG.Format("DLL UnLoad error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );

		return ;  
	}

	CDLLVariable *pDLLVar = PC_FindDLLVariable( pChnlObj, pObj->m_nDLLKey );
	if( NULL == pDLLVar ) 
	{
		
		strMSG.Format("%s not have  the DLL Variable ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	if( NULL != pDLLVar->m_hDLL )
	{
		if( !FreeLibrary( pDLLVar->m_hDLL ) )
		{
			int k = GetLastError();
			strMSG.Format("%s Unload the  DLL fail, Error Codes = %d ;ChIndex = %d", pObj->m_strCaption, k, nIndex);
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
			pDLLVar->m_hDLL = NULL;
			return ; 
		}
		
	}

	PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT );
	return;
}


CNodeObj * CPublicCardObj::PC_DLLLoad(CNodeObj *pCurObj, CCFDocument *pDoc)
{
	CString strMSG("");
	CDLLLoadObj *pObj = (CDLLLoadObj*)pCurObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDLLLoadObj) ) )
	{
		strMSG.Format("DLL Load  Error = Object is not valid" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}

	if( g_pD08a->CompareVarKey(0,pObj->m_nHandleType) )
	//if( 0 == pObj->m_nHandleType )
	{
		//����ϵͳDLL������������ش���
		strMSG.Format("DLL Load  Error=The system script file can not use the normal var!" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	CDLLVariable *pDLLVar = PC_FindSystemDLLVar(  pObj->m_nDLLKey );
	if( NULL == pDLLVar ) 
	{
		strMSG.Format("%s not have  the DLL Variable", pObj->m_strCaption);
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc ); 
	}

	pDLLVar->m_strDLLFileName = pObj->m_strDLLFile;
	CString strFile	= pDLLVar->m_strDLLFileName;
	if( NULL != pDLLVar->m_hDLL )
	{
		if( !FreeLibrary( pDLLVar->m_hDLL ) )
		{
			strMSG.Format("%s Free the old DLL fail ;", pObj->m_strCaption);
			PC_ShowSysMsg( 0, strMSG );
			return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
		}
		pDLLVar->m_hDLL = NULL;
	}
	pDLLVar->m_hDLL = LoadLibrary( strFile );
	if( NULL == pDLLVar->m_hDLL )
	{
		strMSG.Format("%s Load DLL fail ;", pObj->m_strCaption);
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	else
	{
		return PC_MoveNextObj(NEXTTYPE_NEXT,pCurObj,pDoc );
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////
//֧�ֱ�׼�Ĺ���DLL��ͬʱ��DLL�ĺ���Ӧ������ԭ����У�
//1:DLL������C++���Ա�д
//2:�����ķ���ֵΪVoid;
//3:�����Ĳ�������Ϊ3;
//4:����ʾ��: void SampleFunction( int *nParam1, char *pszParam2, char *pszParam3 )
/////////////////////////////////////////////////////////
void CPublicCardObj::PC_DLLCall(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CDLLCallObj *pObj = (CDLLCallObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDLLCallObj) ) )
	{
		strMSG.Format("DLL Call error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	if( "" == pObj->m_strFunction )
	{
		strMSG.Format("%s not have  DLL function ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	CDLLVariable *pDLLVar = PC_FindDLLVariable( pChnlObj, pObj->m_nDLLKey );
	if( NULL == pDLLVar ) 
	{
		strMSG.Format("%s not have  the DLL Variable ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
		return ;  
	}

	if( NULL == pDLLVar->m_hDLL )
	{
		strMSG.Format("%s DLL handle is NULL  ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}

	
	CString strParam1("");
	CString strParam2("");
	CString strParam3("");
	int nParamKey1 = -1;
	int nParamKey2 = -1;
	int nParamKey3 = -1;
	
	//ȡ��������
	CNodeDataList *pDadaList = &(pObj->m_NodeDataList);
	if( g_pD08a->CompareXiaoYu(pDadaList->GetCount(),3) )
	//if( pDadaList->GetCount() < 3 )
	{
		strMSG.Format("%s the Param number < 3  ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}
	POSITION pos = pDadaList->GetHeadPosition( );
	CNodeData *pData = pDadaList->GetNext(pos);
	////��1������
	if( NULL != pData )
	{
		nParamKey1 = pData->m_nKey;
		if( !PC_GetNorVarValues( pChnlObj, strParam1, nParamKey1 ) )
		{
			strMSG.Format("%s Get Param1 Value fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
			return ; 
		}
	}
	else
	{
		strMSG.Format("%s Get Param1  fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
		return ; 
	}

	//��2������
	pData = pDadaList->GetNext(pos);
	if( NULL != pData )
	{
		nParamKey2 = pData->m_nKey;
		if( !PC_GetNorVarValues( pChnlObj, strParam2,nParamKey2 ) )
		{
			strMSG.Format("%s Get Param2 Value fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
			return ; 
		}
	}
	else
	{
		strMSG.Format("%s Get Param2  fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ; 
	}

	//��3������
	pData = pDadaList->GetNext(pos);
	if( NULL != pData )
	{
		nParamKey3 = pData->m_nKey;
		if( !PC_GetNorVarValues( pChnlObj, strParam3, nParamKey3 ) )
		{
			strMSG.Format("%s Get Param3 Value fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
			return ; 
		}
	}
	else
	{
		strMSG.Format("%s Get Param3  fail ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ; 
	}

	try
	{

			DLLCALL_FUNCTION lpfnDllFunc;// = NULL;
			int nParam1 = atoi( strParam1 );
			char szParam2[500];
			char szParam3[500];
			memset( szParam2, 0, sizeof(szParam2) );
			memset( szParam3, 0, sizeof(szParam3) );

			memmove( szParam2, strParam2, strlen(strParam2) >= 500 ? 499:strlen(strParam2) );
			memmove( szParam3, strParam3, strlen(strParam3) >= 500 ? 499:strlen(strParam3) );

			lpfnDllFunc = (DLLCALL_FUNCTION)GetProcAddress( pDLLVar->m_hDLL, pObj->m_strFunction );
			if (!lpfnDllFunc)
			{
				int k = GetLastError();
				strMSG.Format("%s DLL handle error ,Error Codes = %d ;ChIndex = %d", pObj->m_strCaption, k, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return ;  
			}
			else
			{
			  // call the function
			  lpfnDllFunc( &nParam1, szParam2, szParam3 );
			}

			//����ֵ
			strParam1.Format("%d",nParam1);
			strParam2.Format("%s",szParam2);
			strParam3.Format("%s",szParam3);
			PC_SaveValuesToVar( pChnlObj,  strParam1, nParamKey1 );
			PC_SaveValuesToVar( pChnlObj,  strParam2, nParamKey2 );
			PC_SaveValuesToVar( pChnlObj,  strParam3, nParamKey3 );
			PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT);
	}
	catch(...)
	{
		int k = GetLastError();
		strMSG.Format("%s DLL handle error ,Error Codes = %d ;ChIndex = %d", pObj->m_strCaption, k, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
	}
	return;
}

void CPublicCardObj::PC_ShowCommMsg(CString strMSG)
{
	if( NULL == m_pCommList ) return;
	if( !IsWindow( m_pCommList->GetSafeHwnd() ) ) return;
	strMSG = (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S") + strMSG;
	int n_line = m_pCommList->GetCount();
	if (n_line > 200)
	{
		m_pCommList->InsertString(n_line, strMSG );
		m_pCommList->SetCurSel(n_line);
		m_pCommList->DeleteString(0);
		m_pCommList->ResetContent();
	}
	else
	{
		m_pCommList->InsertString(n_line, strMSG);
		m_pCommList->SetCurSel(n_line);
	}

	return;
}


void CPublicCardObj::PC_ShowSysMsg(int nIndex, CString strMSG)
{
	
	if( NULL == m_plistSystem ) return;
	if( !IsWindow( m_plistSystem->GetSafeHwnd() ) ) return;
	strMSG = (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S  ") + strMSG;
	int n_line = m_plistSystem->GetCount();
	if (n_line > 2000)
	{
		m_plistSystem->InsertString(n_line, strMSG );
		m_plistSystem->SetCurSel(n_line);

		m_plistSystem->DeleteString(0);
		m_plistSystem->ResetContent();
	}
	else
	{
		m_plistSystem->InsertString(n_line, strMSG);
		m_plistSystem->SetCurSel(n_line);
	}
	PC_WriteSysLog((LPCTSTR )strMSG);
	return;
}



void CPublicCardObj::PC_ShowChannelBlock(int nIndex, CString strBlock)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	m_plstChannel->SetItemText( nIndex,3, strBlock );
}

void CPublicCardObj::PC_ShowChannelDTMF(int nIndex, CString strDTMF)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	m_plstChannel->SetItemText( nIndex,10, strDTMF );
}

void CPublicCardObj::PC_ShowChannelConferenceNO(int nIndex, int nConferNO)
{
	if( nIndex < 0 || NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	CString str("");
	str.Format("%d", nConferNO );
	m_plstChannel->SetItemText( nIndex,8, str );
	
	return;
}

void CPublicCardObj::PC_ShowChannelState(int nIndex, CString strState)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	m_plstChannel->SetItemText( nIndex,4, strState );
	return;
}
void CPublicCardObj::PC_ShowChannelCallerID(int nIndex, CString strCallerID)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	m_plstChannel->SetItemText( nIndex,7, strCallerID );
	return;
}
void CPublicCardObj::PC_ShowChannelCalledID(int nIndex, CString strCalledID)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	m_plstChannel->SetItemText( nIndex,8, strCalledID );
	return;
}
void CPublicCardObj::PC_ShowConnectToChnl(int nIndex, int nConnectToIndex)
{
	if( nIndex < 0 || NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	CString str("");
	str.Format("%d", nConnectToIndex );
	m_plstChannel->SetItemText( nIndex,5, str );
	
	return;
}


CNewDBConnVar * CPublicCardObj::PC_FindSystemDBVar(int nKey)
{
	POSITION pos = m_pSysDBConnVarList->GetHeadPosition();
	while(pos)
	{
		CNewDBConnVar * pVar = m_pSysDBConnVarList->GetNext(pos);
		if( pVar != NULL )
		{
			if( nKey == pVar->m_nKey )
			{
				return pVar ;
			}
		}
	}

	return NULL;
}


CDLLVariable* CPublicCardObj::PC_FindSystemDLLVar(int nKey)
{
	POSITION pos = m_pSysDLLVarList->GetHeadPosition();
	while(pos)
	{
		CDLLVariable * pVar = m_pSysDLLVarList->GetNext(pos);
		if( pVar != NULL )
		{
			if( nKey == pVar->m_nKey )
			{
				return pVar ;
			}
		}
	}

	return NULL;
}



BOOL  CPublicCardObj::PC_MoveNext_Node(int nIndex,CFChannelInfo *pChnlObj, CNextNode *pNextObj)
{
	BOOL bResult  = pChnlObj->MoveNext_Node( pNextObj );
	if( bResult )
	{
		CNodeObj *pObj = pChnlObj->GetCurrentNodeObj();
		PC_ShowChannelBlock( nIndex, pObj->m_strCaption );
	}
	
	return bResult;
}
BOOL  CPublicCardObj::PC_MoveNext_Node(int nIndex,CFChannelInfo *pChnlObj,  int nNextType)
{

	BOOL bResult  = pChnlObj->MoveNext_Node( nNextType );
	if( bResult )
	{
		
		CNodeObj *pObj = pChnlObj->GetCurrentNodeObj();
		PC_ShowChannelBlock( nIndex, pObj->m_strCaption );
	}
	
	return bResult;
}
void CPublicCardObj::PC_SetVariable( int nIndex,CFChannelInfo *pChnlObj)
{
	PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT );
	return;
}

void CPublicCardObj::PC_OpenDatabase(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CDBOpenObj *pObj = (CDBOpenObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDBOpenObj) ) )
	{
		strMSG.Format("OpenDatabase Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj, NEXTTYPE_ERROR );
		return ;   
	}
	
	
	int nKey = pObj->m_nDBKey;
	if( -1 == nKey )
	{
		strMSG.Format("OpenDatabase  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj, NEXTTYPE_ERROR );
		return;
	}

	CNewDBConnVar *pDBVar = NULL;
	if( g_pD08a->CompareVarKey(0, pObj->m_nDBType) )
	//if( 0 == pObj->m_nDBType ) //��ͨ����������ݿ�
		pDBVar = pChnlObj->FindDBConnectVar( nKey );
	else//��ϵͳ��������ݿ�
		pDBVar = PC_FindSystemDBVar( nKey );

	if( NULL == pDBVar )
	{
		strMSG.Format("OpenDatabase  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj,NEXTTYPE_ERROR );
		return;
	}

	
	CString strError="";
	BOOL bResult = pDBVar->OpenDataBase( pObj->m_strConn, strError);
	if( !bResult )
	{
		strMSG.Format("OpenDatabase %s Error = %s  ; Connstr = %s ; ChIndex = %d ", pObj->m_strCaption, strError, pDBVar->m_strConn, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
	}
	else
	{
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT );
	}

	return;
}



void CPublicCardObj::PC_CloseDatabase(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CDBCloseObj *pObj = (CDBCloseObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDBCloseObj) ) )
	{
		strMSG.Format("CloseDatabase Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;
	}
	
	int nKey = pObj->m_nDBKey;
	if( -1 == nKey )
	{
		strMSG.Format("CloseDatabase  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return;
	}
	
	
	CNewDBConnVar *pDBVar = NULL;
	if( g_pD08a->CompareVarKey(0,pObj->m_nDBType) )
	//if( 0 == pObj->m_nDBType )
		pDBVar = pChnlObj->FindDBConnectVar( nKey );
	else
		pDBVar = PC_FindSystemDBVar( nKey );

	if( NULL == pDBVar )
	{
		strMSG.Format("CloseDatabase  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
		return;
	}

	CString strError="";
	BOOL bResult = pDBVar->CloseDatabase(strError);
	if( !bResult )
	{
		strMSG.Format("CloseDatabase %s Error = %s  ; Connstr = %s ; ChIndex = %d ", pObj->m_strCaption, strError, pDBVar->m_strConn, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
	}
	else
	{
		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_NEXT );
	}

	return;
}

void CPublicCardObj::PC_ExcuteSQL(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CNewSQLExcuteObj *pObj = (CNewSQLExcuteObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewSQLExcuteObj) ) )
	{
		strMSG.Format("ExcuteSQL Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;   
	}
	
	int nStep = pChnlObj->m_nStep;
	switch( nStep )
	{
	case 0:
		{
			
			if( -1 == pObj->m_nDBKey )
			{
				strMSG.Format("ExcuteSQL  %s Error = not have the var %s; ChIndex = %d ", pObj->m_strCaption,pObj->m_strDB, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return;
			}
			CNewDBConnVar *pDBVar = NULL;
			if( g_pD08a->CompareVarKey(0,pObj->m_nDBType) )
			//if( 0 == pObj->m_nDBType ) //��ͨ����������ݿ�
				pDBVar = pChnlObj->FindDBConnectVar( pObj->m_nDBKey );
			else//��ϵͳ��������ݿ�
				pDBVar = PC_FindSystemDBVar( pObj->m_nDBKey );

			if( NULL == pDBVar )
			{
				strMSG.Format("ExcuteSQL  %s Error = not have the DB var; ChIndex = %d ", pObj->m_strCaption, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return;
			}

			if( !pDBVar->m_bConnect)
			{
				strMSG.Format("[%s][Excute SQL Error ChnlIndex:%d][Database connect is closed]",pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return;
			}

			CNewRecordsetVar *pRS = pChnlObj->FindRecordsetVar( pObj->m_nRecordsetKey );
			if( NULL == pRS )
			{
				strMSG.Format("[%s][It is ExcuteSQL Error that have not the Recordset  var]; ChIndex = %d ", pObj->m_strCaption, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return;
			}

			CString strSQL("");
			BOOL bResult = pChnlObj->GetSQLExpressValues( strSQL, pRS->m_strSQL );
			if(!bResult)
			{
				strMSG.Format("[%s][Excute SQL Error ChnlIndex:%d][SQL:%s]",pObj->m_strCaption, nIndex,strSQL);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				return ;
			}
			
			pObj->m_pDB = pDBVar;
			pObj->m_pRS = pRS;
			pObj->m_str = strSQL;

			//����ִ��SQL�����߳�
			CWinThread* pCallThread = AfxBeginThread(ExcuteSQL_Progress, pObj,  THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			::DuplicateHandle(GetCurrentProcess(), pCallThread->m_hThread,  GetCurrentProcess()  , &pChnlObj->m_Threadhandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
			ResumeThread(pChnlObj->m_Threadhandle);
			pChnlObj->m_nStep = 1;
		}
		break;
	case 1:
		{
			DWORD   dwExitCode = 0;
			GetExitCodeThread( pChnlObj->m_Threadhandle, &dwExitCode );
			if( STILL_ACTIVE == dwExitCode )
			{
				return;
			}
			else
			{
				if( 0 != dwExitCode )
				{
					strMSG.Format("[%s][Excute SQL Error][ExitCode:%d][%s]",pObj->m_strCaption, dwExitCode ,pObj->m_str );
					PC_ShowSysMsg( nIndex, strMSG );

					PC_ShowSysMsg( nIndex, pObj->m_strError );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
					return ;
				}
				else
				{
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NEXT );
					return ;
				}
				
			}
		}
		break;
	default:
		break;
	}


	return;
}

CNodeObj * CPublicCardObj::PC_ExcuteSQL(CNodeObj *pCurObj, CCFDocument *pDoc)
{
	CString strMSG("");
	CNewSQLExcuteObj *pObj = (CNewSQLExcuteObj*)pCurObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewSQLExcuteObj) ) )
	{
		strMSG.Format("ExcuteSQL Error, = Object is not valid" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );   
	}
	
	if( g_pD08a->CompareVarKey(0,pObj->m_nDBType) )
	//if( 0 == pObj->m_nDBType ) //��ͨ����������ݿ�
	{
		strMSG.Format("ExcuteSQL Can not use channel DB Connect" );
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}

	int nKey = pObj->m_nDBKey;
	if( -1 == nKey )
	{
		strMSG.Format("ExcuteSQL  %s Error = not have the var; ", pObj->m_strCaption);
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}

	//��ϵͳ��������ݿ�
	CNewDBConnVar *pDBVar = PC_FindSystemDBVar( nKey );
	if( NULL == pDBVar )
	{
		strMSG.Format("ExcuteSQL OpenDatabase  %s Error = not have the var", pObj->m_strCaption);
		PC_ShowSysMsg( 0, strMSG );
		return PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	
	CString strError="";
	BOOL bResult = pDBVar->ExcuteSQL( pObj->m_strSQL, strError);
	if( !bResult )
	{
		strMSG.Format("ExcuteSQL %s Error = %s  ; SQL = %s ", pObj->m_strCaption, strError, pObj->m_strSQL );
		PC_ShowSysMsg( 0, strMSG );
		return  PC_MoveNextObj(NEXTTYPE_ERROR,pCurObj,pDoc );
	}
	else
	{
		return  PC_MoveNextObj(NEXTTYPE_NEXT,pCurObj,pDoc );
	}


}

void CPublicCardObj::PC_DBBrowse( int nIndex, CFChannelInfo *pChnlObj )
{
	CString strMSG("");
	CDBBrowseObj *pObj = (CDBBrowseObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CDBBrowseObj) ) )
	{
		strMSG.Format("DBBrowse Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;   
	}


	if( -1 == pObj->m_nRecordsetKey)
	{
		strMSG.Format("DBBrowse  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj, NEXTTYPE_ERROR );
		return;
	}

	CNewRecordsetVar * pVar = pChnlObj->FindRecordsetVar( pObj->m_nRecordsetKey ) ;
	if( NULL == pVar )
	{
		strMSG.Format("DBBrowse  %s Error = not have the Recordset  var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return;
	}

	int nBrowseType = pObj->m_nBrowseType;
	try
	{
		if( pVar->m_pRecordset->adoEOF )
		{
			PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_EOF_FALSE );
			return;
		}
		
		switch( nBrowseType )
		{
		case 0://First
			pVar->m_pRecordset->MoveFirst();
			PC_DBBrowse_GetFieldValuse( nIndex,pChnlObj, pVar, pObj );
			break;
		case 1://Next
			PC_DBBrowse_GetFieldValuse( nIndex,pChnlObj, pVar, pObj );
			pVar->m_pRecordset->MoveNext();
			break;
		case 2://Last
			pVar->m_pRecordset->MoveLast();
			PC_DBBrowse_GetFieldValuse( nIndex,pChnlObj, pVar, pObj );
			break;
		case 3://Prev
			pVar->m_pRecordset->MovePrevious();
			PC_DBBrowse_GetFieldValuse( nIndex,pChnlObj, pVar, pObj );
			break;
		case 4://Current
			PC_DBBrowse_GetFieldValuse( nIndex,pChnlObj, pVar, pObj );
			break;
		case 5://!EOF==TRUE
			break;
		default:
			break;
		}

		PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_NEXT );

	}catch(_com_error e)///��׽�쳣
	{
		strMSG.Format("DBBrowse  %s Error = %s; ChIndex = %d ", pObj->m_strCaption, (LPTSTR)e.Description(), nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;
	} 
	

	return;
}


void CPublicCardObj::PC_DBBrowse_GetFieldValuse(int nIndex, CFChannelInfo *pChnlObj,CNewRecordsetVar * pRS, CDBBrowseObj *pObj)
{
	CString strMSG("");
	try
	{
		
		CNodeDataList *pDadaList = &(pObj->m_NodeDataList);
		POSITION pos = pDadaList->GetHeadPosition( );
		while ( pos )
		{
			_variant_t vFieldValues;
			CString strValues("");
			CNodeData *pVar = pDadaList->GetNext(pos);
			if( NULL != pVar )
			{
				vFieldValues = pRS->m_pRecordset->GetCollect( (_bstr_t)pVar->m_strName );
				strValues =  (LPCTSTR)(_bstr_t)vFieldValues ;
				strValues.TrimRight();
				PC_SaveValuesToVar( nIndex,pChnlObj,  strValues, pVar->m_nKey );

			}
		}

	}catch(_com_error e)///��׽�쳣
	{
		strMSG.Format("DBBrowse  %s Error = %s; ChIndex = %d ", pObj->m_strCaption, (LPTSTR)e.Description(), nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;
	} 
	return;
}

BOOL CPublicCardObj::PC_SaveValuesToVar(int nIndex, CFChannelInfo *pChnlObj, CString strValues, int nVarKey)
{
	if( nVarKey < 0 ) return FALSE;
	if( g_pD08a->CompareXiaoYu( nVarKey, 51 ) )
	//if( nVarKey <= 50 )
	{
		//����
		return FALSE;
	}
	else if( nVarKey > 50 && nVarKey < 100 )
	{
		//ϵͳ����
		return PC_SaveValuesToSysVar(strValues, nVarKey );
	}
	else if( nVarKey >= 100 && nVarKey < 4000 )
	{
		//��ͨ����
		return pChnlObj->SaveToNorVar(strValues, nVarKey );
	}
	else
	{
		//ģ���������
		return pChnlObj->SaveToModuleParam( strValues, nVarKey );
	}

}

BOOL CPublicCardObj::PC_SaveValuesToSysVar( CString strValues, int nVarKey)
{

	POSITION pos  = m_pSysNormalVarList->GetHeadPosition();
	while ( pos )
	{
		CNormalVar * pVar = m_pSysNormalVarList->GetNext(pos);
		if( pVar != NULL )
		{
			if( nVarKey == pVar->m_nKey )
			{
				g_Section.Lock();
				pVar->m_strVariable = strValues ;
				g_Section.Unlock();
				return TRUE ;
			}
		}
	}

	return FALSE;
}


void CPublicCardObj::PC_Block(int nIndex ,CFChannelInfo *pChnlObj )
{
	CString strMSG("");
	CNewBlockObj *pObj = (CNewBlockObj*)pChnlObj->GetCurrentNodeObj();
	if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewBlockObj) ) )
	{
		strMSG.Format("Block Error; ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;   
	}

	CNodeObj *pNewObj = pObj->m_ChildObjLst.GetHead();
	if( NULL == pObj )
	{
		strMSG.Format("Block Error; ChIndex = %d   The first is NULL ", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;
	}
	
	if( NODETYPE_ENTER != pNewObj->m_nType )
	{
		strMSG.Format("Block Error; ChIndex = %d   Block first node is not enter node ", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;
	}
	pChnlObj->SetCurrentNodeObj( pNewObj );
	return;
}

void CPublicCardObj::PC_Enter(int nIndex,CFChannelInfo *pChnlObj )
{
	PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_NEXT );
	return;
}

BOOL CPublicCardObj::PC_TransferToIVR(int nIndex, int NodeKey,CFChannelInfo *pChnlInfo )
{
	CString strMSG("");

	CNodeObj *pAimObj =  pChnlInfo->FindNodeObj( NodeKey);
	if( NULL == pAimObj )
	{
		return FALSE ;  
	}

	while(1)
	{
		DWORD   dwExitCode = 0;
		GetExitCodeThread( pChnlInfo->m_Threadhandle, &dwExitCode );
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

	pChnlInfo->SetCurrentNodeObj( pAimObj );
	pChnlInfo->m_nStep = 0;
	PC_ShowChannelBlock( nIndex, pAimObj->m_strCaption );


	return TRUE;
}
void CPublicCardObj::PC_GoTo(int nIndex,CFChannelInfo *pChnlInfo )
{
	CString strMSG("");
	if( pChnlInfo->m_nStep > 0 ) return;
	CGoToObj *pObj = (CGoToObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CGoToObj) ) )
	{
		strMSG.Format("GoTo error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,NEXTTYPE_ERROR);
		return ;  
	}

	int nGoToKey = pObj->m_nNextObjKey;
	CNodeObj *pAimObj =  pChnlInfo->FindNodeObj( nGoToKey);
	if( NULL == pAimObj )
	{
		strMSG.Format("%s GoTo Error ,Have not Aim Node;ChIndex = %d", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,NEXTTYPE_ERROR);
		return ;  
	}
	pChnlInfo->SetCurrentNodeObj( pAimObj );
	PC_ShowChannelBlock( nIndex, pAimObj->m_strCaption );


	return;
}
void CPublicCardObj::PC_Return(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CNodeObj *pObj = pChnlObj->GetCurrentNodeObj();
    if ( NULL == pObj )
	{
		strMSG.Format("Return Error; ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;   
	}

	int nReturnKey = pObj->m_nKey;
	//ͨ��return��m_nBlockKeyֵ�ҵ�������Block
	pChnlObj->SetCurrentNodeObj(  pChnlObj->FindNodeObj( pObj->m_nBlockKey ) );
	PC_MoveNext_Node( nIndex,pChnlObj,nReturnKey );
	return;
}

void CPublicCardObj::PC_Compare(int nIndex,CFChannelInfo *pChnlObj)
{
	CString strMSG("");
	CCompareObj *pObj = (CCompareObj*)pChnlObj->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CCompareObj) ) )
	{
		strMSG.Format("Compare Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlObj, NEXTTYPE_ERROR );
		return ;   
	}

	CString strVarValuse("");
	BOOL bResult = PC_GetNorVarValues( pChnlObj,strVarValuse, pObj->m_nVarKey );
    if( !bResult )
	{
		strMSG.Format("Compare  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
		return ;  
	}
	

	strMSG.Format("Compare  %s Error = Campare Error; ChIndex = %d ", pObj->m_strCaption, nIndex );

	POSITION pos = pObj->m_NextNodeList.GetHeadPosition();
	int nType = 0;
	CString strExpression="";
	while( pos )
	{
		CNextNode * pNextObj =pObj->m_NextNodeList.GetNext(pos);
		if( NULL == pNextObj  ) continue;
		nType = pNextObj->m_nType;
		if( !PC_GetExpressValues( pChnlObj, strExpression, pNextObj->m_strData) )
		{
			strMSG.Format("Compare  %s Error = Get Express values error; ChIndex = %d ", pObj->m_strCaption, nIndex );
			PC_ShowSysMsg( nIndex, strMSG );
			PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
			return ;  
		}

		if( 50 == nType )//==
		{
			if( strVarValuse == strExpression )
			{
				if( !PC_MoveNext_Node( nIndex, pChnlObj,pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				}
				return;
			}
		}
		else if( 51 == nType )//!=
		{
			if( strVarValuse != strExpression )
			{
				if( !PC_MoveNext_Node( nIndex, pChnlObj,pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				}
				return;
			}
		}
		else if( 52 == nType )//<
		{
			if( atof(strVarValuse) < atof( strExpression ) )
			{
				if( !PC_MoveNext_Node( nIndex,pChnlObj, pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				}
				return;
			}
		}
		else if( 53 == pNextObj->m_nType )//<=
		{
			if( atof(strVarValuse) <= atof(strExpression) )
			{
				if( !PC_MoveNext_Node( nIndex, pChnlObj,pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				}
				return;
			}
		}
		else if( 54 == nType )//>
		{
			if( atof(strVarValuse) > atof(strExpression) )
			{
				if( !PC_MoveNext_Node( nIndex,pChnlObj, pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_ERROR );
				}
				return;
			}
		}
		else if( 55 == nType )//>=
		{
			if( atof(strVarValuse) >= atof(strExpression) )
			{
				if( !PC_MoveNext_Node( nIndex,pChnlObj, pNextObj ) )
				{
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex,pChnlObj,  NEXTTYPE_ERROR );
				}
				return;
			}
		}

	}//end while
	

	//û��ƥ���
	PC_MoveNext_Node( nIndex, pChnlObj, NEXTTYPE_NOMATCH );

	return;
}


void CPublicCardObj::PC_ShowFaxUpdate(int nFaxIndex ,CFaxStateList *pFaxChannel )
{

	if( NULL == m_plstFax ) return;
	CString strMSG("");
	if( !IsWindow( m_plstFax->GetSafeHwnd() ) ) return;
	int nState = pFaxChannel->m_nStatus;
	if( CIF_FAX_FREE == nState )
	{
		m_plstFax->SetItemText(nFaxIndex,1,"Free");
	}
	else if( CIF_FAX_RECEIVE == nState )
	{
		m_plstFax->SetItemText(nFaxIndex,1,"Reciveing");
	}
	else if( CIF_FAX_SEND == nState )
	{
		m_plstFax->SetItemText(nFaxIndex,1,"Sending");
	}
	
	strMSG.Format("%d",pFaxChannel->m_nCurrPage );
	m_plstFax->SetItemText(nFaxIndex,2,strMSG);
	m_plstFax->SetItemText(nFaxIndex,3,pFaxChannel->m_strFaxFileName);

	return;
}

/////////////////////////////////////////////////
//�ú�������nKey�Ĵ�С���ж��Ƿ���ͨ������DLL,
//����ϵͳ����DLL, nKey < 100Ϊϵͳ���ġ�
//
///////////////////////////////////////////////////
CDLLVariable * CPublicCardObj::PC_FindDLLVariable( CFChannelInfo *pChnlInfo, int nKey)
{
	CDLLVariable *pDLLVar = NULL;
	if( nKey > 100 )
	{
		//ϵͳ����
		pDLLVar = pChnlInfo->FindDLLVariable( nKey );
	}
	else
	{
		//ͨ������DLL
		POSITION pos = m_pSysDLLVarList->GetHeadPosition();
		while(pos)
		{
			CDLLVariable * pVar = m_pSysDLLVarList->GetNext(pos);
			if( pVar != NULL )
			{
				if( nKey == pVar->m_nKey )
				{
					pDLLVar = pVar ;
					break;
				}
			}
		}


	}
	return pDLLVar;
}

void CPublicCardObj::PC_StartTimer( CFChannelInfo *pChnlInfo )
{
	//GetTickCount()ȡ��ϵͳ���������ڵ�ʱ�䣬����Ϊ��λ
	//��49.7����ת��0
	//1�������һǧ��֮һ��
	pChnlInfo->m_lTimeElapse =GetTickCount();//

}
/////////////////////////////////
long CPublicCardObj::PC_ElapseTime( CFChannelInfo *pChnlInfo )
{
	//GetTickCount()ȡ��ϵͳ���������ڵ�ʱ�䣬����Ϊ��λ
	//��49.7����ת��0
	//1�������һǧ��֮һ��
	long lTimeElapse = abs( GetTickCount() - pChnlInfo->m_lTimeElapse) ;
	if( 0 != lTimeElapse ) lTimeElapse = lTimeElapse/1000;
	return lTimeElapse;
}

void CPublicCardObj::PC_StoredProcedures(int nIndex, CFChannelInfo *pChnlInfo )
{
	CString strMSG("");
	CNewStoreProObj *pObj = (CNewStoreProObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewStoreProObj) ) )
	{
		strMSG.Format("Excute Stored Procedures Error, ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
		return ;   
	}
	
	int nStep = pChnlInfo->m_nStep;
	switch( nStep )
	{
	case 0:
		{
			
			if( -1 == pObj->m_nDBKey )
			{
				strMSG.Format("Excute Stored Procedures  %s Error = not have the var; ChIndex = %d ", pObj->m_strCaption, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			CNewDBConnVar *m_pDB = NULL;
			if( g_pD08a->CompareVarKey( 0, pObj->m_nDBType ) )
			//if( 0 == pObj->m_nDBType ) //��ͨ����������ݿ�
				m_pDB = pChnlInfo->FindDBConnectVar( pObj->m_nDBKey );
			else//��ϵͳ��������ݿ�
				m_pDB = PC_FindSystemDBVar( pObj->m_nDBKey );

			if( NULL == m_pDB )
			{
				strMSG.Format("Excute Stored Procedures  %s Error = not have the DB var; ChIndex = %d ", pObj->m_strCaption, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}
			if( !m_pDB->m_bConnect )
			{
				strMSG.Format("[Excute Stored procedure %s Error ChnlIndex:%d][Database connect is closed]",pObj->m_strStoreProName,nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return ;
			}

			CNewRecordsetVar *pRS  = pChnlInfo->FindRecordsetVar( pObj->m_nRecordsetKey ) ;
			if( NULL == pRS )
			{
				strMSG.Format("Excute Stored Procedures  %s Error = not have the Recordset  var; ChIndex = %d ", pObj->m_strCaption, nIndex );
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
				return;
			}
			

			pObj->m_pDB = m_pDB;
			pObj->m_pRS = pRS;
			pObj->m_pChnlInfo = pChnlInfo;

			//����ִ��SQL�����߳�
			CWinThread* pCallThread = AfxBeginThread(ExcuteStorePro_Progress, pObj,  THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			::DuplicateHandle(GetCurrentProcess(), pCallThread->m_hThread,  GetCurrentProcess()  , &pChnlInfo->m_Threadhandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
			ResumeThread(pChnlInfo->m_Threadhandle);
			pChnlInfo->m_nStep = 1;
		}
		break;
	case 1:
		{

			DWORD   dwExitCode = 0;
			GetExitCodeThread( pChnlInfo->m_Threadhandle, &dwExitCode );
			if( STILL_ACTIVE == dwExitCode )
			{
				return;
			}
			else
			{
				if( 0 != dwExitCode )
				{
					strMSG.Format("[%s][Excute Stored Procedures][ExitCode:%d]",pObj->m_strCaption, dwExitCode );
					PC_ShowSysMsg( nIndex, strMSG );

					PC_ShowSysMsg( nIndex, pObj->m_strError );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return ;
				}
				else
				{
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_NEXT );
					return ;
				}
				
			}
		}
		break;
	default:
		break;
	}


	return;
}

//Express( <SaveTo> ; <Variable> )");
void CPublicCardObj::PC_Cus_Express(int nIndex,CFChannelInfo *pChnlInfo,CCustomObj *pObj )
{
	CString strMSG("");
	CString strFunStr = pObj->m_strFunName; 
    if ("" == strFunStr  )
	{
		strMSG.Format("Custom Express  error : ChIndex = %d ", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}

	try
	{		
			CString str = strFunStr;
			str.Replace("Express",NULL);
			CString str1("");
			CString str2("");
			CString str3("");
			CString str4("");
			if( !PC_DivideString( str,";",str1,str2,str3,str4) )
			{
				strMSG.Format("%s  Custom Express is error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			
			//ͨ��������ʽ�ı������������жϱ��ʽ�ļ��㷽ʽ
			CString strExpressValues(""); 
			int nVarType = 0;
			int nVarKey = pObj->GetVarKey( str1 );
			if( nVarKey <= 50 )
			{
				//����ֵ���ܱ��浽������
				strMSG.Format("%s Can not save the Express Values to constant ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			if( !PC_GetNorVarValues( pChnlInfo,strExpressValues, nVarType, nVarKey ) )
			{
				//û���ҵ��ñ���
				strMSG.Format("%s The Express Values Variable is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			
			if( DATATYPE_INTEGER == nVarType )//interger
			{
				int nResult=0;
				if( PC_Express_Integer(nIndex,pChnlInfo,str2,  nResult) )
					strExpressValues.Format("%d",nResult);
				else{
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
					return;
				}
					
			}
			else if( DATATYPE_FLOAT == nVarType )//Double
			{
				double dResult = 0.0;
				if( PC_Express_Double( nIndex,pChnlInfo,str2,  dResult) )
					strExpressValues.Format("%f",dResult);
				else{
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return;
				}
					
			}
			else//��������,�����ַ���������
			{
				if( !PC_Express_Str( nIndex,pChnlInfo,str2,  strExpressValues) ){
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return;
				}

			}

			//����ɹ���������ʽ
			if( PC_SaveValuesToVar( pChnlInfo, strExpressValues, nVarKey ) )
			{
				pObj->MoveNextFun();
				return;
			}
			else
			{
				strMSG.Format("%s Save the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
			}
	}
	catch(...)
	{
		strMSG.Format("%s the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
	}

	return;
}

void CPublicCardObj::PC_Express(int nIndex,CFChannelInfo *pChnlInfo)
{
	CString strMSG("");
	CExpressObj *pObj = (CExpressObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CExpressObj) ) )
	{
		strMSG.Format("Express error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}

	try
	{
			//ͨ��������ʽ�ı������������жϱ��ʽ�ļ��㷽ʽ
			CString strExpressValues(""); 
			int nVarType = 0;
			int nVarKey = pObj->GetVarKey( pObj->m_strExpressValues);
			if( g_pD08a->CompareXiaoYu(nVarKey, 51) )
			//if( nVarKey <= 50 )
			{
				//����ֵ���ܱ��浽������
				strMSG.Format("%s Can not save the Express Values to constant ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			if( !PC_GetNorVarValues( pChnlInfo,strExpressValues, nVarType, nVarKey ) )
			{
				//û���ҵ��ñ���
				strMSG.Format("%s The Express Values Variable is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
				return;
			}

			if( g_pD08a->CompareVarKey(DATATYPE_INTEGER, nVarType) )
			//if( DATATYPE_INTEGER == nVarType )//interger
			{
				int nResult=0;
				if( PC_Express_Integer(nIndex,pChnlInfo,pObj->m_strExpress,  nResult) )
					strExpressValues.Format("%d",nResult);
				else{
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
					return;
				}
					
			}
			else if( DATATYPE_FLOAT == nVarType )//Double
			{
				double dResult = 0.0;
				if( PC_Express_Double( nIndex,pChnlInfo,pObj->m_strExpress,  dResult) )
					strExpressValues.Format("%f",dResult);
				else{
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return;
				}
					
			}
			else//��������,�����ַ���������
			{
				if( !PC_Express_Str( nIndex,pChnlInfo,pObj->m_strExpress,  strExpressValues) ){
					strMSG.Format("%s Calculating the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return;
				}

			}

			//����ɹ���������ʽ
			if( PC_SaveValuesToVar( pChnlInfo, strExpressValues, nVarKey ) )
			{
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_NEXT);
			}
			else
			{
				strMSG.Format("%s Save the Values of the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
			}
	}
	catch(...)
	{
		strMSG.Format("%s the Express  is Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
	}

	return;
}
BOOL CPublicCardObj::PC_Express_Double(int nIndex,CFChannelInfo *pChnlInfo,CString strExpress, double &dResult)
{
	int nPos1	= -1;
	int nPos2	= -1;
	int nPos3	= -1;
	int nVarKey = -1;

	BOOL bResult = TRUE;
	CString strVar("");
	CString strVarValues("");
	
	
	CString strOldExpress = strExpress;
	strOldExpress.Replace( "\"" , NULL );
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
		if( !PC_GetNorVarValues( pChnlInfo, strVarValues, nVarKey ) )
		{
			//û���ҵ��ñ���
			bResult = FALSE;
			break;
		}
		if( "" == strVarValues )
			strNewExpress = strNewExpress + "0";
		else
			strNewExpress = strNewExpress + strVarValues;

	}//end while

	if( bResult )
	if( "" == strNewExpress ) 
	{
		//����û�б���
		strNewExpress = strOldExpress;
	}
	
	strNewExpress.Replace(" ",NULL);
	CCalculateExpress  CalEx;
	return CalEx.calculate( (LPTSTR)(LPCTSTR)strNewExpress, dResult );

}

BOOL CPublicCardObj::PC_Express_Integer(int nIndex,CFChannelInfo *pChnlInfo,CString strExpress, int &nResult)
{
	int nPos1	= -1;
	int nPos2	= -1;
	int nPos3	= -1;
	int nVarKey = -1;

	BOOL bResult = TRUE;
	CString strVar("");
	CString strVarValues("");
	
	
	CString strOldExpress = strExpress;
	strOldExpress.Replace( "\"" , NULL );
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
		if( !PC_GetNorVarValues( pChnlInfo, strVarValues, nVarKey ) )
		{
			//û���ҵ��ñ���
			bResult = FALSE;
			break;
		}
		if( "" == strVarValues )
			strNewExpress = strNewExpress + "0";
		else
			strNewExpress = strNewExpress + strVarValues;

	}//end while

	if( bResult )
	if( "" == strNewExpress ) 
	{
		//����û�б���
		strNewExpress = strOldExpress;
	}

	CCalculateExpress  CalEx;
	strNewExpress.Replace(" ",NULL);
	double dResult=0;
	if( CalEx.calculate( (LPTSTR)(LPCTSTR)strNewExpress, dResult )  )
	{
		nResult = (int)dResult;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_Express_Str(int nIndex,CFChannelInfo *pChnlInfo,CString strExpress, CString &strResult)
{
	int nPos1	= -1;
	int nPos2	= -1;
	int nPos3	= -1;
	int nVarKey = -1;

	BOOL bResult = TRUE;
	CString strVar("");
	CString strVarValues("");
	
	
	CString strOldExpress = strExpress;
	strOldExpress.Replace( "\"" , NULL );
	strOldExpress.Replace( " " , NULL );
	strOldExpress.Replace( "*" , NULL );
	strOldExpress.Replace( "%" , NULL );
	strOldExpress.Replace( "+" , NULL );
	strOldExpress.Replace( "-" , NULL );
	strOldExpress.Replace( "/" , NULL );
	strOldExpress.Replace( "(" , NULL );
	strOldExpress.Replace( ")" , NULL );

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
		if( !PC_GetNorVarValues( pChnlInfo, strVarValues, nVarKey ) )
		{
			//û���ҵ��ñ���
			bResult = FALSE;
			break;
		}
		strNewExpress = strNewExpress + strVarValues;

	}//end while

	if( bResult )
	if( "" == strNewExpress ) 
	{
		//����û�б���
		strNewExpress = strOldExpress;
	}

	strResult = strNewExpress;

	return TRUE;
}



void CPublicCardObj::PC_ClearVoxFileEndDTMF(LPCTSTR lpszFileName)
{
	if( NULL == lpszFileName ) return;
	TRY
	{
		CFile voxFile;
		CFileException e;
		int nResult = 0;
		nResult = voxFile.Open(lpszFileName,CFile::modeReadWrite,&e);
		if( 0 == nResult )
			return ;
		else
		{
			DWORD len = voxFile.GetLength();
			if(  (len - 2048) <= 0 )
				voxFile.SetLength(0);
			else
				voxFile.SetLength(len - 2048);
		}
	}
	CATCH( CFileException, e )
	{
		return ;
	}
	END_CATCH
}

void CPublicCardObj::PC_Function(int nIndex, CFChannelInfo *pChnlInfo,int nTotalChnl ,CFChannelInfo *pArrChnlInfo)
{
	CString strMSG("");
	CFunctionObj *pObj = (CFunctionObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CFunctionObj) ) )
	{
		strMSG.Format("The Function error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}

	try
	{
			CString strOldExpress = pObj->m_strFunction;
			strOldExpress.Replace( "\"" , NULL );
			strOldExpress.Replace(" ", NULL);
			CString strNewExpress = "";
			CString strReturn("");

			if( strOldExpress.Left(15) == "GetLinkChnlVar(" )
			{
				int nLink = pChnlInfo->m_nLinkCh;
				if( nLink >=0 && nLink < nTotalChnl )
				{
					CFChannelInfo *pLinkChnlInfo = &(pArrChnlInfo[nLink]);
					if( PC_GetExpressValues( pLinkChnlInfo,strNewExpress,strOldExpress) )
					{
						strNewExpress.Replace( "GetLinkChnlVar(", NULL);
						strNewExpress.Replace( ")", NULL);
						strReturn=strNewExpress;
						//���淵��ֵ
						int nVarKey1 = pObj->GetVarKey( pObj->m_strReturnValues);
						if( PC_SaveValuesToVar(pChnlInfo, strReturn, nVarKey1 ) )
						{
							PC_MoveNext_Node( nIndex,  pChnlInfo,NEXTTYPE_NEXT);
							return;
						}
						else
						{
							strMSG.Format("%s GetLinkChnlVar: can't Save the return Values ;ChIndex = %d", pObj->m_strCaption, nIndex);
							PC_ShowSysMsg( nIndex, strMSG );
							PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
							return;
						}
						
					}
					else
					{
						strMSG.Format("%s GetLinkChnlVar: get not variable values ;ChIndex = %d", pObj->m_strCaption, nIndex);
						PC_ShowSysMsg( nIndex, strMSG );
						PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
						return;
					}
				}
				else
				{
					strMSG.Format("%s GetLinkChnlVar:it has not link channel ;ChIndex = %d", pObj->m_strCaption, nIndex);
					PC_ShowSysMsg( nIndex, strMSG );
					PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
					return;
				}

			}

			PC_GetExpressValues( pChnlInfo,strNewExpress,strOldExpress);

			strNewExpress.Replace(" ", NULL);
			CString str("");
			CString str1("");
			CString str2("");

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
				
				i = PC_FindUerChannel( str1 , nTotalChnl ,pArrChnlInfo);
				strReturn.Format( "%d",i );
			}
			else if( strNewExpress.Left(12) == "GetHoldChnl(" )
			{
				
				int nHold = PC_GetHoldChnl( nIndex , nTotalChnl ,pArrChnlInfo);
				strReturn.Format( "%d",nHold );
			}
			else if( strNewExpress.Left(13) == "GetFreeAgent(" )
			{
				
				int nHold = PC_FindFreeChannel( CF_CHTYPE_USER , nTotalChnl ,pArrChnlInfo);
				strReturn.Format( "%d",nHold );
			}
			else if( strNewExpress.Left(16) == "GetLinkChnlType(" )
			{
				
				int nLink = pChnlInfo->m_nLinkCh;
				if( nLink >=0 && nLink < nTotalChnl )
				{
					CFChannelInfo *pLinkChnlInfo = &(pArrChnlInfo[nLink]);
					strReturn.Format( "%d",pLinkChnlInfo->m_nChanelType );
				}
				else
				{
					strReturn="0";
				}
				
				
			}
			
			//���淵��ֵ
			int nVarKey1 = pObj->GetVarKey( pObj->m_strReturnValues);
			if( PC_SaveValuesToVar(pChnlInfo, strReturn, nVarKey1 ) )
			{
				PC_MoveNext_Node( nIndex,  pChnlInfo,NEXTTYPE_NEXT);
			}
			else
			{
				strMSG.Format("%s can't Save the return Values of the function ;ChIndex = %d", pObj->m_strCaption, nIndex);
				PC_ShowSysMsg( nIndex, strMSG );
				PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_ERROR );
			}

		
	}
	catch(...)
	{
		strMSG.Format("%s the Express  is catch Error ;ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMSG );
		PC_MoveNext_Node( nIndex,pChnlInfo,  NEXTTYPE_ERROR );
	}

	return;
}

int CPublicCardObj::PC_GetHoldChnl(int nIndex,int nTotalChnl, CFChannelInfo *pArrChnlInfo)
{
	CFChannelInfo *pChnlInfo = &pArrChnlInfo[nIndex];
	int nLinkIndex = -1;
	for( int i = 0; i < nTotalChnl; i++ )
	{
		if( nIndex == pChnlInfo->m_nLinkCh )
		{
			nLinkIndex = i;
			break;
		}
	}
	return nLinkIndex;
}

int CPublicCardObj::PC_FindUerChannel(CString strLocalDN, int nTotalChnl, CFChannelInfo *pArrChnlInfo)
{
	for( int i = 0; i < nTotalChnl; i++ )
	{
		if( strLocalDN == pArrChnlInfo[i].m_strLocalDN && CF_CHTYPE_USER == pArrChnlInfo[i].m_nChanelType )
		{
			return i;
		}
	}
	return -1;
}

int CPublicCardObj::PC_FindFreeChannel(int nChnlType, int nTotalChnl, CFChannelInfo *pArrChnlInfo)
{
	static int nCurrentTrunkNo = 0 ;
	int i = 0;
	for( i = nCurrentTrunkNo ; i < nTotalChnl ; i++ ) 
	{
		if( nChnlType == pArrChnlInfo[i].m_nChanelType && CHANNEL_STATE_FREE == pArrChnlInfo[i].m_nState )
		{

			nCurrentTrunkNo = i+1 ;
			return i;

		}
	}
	
	for( i = 0 ; i < nCurrentTrunkNo ; i++ )
	{
		if( nChnlType == pArrChnlInfo[i].m_nChanelType && CHANNEL_STATE_FREE == pArrChnlInfo[i].m_nState )
		{

			nCurrentTrunkNo = i+1 ;
			return i;

		}
	}

	return -1 ;
}



bool CPublicCardObj::PC_OpenLogFile(void)
{
	TRY
	{
		m_nIsSaveSysLog = GetPrivateProfileInt("SystemOption","SaveSysLog",0,"IvrSystem\\ChiefIVRSet.ini");
		if( m_nIsSaveSysLog > 0  )
		{
			CString csFileName = "";
			csFileName = theApp.m_strPath + "\\IvrSyslog\\Sys" + (CTime::GetCurrentTime()).Format("IVR%Y%m%d%H%M%S") + ".txt" ;
			int nResult = 0;
			nResult = m_ErrorLogFile.Open((LPCTSTR )csFileName,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyNone);
			if( 0 == nResult )
			{
				m_nIsSaveSysLog = 0;
				return false;
			}
			else
			{
				m_nIsSaveSysLog = 1;
				return true;
			}
		}
		else
		{
				m_nIsSaveSysLog = 0;
				return false;
		}
	}
	CATCH( CFileException, e )
	{
		m_nIsSaveSysLog = 0;
		return false;
	}
	END_CATCH
}

void CPublicCardObj::PC_WriteSysLog(LPCTSTR lpsz)
{
	
	if( m_nIsSaveSysLog <= 0  ) return;
	TRY
	{
		if(m_ErrorLogFile.GetLength() > 402400 )
		{
			m_ErrorLogFile.Close();
			PC_OpenLogFile();
		}
		CString csTemp = "";
		csTemp.Format("\t\n%s",lpsz);
		m_ErrorLogFile.WriteString( (LPCTSTR) csTemp );
	}
	CATCH( CFileException, e )
	{
		return ;
	}
	END_CATCH


}

void CPublicCardObj::PC_CTI_OffHookMsg(int nIndex, CFChannelInfo *pChnlInfo)
{
	//����ժ����Ϣ
	CString strMSG("");
	strMSG.Format("[Send OffHook DN:%s]",pChnlInfo->m_strLocalDN);
	PC_ShowCommMsg(strMSG);
	CTI_OffHookMsg( atoi(pChnlInfo->m_strLocalDN) );//����

	return;
}

void CPublicCardObj::PC_CTI_HangUpMsg(int nIndex, CFChannelInfo *pChnlInfo)
{
	//���͹һ���Ϣ
	CString strMSG("");
	strMSG.Format("[Send HangUP DN:%s]",pChnlInfo->m_strLocalDN);
	PC_ShowCommMsg(strMSG);
	CTI_HangUpMsg( atoi(pChnlInfo->m_strLocalDN) );
	return;

}

void CPublicCardObj::PC_CTI_RingingMsg(int nDN, const char *pszTel, const char *pszTel2)
{
	//����������Ϣ
	CString strMSG("");
	strMSG.Format("[Send Ringing DN:%d][CallerID:%s][CalledID:%s]", nDN, pszTel ,pszTel2 );
	PC_ShowCommMsg(strMSG);
	CTI_RingingMsg( nDN,pszTel, pszTel2 );

	return;
}

void CPublicCardObj::PC_CTI_MakeCallResponseMsg(int nDN, int nResult, const char *pszTel)
{
	//����MakeCall Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send MakeCall Response DN:%d][Tel:%s][Result:%s]", nDN, pszTel, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_MakeCallMsg( nDN, nResult, pszTel);

	return;
}

void CPublicCardObj::PC_CTI_PickupHoldResponseMsg(int nDN, int nResult,const char *pszCallerID, const char *pszCalledID )
{
	//����MakeCall Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send PickupHold Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_PickupHoldMsg( nDN, nResult ,pszCallerID, pszCalledID);

	return;
}

void CPublicCardObj::PC_CTI_HoldResponseMsg(int nDN, int nResult)
{
	//����Hold Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Hold Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_HoldMsg( nDN, nResult );

	return;
}

void CPublicCardObj::PC_CTI_MonitorResponseMsg(int nDN, int nResult)
{
	//����Monitor Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Monitor Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_MonitorMsg( nDN, nResult,"");

	return;
}

void CPublicCardObj::PC_CTI_AnswerCallResponseMsg(int nDN, int nResult)
{
	//����AnswerCall Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send AnswerCall Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_AnswerCallMsg( nDN, nResult );

	return;
}


void CPublicCardObj::PC_CTI_DisMonitorResponseMsg(int nDN, int nResult)
{
	//����DisMonitor Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send DisMonitor Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_DisMonitorMsg( nDN, nResult);

	return;
}
void CPublicCardObj::PC_CTI_GetActiveCallResponseMsg(int nDN, int nResult,const char *pszCallerID, const char *pszCalledID )
{
	//����GetActiveCall Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send GetActiveCall Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_GetActiveCallMsg( nDN , nResult,pszCallerID, pszCalledID);

	return;
}

void CPublicCardObj::PC_CTI_CancelTransferResponseMsg(int nDN, int nResult, char *pszTel)
{
	//���� Cancel Transfer Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Cancel Transfer Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_CancelTransferMsg( nDN, nResult,"");

	return;
}

void CPublicCardObj::PC_CTI_TransferResponseMsg(int nDN, int nResult, char *pszTel)
{
	//����Transfer Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Transfer Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_TransferMsg( nDN, nResult,"");

	return;
}

void CPublicCardObj::PC_CTI_LoginResponseMsg(int nDN , int nResult )
{
	//����Login Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Login Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_Login( nDN, nResult);
	return;
}

void CPublicCardObj::PC_CTI_LogOutResponseMsg(int nDN , int nResult )
{
	//����LogOut Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send LogOut Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_LogOut( nDN, nResult);
	return;
}

void CPublicCardObj::PC_CTI_ReadyResponseMsg(int nDN , int nResult )
{
	//���� Ready Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Ready Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_Ready( nDN, nResult);
	return;
}

void CPublicCardObj::PC_CTI_NotReadyResponseMsg(int nDN , int nResult )
{
	//���� NotReady Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send NotReady Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_NotReady( nDN, nResult);
	return;
}

void CPublicCardObj::PC_TraceInfo(int nIndex, CFChannelInfo *pChnlInfo)
{
	CString strMsg("");
	CTraceObj *pObj = (CTraceObj*)pChnlInfo->GetCurrentNodeObj();
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CTraceObj) ) )
	{
		strMsg.Format("TraceInfo error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMsg );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}

	CString strResult("");
	if( !PC_Express_Str( nIndex, pChnlInfo,pObj->m_strTrace, strResult ) )
	{
		strMsg.Format("%s  TraceInfo Error; ChIndex = %d", pObj->m_strCaption, nIndex);
		PC_ShowSysMsg( nIndex, strMsg );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}
	else
	{
		strMsg.Format("%s  ChIndex = %d TRACE=%s", pObj->m_strCaption, nIndex,strResult);
		PC_ShowSysMsg( nIndex, strMsg );
		PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_NEXT );
	}
}

void CPublicCardObj::PC_Moudle(int nIndex, CFChannelInfo *pChnlInfo)
{
	CString strMsg("");
	CNewModuleObj *pObj = (CNewModuleObj*)pChnlInfo->m_pCurrentNodeObj;
    if (pObj == NULL || !pObj->IsKindOf( RUNTIME_CLASS(CNewModuleObj) ) )
	{
		strMsg.Format("Module error : ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMsg );
		PC_MoveNext_Node( nIndex,pChnlInfo, NEXTTYPE_ERROR);
		return ;  
	}


	CParameterList *pParamList = &(pObj->m_ParameterLst);
	POSITION pos = pParamList->GetHeadPosition();
	while(pos)
	{
		CParameter * pParam = pParamList->GetNext(pos);
		if( pParam != NULL )
		{
			int nKey = pObj->GetVarKey( pParam->m_strVariable );
			if( nKey >= 100 && nKey < 4000 )
			{
				pChnlInfo->InPutVarValuesToParam( pParam, nKey );
			}
		}
	}

	PC_MoveNext_Node( nIndex, pChnlInfo, NEXTTYPE_NEXT );

	return;
}



void CPublicCardObj::PC_MoudleEnd(int nIndex, CFChannelInfo *pChnlInfo)
{
	CString strMSG("");
	CNodeObj *pObj = pChnlInfo->GetCurrentNodeObj();
    if ( NULL == pObj )
	{
		strMSG.Format("ModuleEnd Error; ChIndex = %d   Error = Object is not valid", nIndex );
		PC_ShowSysMsg( nIndex, strMSG );
		return ;   
	}

	int nNextType = pObj->m_nKey;
	pChnlInfo->MoveNext_Node(nNextType );

	return;
}



BOOL CPublicCardObj::PC_DivideString(CString strOld, LPCTSTR lpszSub,CString &str1, CString &str2,CString &str3,CString &str4,CString &str5)
{
	if( "" == strOld || NULL == lpszSub || "" == lpszSub ){
		return FALSE;
	}
	try
	{
		str1="";
		str2="";
		str3="";
		str4="";
		str5="";
		
		CString strFunStr = strOld;
		strFunStr.Replace(" ",NULL);
		strFunStr.Replace("(",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace("\"",NULL);
		int i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			return FALSE;
		}
		str1 = strFunStr.Mid(0,i ); 
		
		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str2 = strFunStr;
			return TRUE;
		}
		str2 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str3 = strFunStr;
			return TRUE;
		}
		str3 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str4 = strFunStr;
			return TRUE;
		}
		str4 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str5 = strFunStr;
			return TRUE;
		}
		str5 = strFunStr.Mid(0,i ); 

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_DivideString(CString strOld, LPCTSTR lpszSub,CString &str1, CString &str2,CString &str3,CString &str4)
{
	if( "" == strOld || NULL == lpszSub || "" == lpszSub ){
		return FALSE;
	}
	try
	{
		str1="";
		str2="";
		str3="";
		str4="";

		
		CString strFunStr = strOld;
		strFunStr.Replace(" ",NULL);
		strFunStr.Replace("(",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace("\"",NULL);
		int i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			return FALSE;
		}
		str1 = strFunStr.Mid(0,i ); 
		
		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str2 = strFunStr;
			return TRUE;
		}
		str2 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str3 = strFunStr;
			return TRUE;
		}
		str3 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str4 = strFunStr;
			return TRUE;
		}
		str4 = strFunStr.Mid(0,i ); 

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_DivideString(CString strOld, LPCTSTR lpszSub,CString &str1, CString &str2,CString &str3)
{
	if( "" == strOld || NULL == lpszSub || "" == lpszSub ){
		return FALSE;
	}
	try
	{
		str1="";
		str2="";
		str3="";
		
		CString strFunStr = strOld;
		strFunStr.Replace(" ",NULL);
		strFunStr.Replace("(",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace("\"",NULL);
		int i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			return FALSE;
		}
		str1 = strFunStr.Mid(0,i ); 
		
		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str2 = strFunStr;
			return TRUE;
		}
		str2 = strFunStr.Mid(0,i ); 

		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str3 = strFunStr;
			return TRUE;
		}
		str3 = strFunStr.Mid(0,i ); 

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_DivideString(CString strOld, LPCTSTR lpszSub,CString &str1, CString &str2)
{
	if( "" == strOld || NULL == lpszSub || "" == lpszSub ){
		return FALSE;
	}
	try
	{
		str1="";
		str2="";
		
		CString strFunStr = strOld;
		strFunStr.Replace(" ",NULL);
		strFunStr.Replace("(",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace("\"",NULL);
		int i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			return FALSE;
		}
		str1 = strFunStr.Mid(0,i ); 
		
		strFunStr = strFunStr.Mid(i+1); 
		i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str2 = strFunStr;
			return TRUE;
		}
		str2 = strFunStr.Mid(0,i ); 

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_DivideString(CString strOld, LPCTSTR lpszSub,CString &str1 )
{
	if( "" == strOld || NULL == lpszSub || "" == lpszSub ){
		return FALSE;
	}
	try
	{
		str1="";
		
		CString strFunStr = strOld;
		strFunStr.Replace(" ",NULL);
		strFunStr.Replace("(",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace(")",NULL);
		strFunStr.Replace("\"",NULL);
		int i = strFunStr.Find(lpszSub);
		if( -1 == i ){
			str1 = strFunStr; 
			return TRUE;
		}
		str1 = strFunStr.Mid(0,i ); 
		
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

}

void CPublicCardObj::PC_Cus_MoveNextFun(CFChannelInfo *pChnlObj, CCustomObj *pObj)
{
	pChnlObj->m_nStep = 1;
	pObj->MoveNextFun();
	return;
}

BOOL CPublicCardObj::PC_GetParameterValues(CFChannelInfo *pInfo, CString strOld, CString &str1, CString &str2, CString &str3, CString &str4, CString &str5)
{
	if( PC_DivideString( strOld, ";",str1, str2,str3,str4,str5) )
	{
		BOOL bResult = FALSE;
		CString strTemp = "";
		if( "" != str1 ){
			strTemp = str1;
			if( !PC_GetExpressValues(  pInfo, str1, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str2 ){
			strTemp = str2;
			if( !PC_GetExpressValues(  pInfo, str2, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str3 ){
			strTemp = str3;
			if( !PC_GetExpressValues(  pInfo, str3, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str4 ){
			strTemp = str4;
			if( !PC_GetExpressValues(  pInfo, str4, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str5 ){
			strTemp = str5;
			if( !PC_GetExpressValues(  pInfo, str5, strTemp ) ){
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_GetParameterValues(CFChannelInfo *pInfo, CString strOld, CString &str1, CString &str2, CString &str3, CString &str4)
{
	if( PC_DivideString( strOld, ";",str1, str2,str3,str4) )
	{
		BOOL bResult = FALSE;
		CString strTemp = "";
		if( "" != str1 ){
			strTemp = str1;
			if( !PC_GetExpressValues(  pInfo, str1, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str2 ){
			strTemp = str2;
			if( !PC_GetExpressValues(  pInfo, str2, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str3 ){
			strTemp = str3;
			if( !PC_GetExpressValues(  pInfo, str3, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str4 ){
			strTemp = str4;
			if( !PC_GetExpressValues(  pInfo, str4, strTemp ) ){
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_GetParameterValues(CFChannelInfo *pInfo, CString strOld, CString &str1, CString &str2, CString &str3)
{
	if( PC_DivideString( strOld, ";",str1, str2,str3) )
	{
		BOOL bResult = FALSE;
		CString strTemp = "";
		if( "" != str1 ){
			strTemp = str1;
			if( !PC_GetExpressValues(  pInfo, str1, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str2 ){
			strTemp = str2;
			if( !PC_GetExpressValues(  pInfo, str2, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str3 ){
			strTemp = str3;
			if( !PC_GetExpressValues(  pInfo, str3, strTemp ) ){
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_GetParameterValues(CFChannelInfo *pInfo, CString strOld, CString &str1, CString &str2 )
{
	if( PC_DivideString( strOld, ";",str1, str2) )
	{
		BOOL bResult = FALSE;
		CString strTemp = "";
		if( "" != str1 ){
			strTemp = str1;
			if( !PC_GetExpressValues(  pInfo, str1, strTemp ) ){
				return FALSE;
			}
		}

		if( "" != str2 ){
			strTemp = str2;
			if( !PC_GetExpressValues(  pInfo, str2, strTemp ) ){
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_GetParameterValues(CFChannelInfo *pInfo, CString strOld, CString &str1 )
{
	if( PC_DivideString( strOld, ";",str1) )
	{
		BOOL bResult = FALSE;
		CString strTemp = "";
		if( "" != str1 ){
			strTemp = str1;
			if( !PC_GetExpressValues(  pInfo, str1, strTemp ) ){
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void CPublicCardObj::PC_CTI_JoinConferenceResponseMsg(int nDN, int nResult)
{
	//����JoinConference Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send JoinConference Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_JoinConference( nDN, nResult );

	return;
}

void CPublicCardObj::PC_CTI_ConferenceCallResponseMsg(int nDN, int nResult)
{
	//����ConferenceCall Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send ConferenceCall Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_ConferCall( nDN, nResult );

	return;
}



void CPublicCardObj::PC_CT_ConnectValidate(int nDN )
{
	//����Socket ������֤��Ϣ
	CString strMSG("");
	strMSG.Format("[Send TCP/IP Connect Validate Response DN:%d]", nDN );
	PC_ShowCommMsg(strMSG);
	CTI_ValidateMsg( nDN );
}

void CPublicCardObj::PC_ShowXmsEvent( CString strMSG )
{
	if( NULL == m_plistSystem ) return;
	if( !IsWindow( m_plstXmsEvent->GetSafeHwnd() ) ) return;
	strMSG = (CTime::GetCurrentTime()).Format("%Y-%m-%d %H:%M:%S  ") + strMSG;
	int n_line = m_plstXmsEvent->GetCount();
	if (n_line > 5000)
	{
		m_plstXmsEvent->InsertString(n_line, strMSG );
		m_plstXmsEvent->SetCurSel(n_line);

		m_plstXmsEvent->DeleteString(0);
		m_plstXmsEvent->ResetContent();
	}
	else
	{
		m_plstXmsEvent->InsertString(n_line, strMSG);
		m_plstXmsEvent->SetCurSel(n_line);
	}

	return;
}

void CPublicCardObj::PC_ShowChannelVoice(int nIndex, int nDsp, int nID)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	CString str = "";
	str.Format("%d,%d",nDsp,nID );
	m_plstChannel->SetItemText( nIndex,6, str );
}

void CPublicCardObj::PC_ShowChannelFax(int nIndex, int nDsp, int nID)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	CString str = "";
	str.Format("%d,%d",nDsp,nID );
	m_plstChannel->SetItemText( nIndex,7, str );
}

void CPublicCardObj::PC_ShowChannelConf(int nIndex, int nDsp, int nID)
{
	if( NULL == m_plstChannel ) return;
	if( !IsWindow( m_plstChannel->GetSafeHwnd() ) ) return;
	CString str = "";
	str.Format("%d,%d",nDsp,nID );
	m_plstChannel->SetItemText( nIndex,6, str );
}

CVipCallSetObj * CPublicCardObj::PC_GetVipCallSetObj(CString strTel)
{
	strTel.Replace(" ",NULL);
	POSITION pos  = m_pVipCallSetObjList->GetHeadPosition();
	while ( pos )
	{
		CVipCallSetObj * pVar = m_pVipCallSetObjList->GetNext(pos);
		if( pVar != NULL )
		{
			if( strTel == pVar->m_strDN )
			{
				return pVar ;
			}
		}
	}

	return NULL;
}

BOOL CPublicCardObj::PC_InitVipConfig()
{
	CString strfile = "";
	strfile = theApp.m_strPath + "\\NorVip.ccf";


	CCIFVipCallSetDoc doc;
	if( ! doc.OnOpenDocument(strfile) )
	{
		PC_ShowSysMsg( 0 , "Not find NorVip.ccf file" );
		return FALSE;
	}
	

	CString str = "";
	POSITION pos = doc.m_VipCallSetObjList.GetHeadPosition();
	int i = 0;
	while( pos )
	{
		CVipCallSetObj *pObj = doc.m_VipCallSetObjList.GetNext(pos);
		if( NULL != pObj )
		{
			CVipCallSetObj *pNewObj = new CVipCallSetObj;
			*pNewObj = *pObj;
			m_VipCallSetObjList.AddTail(pNewObj);

		}
	}


	return TRUE;
}

void CPublicCardObj::PC_ChangeLineState(CFChannelInfo *pChnlInfo, int nState)
{
	if( nState == pChnlInfo->m_nState )
		return;
	pChnlInfo->m_nState =  nState;
	CString strState = "";
	
	switch( nState )
	{
	case CHANNEL_STATE_FREE://ͨ�����ڷ�ժ����ͨ��������״̬ʱ��ͨ�������ڸ�״̬
		strState = "Free";
		break;
	case CHANNEL_STATE_CONNECT://ͨ������ʹ��״̬
		strState = "Busy";
		break;
	case CHANNEL_STATE_HOLD://ͨ������Hold״̬
		strState = "Hold";
		break;
	case CHANNEL_STATE_DIAL://ͨ������Dial״̬
		strState = "Dial";
		break;
	case CHANNEL_STATE_RINGING://ͨ����������״̬
		strState = "Ringing";
		break;
	case CHANNEL_STATE_MONITORING://ͨ�����ڼ���״̬
		strState = "Monitoring";
		break;
	case CHANNEL_STATE_TRANSFER://ͨ�����ں���ת��״̬
		strState = "Transfer";
		break;
	case CHANNEL_STATE_CLOSE://ͨ�����ں���ת��״̬
		strState = "Close";
		break;
	case CHANNEL_STATE_WAITQUENE://ͨ�����ں���ת��״̬
		strState = "WaitQueue";
		break;
	case CHANNEL_STATE_WAITFREE://ͨ�����ں���ת��״̬
		strState = "WaitFree";
		break;
	case CHANNEL_STATE_OFFHOOK://
		strState = "OffHook";
		break;
	case CHANNEL_STATE_AFTERWORK://
		strState = "AfterWork";
		break;
	default:
		break;
	}

	PC_ShowChannelState( pChnlInfo->m_nIndex, strState);

	return;
}

void CPublicCardObj::PC_CTI_SetBusyResponseMsg(int nDN, int nResult)
{
	//���� Ready Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send Set Busy Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_SetBusy( nDN, nResult);
	return;
}

void CPublicCardObj::PC_CTI_InserverResponseMsg(int nDN, int nResult)
{
	//���� Ready Response��Ϣ
	CString strMSG("");
	strMSG.Format("[Send In Server Response DN:%d][Result:%s]", nDN, (0 == nResult?"Success":"fail") );
	PC_ShowCommMsg(strMSG);
	CTI_SetInServer( nDN, nResult);
	return;
}

BOOL CPublicCardObj::PC_GetSplitString(CString strOld, CString &str1, CString &str2)
{
	int nPos1 = strOld.Find( '|');
	if( -1 == nPos1 )
	{
		return FALSE;
	}
	str1 = strOld.Left( nPos1 );
	str2 = strOld.Right( strlen(strOld) - nPos1-1 );
	return TRUE;
}


BOOL CPublicCardObj::PC_Bfx2Tiff(CString &filename)
{
	try
	{
		CString strbfxFile = filename;
		
		if( strlen(strbfxFile ) <= 0 )
			return FALSE;

		if( -1 != strbfxFile.Find(".tif") )
			return TRUE;
		CString strTifFile = filename;
		strTifFile.Replace(".bfx",".tif");

		int nResult = DJCvt_Bfx2Tiff(  (LPTSTR)(LPCTSTR)strbfxFile ,(LPTSTR)(LPCTSTR)strTifFile );
		if( 1 == nResult )
		{
			filename = strTifFile;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		return FALSE;
	}

}

BOOL CPublicCardObj::PC_Tiff2Bfx(CString &filename)
{
	try
	{
		CString strTifFile = filename;
		
		if( strlen(strTifFile ) <= 0 )
			return FALSE;
		if( -1 != strTifFile.Find(".bfx") )
			return TRUE;
		CString strbfxFile = filename;
		strbfxFile.Replace(".tif",".bfx");
		int nResult = DJCvt_Tiff2Bfx(  (LPTSTR)(LPCTSTR)strTifFile ,(LPTSTR)(LPCTSTR)strbfxFile );
		if( 1 == nResult )
		{
			filename = strbfxFile;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
	catch(...)
	{
		return FALSE;
	}
}
