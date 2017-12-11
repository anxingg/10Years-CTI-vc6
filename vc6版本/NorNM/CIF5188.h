#ifndef CIF5188_H
#define CIF5188_H


#include "..\\include\\NodeObj.h"

extern CD08A *g_pD08a;


#define WM_MY_FRIEND_MSG		WM_USER+220//���Ϳ������к���Ϣ
#define MAX_CHANNEL_COUNT		60		//���ͨ����
#define MAX_FAX_CHANNEL_COUNT	2		//����FAXͨ����


class CFaxStateList
{
public:
	CFaxStateList(){
	m_nStatus = 0;
	m_nCurrPage = 0;
	m_nTotalPage = 0;
	m_strFaxFileName = "";
	m_strPhone = "";
	};
	~CFaxStateList(){};
public:
	int m_nStatus;
	int m_nCurrPage;
	int m_nTotalPage;
	CString m_strFaxFileName;
	CString m_strPhone;
};


class CPlayFileList
{
public:
	CPlayFileList(){ m_nIndex = -1; m_strPlayFile = ""; };
	~CPlayFileList(){};
public:
	int m_nIndex;
	CString m_strPlayFile;
};


enum CONF_STATE
{
	CONF_FREE,//����״̬��û��ռ��
	CONF_RECORD,//���ڻ���¼��״̬
	CONF_PLAY,//���ڻ������״̬
	CONF_PLAY_INDEX,//�����ļ��б�
	CONF_ADD,
	CONF_EXIT,
	CONF_LEAVE
};
typedef struct
{
	CONF_STATE State;
	int nChnlIndex;//���������ͨ���ţ������ڿ���״̬ʱ����ֵΪ-1
	int nPlayIndex;//����û��������ͨ����
	int nRecordIndex;//����û���¼����ͨ����
	int nConfPlayID;//�����������ͨ���ţ������ڿ���״̬ʱ����ֵΪ-1
	int nConfRecordID;//����¼������ͨ���ţ������ڿ���״̬ʱ����ֵΪ-1
	int nVocID;//������뿪��������ͨ��
	int nRecID;//������뿪��������ͨ��
	int nMemberNum;
	int nResNum;
	int nListenNum;
}CONF_STRUCT;


#define CIF_FAX_FREE        0
#define CIF_FAX_RECEIVE     1
#define CIF_FAX_SEND        2

//�������״̬����
#define CIF_CONF_NULL			0 //����(��û�д�����������)
#define CIF_CONF_LISTEN		    1 //ֻ����Ա���ڻ�����ֻ����������˵
#define CIF_CONF_SPEAKER_LISTEN 2 //��ͨ��Ա����˵���������ж�̬����Ȩ
#define CIF_CONF_SPEAKER_ALWAYS 3 //ʼ��˵��Ա�����Ǿ��з���Ȩ
#define CIF_CONF_SPEAKER_ONLY   4 //ֻ˵��Ա���ڻ�����ֻ��˵������������Ҫ���ڶԻ��������
#define CIF_CONF_CREAT          5 //�����˸û��飬ͬʱ��˵��������Ȩ����û���

 void NewAssignsObjList( const CNodeObjList &s , CNodeObjList *d,int nAssigns );
///////////////////////////////////////////////
//class  CNewRecordsetVar
//////////////////////////////////////////////
class  CNewRecordsetVar : public CRecordsetVar  
{
public:
	CNewRecordsetVar();
	virtual ~CNewRecordsetVar();
	CNewRecordsetVar( const CNewRecordsetVar &s );
	CNewRecordsetVar( const CRecordsetVar &s );
	CNewRecordsetVar & operator =( const CNewRecordsetVar &s );

public:
	void PrintComError(CString &ErrStr , _com_error &e);
	_RecordsetPtr m_pRecordset;
	_CommandPtr m_pCommand;
};

typedef  CTypedPtrList<CObList, CNewRecordsetVar*> CNewRecordsetVarList;

///////////////////////////////////////////////
//class  CNewDBConnVar
//////////////////////////////////////////////

class  CNewDBConnVar:public CDBConnVar
{
public:
	BOOL OpenDataBase( CString strConn, CString &strError );
	BOOL CloseDatabase( CString &strError );
	CNewDBConnVar();
	CNewDBConnVar( const CNewDBConnVar &s );
	CNewDBConnVar( const CDBConnVar &s );
	CNewDBConnVar & operator =( const CNewDBConnVar &s );
	virtual ~CNewDBConnVar();
public:
	void PrintProviderError( CString &ErrStr, _ConnectionPtr pConnection);
	void PrintComError(CString &ErrStr , _com_error &e);
	BOOL ExcuteSQL(  CString strSQL, CString &strError );
	BOOL ExcuteSQL( CNewRecordsetVar *RS,CString strSQL,CString &strError );
	_ConnectionPtr	m_pConnection;
	BOOL m_bConnect;
	_RecordsetPtr m_pRecordset;
	_CommandPtr m_pCommand;

};

typedef  CTypedPtrList<CObList, CNewDBConnVar*> CNewDBConnVarList;

///////////////////////////////////////////////
//class  CFChannelInfo
//////////////////////////////////////////////
class   CFChannelInfo
{

public:
	CFChannelInfo();
	virtual ~CFChannelInfo();
public:
	void MoveFirst_Node( void );
	bool SaveToNorVar( CString strValues, int nVarKey );
	BOOL MoveNext_Node( int nNextType );
	void RemoveALLObjects();

	CNodeObj * FindNodeObj( int nKey );
public:
	BOOL GetNorVarValues(CString &strValues, int &nVarType, int nVarKey);
	BOOL GetSQLExpressValues( CString &strNewExpress, CString strOldExpress );
	BOOL MoveNext_Node( CNextNode *pNextObj );
	void ExcuteSQL( void );
	CNewRecordsetVar * FindRecordsetVar( int nKey );
	CNewDBConnVar *  FindDBConnectVar( int nKey);
	bool GetNorVarValues(CString &strValues, int nVarKey);
	void PrintADOVersion( _ConnectionPtr pConnection );
	void PrintComError(_com_error &e);
	void PrintProviderError(_ConnectionPtr pConnection);
public:
	void PrintADOConnectString( _ConnectionPtr pConnection);
	void InitNormalVar( void );
	BOOL GetContantValues(CString &strValues, int &nVarType,int nVarKey);
	void MoveCallEndBlock_Node( void );
	BOOL GetConstAndVarValues( CString &strValues, int nVarKey);
	void ExcuteStoredPro( void );
	CDLLVariable * FindDLLVariable( int nKey );
	void ShowSysMsg(CString strMSG);
	BOOL GetContantValues(CString &strValues,  int nVarKey);
public:
	BOOL m_bRecordLoop;
	BOOL m_bPlayLoop;

	int m_nRecordType;//¼�����ͣ�0��ͨ 1����
	int m_nPlayType;//�������ͣ�0��ͨ 1����
public:
	CString m_strAgentPwd;//��ϯ����
	CString m_strAgentID;//��ϯ����
	CString m_nGroup;//��ϯ��ǰ��
	CString m_strLoginTime;//��ϯLogin ʱ��
	CString m_strLogOutTime;//��ϯLogout ʱ��
	CString m_strReadyTime;//��ϯReady ʱ��
	CString m_strNotReadyTime;//��ϯNot Ready ʱ��
	CString m_strAgentName;//��ϯ����
	CString m_strGroupName;//��ϯ��ǰ����

public:
	int m_nDtmfLen;//����DTMF�ĳ���
	CString m_strDtmfEnd;//����DTMF�Ľ�������
	int m_nSaveDtmfKey;//����DTMF�ı��������Key
	int m_nTimeOut;
public:
	BOOL m_bExcuteSQL;//ִ��SQL����Ƿ����
	BOOL m_bRecord;//�Ƿ���Ҫ��¼��
	CNewDBConnVar *m_pCurDBConn;
	CNewRecordsetVar *m_pCurRecset;
	CStoreProObj	*m_pCurStoreProObj;
	int m_nChanelType;
	int m_nLinkCh;//���ӵ�ͨ��
	int m_nFaxChnl;//ռ�õĴ�����Դͨ��
	int m_nLinkType;//����ͨ������������
	int m_nTransferCh;//����ת��ʱ��Ŀ��ͨ��
	int m_nHoldChnl;//�ñ���ͨ��Hold��ͨ����
	int m_nInvitConfChnl;//���������������ͨ����
	int m_nConfNO;//���ڻ�����Դ���
	int m_nConfAction;//��ͨ���Ļ�����Ϊ��������
	int m_nState;//ͨ��״̬
	int m_nChannel;//ͨ�����
	int m_nIndex;//ͨ����ͨ��������ı��
	int m_nChangeVoiceChnl;//����ͨ����
	int m_nStep;//��״̬����
	long m_lTimeElapse;//��ʱ����������λ����
	int m_nRings;
	long m_lCardKey;
	int  m_nEventType;//ʱ�����ͱ���һ����Ĳ�ɣ��յ���绰��Ϣ�¼���
	int  m_nCallType;//�������ͣ�1 Inbound, 0 Outbound -1 not any call
	int  m_nRecordLength;
	int  m_nPlayGroup;//��ͨ�����ڿ��ƻ�������Ļ�����
	int  m_nRecordGroup;//��ͨ�����ڿ��ƻ���¼�����Ļ�����
	int  m_nHandleConferChnl;//�����Ļ���ͨ����,��ͨ����
	int  m_nHandleConferNo;//�����Ļ���š�

	CString m_strPlayFile;
	CString m_strScriptFile;
	CString m_strRecordFile;//���¼���ļ����ı����
	CString m_strReciveFaxFile;
	CString m_strSendFaxFile;
	CString m_strState;
	CString m_strtEndTime;//ͨ������ʱ��
	CString m_strtBeginTime;//ͨ����ʼʱ��
	CString m_strDTMFRecived;//���յ�DMF��
	CString m_strRingTime;//��ʼ����ʱ��
	int m_nHangUpType;//�һ����ͣ�0�������һ� 1�����һ�
	CNodeObjList m_objects;//�ű��ڵ����
	CNewRecordsetVarList m_NewRecordsetVariList;
	CNewDBConnVarList m_NewDBConnVarList;
	CNormalVarList m_NormalVarList;
	CFDLLVariableList m_DLLVariableList;
	CNodeObj * m_pCurrentNodeObj;

	CString m_strCalledID;//���к���
	CString m_strCallerID;//���к���
	CString m_strLocalDN;//���غ���
	CString m_strInboundCallPK;//��ͨ�����е�ͨ����ʶ
	CString m_strOutboundCallPK;//��ͨ�����е�ͨ����ʶ

	int  m_nDialType;//��������
	CString m_strDialTel;
	int m_nDialChIndex;//���ڱ���makecall��ʱʹ�õ�ͨ����

	int m_nPickUpType;//ȡ�ر��ֵ�����
	CString m_strPickUpTel;//ȡ�ر��ֵ����к���

	int m_nActiveType;
	CString m_strActiveTel;

	int m_nThreadCount;
	CListBox	*m_plistSystem;
	BOOL m_bEventCheckPlay;

	int m_nLineInit;//��·�Ƿ��ʼ����ԭ��0δ��ԭ��1��ԭ��

	int m_nCallInType;//0 �ֻ����� 1�Ƿֻ����� ֻ��������Ч
public:
	BOOL InPutVarValuesToParam( CParameter * pParam , int nVarKey);
	BOOL OutParamValuesToVariable(CString strValues, int nVarKey);
	BOOL SaveToModuleParam(CString strValues, int nVarKey);
	BOOL GetModuleParamValues(CString &strValues, int nVarKey);
	void SetCurrentNodeObj( CNodeObj *pObj );
	CNodeObj * GetCurrentNodeObj( void );
	BOOL m_bDBExcuteEnd;//���ݿ����ִ�н���

	HANDLE m_Threadhandle;//��ǰ��ͨ��ִ�е��߳̾���������ݿ⡢ִ�д洢���̺�SQL�����߳�
};

class  CNormalScriptInfo  
{
public:
	CNormalScriptInfo();
	virtual ~CNormalScriptInfo();
	void InitNodeObjList(CNodeObjList *pObjList, CNodeObj *pObj);
	BOOL InitNormalScript( CString strFile );
public:
	CNodeObjList m_objects;
	CString m_strScriptFile;
public:
	CRecordsetVarList m_RecordsetVariList;
	CDBConnVarList m_DBConnVarList;
	CNormalVarList m_NormalVarList;
	CFDLLVariableList m_DLLVariableList;
};

 


class CNewModuleObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CNewModuleObj);
	CNewModuleObj();
public:

	virtual ~CNewModuleObj();
	CNewModuleObj( const CNewModuleObj &s );
	CNewModuleObj & operator =( const CNewModuleObj &s );
	CNewModuleObj( const CModuleObj &s );
	virtual void Serialize(CArchive& ar);
public:
	CNodeObj *m_pCurObj;
	CNodeObjList m_objects;
	CNewRecordsetVarList m_RecordsetVariList;
	CNewDBConnVarList m_DBConnVarList;
	CNormalVarList m_NormalVarList;
	CFDLLVariableList m_DLLVariableList;

	CParameterList m_ParameterLst;//ģ��ڵ�����б�
	CString m_strFile;//����ģ���ļ��ľ���·���Լ��ļ���

	CNewDBConnVar *m_pCurDBConn;
	CNewRecordsetVar *m_pCurRecset;
	CStoreProObj	*m_pCurStoreProObj;
};


class  CNewBlockObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CNewBlockObj);
	CNewBlockObj();
public:

	virtual ~CNewBlockObj();
	CNewBlockObj( const CNewBlockObj &s );
	CNewBlockObj( const CBlockObj &s );
	CNewBlockObj & operator =( const CNewBlockObj &s );
	CNodeObj * CNewBlockObj::FindNodeObj(int nKey, int &nParentKey);
	CNodeObj * FindObj( int nKey );
	virtual void Serialize(CArchive& ar);
public:
	CNodeObjList m_ChildObjLst;
};


class CDBParameters  
{
public:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	_CommandPtr m_pCommand;
	CString strSQL;
	CString strError;
public:
	CDBParameters();
	virtual ~CDBParameters();
	CDBParameters( const CDBParameters &s );
	CDBParameters & operator =( const CDBParameters &s );

};


class CNewSQLExcuteObj : public CSQLExcuteObj  
{
protected:
	DECLARE_SERIAL(CNewSQLExcuteObj);
	CNewSQLExcuteObj();
public:
	virtual ~CNewSQLExcuteObj();
	CNewSQLExcuteObj( CPoint point );
	CNewSQLExcuteObj( const CSQLExcuteObj &s );
	CNewSQLExcuteObj( const CNewSQLExcuteObj &s );
	CNewSQLExcuteObj & operator =( const CNewSQLExcuteObj &s );
public:
	CNewDBConnVar *m_pDB;
	CNewRecordsetVar *m_pRS;
	CString m_str;
	CString m_strError;
	
public:
	virtual void Serialize(CArchive& ar);
};


class CNewStoreProObj : public CStoreProObj  
{
protected:
	DECLARE_SERIAL(CNewStoreProObj);
	CNewStoreProObj();
public:
	
	virtual ~CNewStoreProObj();
	CNewStoreProObj( CPoint point );
	CNewStoreProObj( const CStoreProObj &s );
	CNewStoreProObj( const CNewStoreProObj &s );
	CNewStoreProObj & operator =( const CNewStoreProObj &s );
public:
	CNewDBConnVar *m_pDB;
	CNewRecordsetVar *m_pRS;
	CString m_str;
	CString m_strError;
	CFChannelInfo *m_pChnlInfo;
public:
	virtual void Serialize(CArchive& ar);
};


class CNewEndCallBlockObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CNewEndCallBlockObj);
	CNewEndCallBlockObj();
public:
	CNewEndCallBlockObj( const CNewEndCallBlockObj &s );
	CNewEndCallBlockObj( const CEndCallBlockObj &s );
	CNewEndCallBlockObj & operator =( const CNewEndCallBlockObj &s );
	virtual ~CNewEndCallBlockObj();
public:
	CNodeObjList m_ChildObjLst;
public:
	CNodeObj * FindNodeObj(int nKey, int &nParentKey);
	CNodeObj * GetEndCallEventCheckObj( void );
	CNodeObj * FindObj( int nKey );
	virtual void Serialize(CArchive& ar);
};

#endif 
