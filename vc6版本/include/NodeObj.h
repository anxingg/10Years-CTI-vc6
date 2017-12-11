// NodeObj.h: interface for the CNodeObj class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _NODEOBJ_H_
#define _NODEOBJ_H_

#include <time.h> 
#include <afxtempl.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// Exported DLL initialization is run in context of application   
// or Regular DLL.   
//extern "C" _declspec(dllexport)  void WINAPI Init_ChiefScriptNodeDLL() ;
//DLL���õĺ������Ͷ���
typedef void(*DLLCALL_FUNCTION)(int *nParam1, char szParam2[500], char szParam3[500]  );

//ͨ��״̬
#define CHANNEL_STATE_FREE  20 //ͨ�����ڷ�ժ����ͨ��������״̬ʱ��ͨ�������ڸ�״̬
#define CHANNEL_STATE_CONNECT  21 //ͨ������ʹ��״̬
#define CHANNEL_STATE_HOLD  22 //ͨ������Hold״̬
#define CHANNEL_STATE_DIAL  23 //ͨ������Dial״̬
#define CHANNEL_STATE_RINGING     24 //ͨ����������״̬
#define CHANNEL_STATE_MONITORING  25 //ͨ�����ڼ���״̬
#define CHANNEL_STATE_TRANSFER  26 //ͨ�����ں���ת��״̬
#define CHANNEL_STATE_CLOSE  27 //ͨ�����ڹر�״̬
#define CHANNEL_STATE_WAITQUENE  28 //�ȴ�����״̬
#define CHANNEL_STATE_WAITFREE  29 //�ȴ��ͷ�״̬̬
#define CHANNEL_STATE_OFFHOOK  30 //ժ��״̬
#define CHANNEL_STATE_AFTERWORK  31 //����״̬

//��������
#define CF_CMD_NO		1;//û������
#define CF_CMD_PLAYFILE 2;//�����ļ�
#define CF_CMD_RECDTMF  3;//����DTMF

//��������
#define CHANNEL_CALLTYPE_NOCALL		29//û�л����
#define CHANNEL_CALLTYPE_OUT		30//ģ�⿨ϵͳ�漰�����ߵĺ���
#define CHANNEL_CALLTYPE_IN			31//ģ�⿨ϵͳ�ֻ���ĺ���
#define CHANNEL_CALLTYPE_COMEIN		32 //�����绰���� 
#define CHANNEL_CALLTYPE_CALLOUT	33 //���й����绰


//�Զ����ͨ������
#define CF_CHTYPE_NOKNOW    -1 //δ֪ͨ������ 
#define CF_CHTYPE_EMPTY     0  //��ͨ�� 
#define CF_CHTYPE_DTNO1     1  //�����м��й�һ������ 
#define CF_CHTYPE_DTNO7     2  //�����м��й��ߺ�����(TUP)
#define CF_CHTYPE_DTDSS1    3  //�����м�ISDN PRI(30B+D)
#define CF_CHTYPE_DTNO7ISUP 4  //�����м��й��ߺ�����(ISUP)
#define CF_CHTYPE_MTP3		5  //�ߺ�����MTP�����ͨ��
#define CF_CHTYPE_TRUNK     6  //ģ���м�����(��ͨ�л���),�ӵ��ž�
#define CF_CHTYPE_USER		7  //ģ���м�����(��ϯ��)���ӵ绰��
#define CF_CHTYPE_RECORD    8  //ģ���м̸�����(�����)����绰��/�绰���ಢ��
#define CF_CHTYPE_VIP	    9  //VOIPͨ������



//ͨ����������
#define CF_CHSWITCH_CONNECT			0//ֱ������
#define CF_CHSWITCH_MONITOR			1//����
#define CF_CHSWITCH_SPEAK			2//������������������������
#define CF_CHSWITCH_DISCONNECT		3//�Ͽ�����
#define CF_CHSWITCH_CALLAGENT		4//������ϯ����ʱ��ͨ����Ϊ�ֻ�����
#define CF_CHSWITCH_CALLCUSTOMER	5//���пͺ�

//��㱣�����ݵĻ����������ͳ�������
#define DATATYPE_INTEGER		0
#define DATATYPE_FLOAT			1
#define DATATYPE_STRING			2
#define DATATYPE_DATE			3
#define DATATYPE_TIME			4
#define DATATYPE_DATATIME		5
#define DATATYPE_DBCONNECT      6//���ݿ����ӱ�������
#define DATATYPE_DBRECORDSET    7//���ݿ��¼��������
#define DATATYPE_DLL		    8//DLL��������
#define DATATYPE_TCP_SOCKET		9//TCP_SOCKET��������
#define DATATYPE_UDP_SOCKET		10//UDP_SOCKET��������

//������ͳ�������
#define NODETYPE_START			0
#define NODETYPE_WAITCALL		1
#define NODETYPE_START_PLAY     2
#define NODETYPE_CHECK_DIAL		3
#define NODETYPE_RELEASECALL	4
#define NODETYPE_CALLHOLD		5
#define NODETYPE_CHECK_TRANSFER	6
#define NODETYPE_START_RECORD	10
#define NODETYPE_SETVARIABLE	12
#define NODETYPE_COMPARE		13
#define NODETYPE_ENTER_CONFERENCE	14
#define NODETYPE_LEAVE_CONFERENCE	15
#define NODETYPE_OPEN_DATABASE		16
#define NODETYPE_CLOSE_DATABASE		17
#define NODETYPE_EXCUTE_SQL			18
#define NODETYPE_DLL_LOAD		19
#define NODETYPE_DLL_UNLOAD		20
#define NODETYPE_DLL_CALL		21
#define NODETYPE_SEND_DTMFCODE	22
#define NODETYPE_RECIVE_DTMF	23
#define NODETYPE_EVENT_CHECK	24
#define NODETYPE_DATABASE_BROWSE 25
#define NODETYPE_DTMF_MENU       26
#define NODETYPE_CALLLOGREQUEST  27
#define NODETYPE_NODE_END        28
#define NODETYPE_SWITCH          29
#define NODETYPE_ANSWERCALL      30
#define NODETYPE_FAXRECIVE       31
#define NODETYPE_FAXSEND         32
#define NODETYPE_CONFERENCE_BUILD      33
#define NODETYPE_CONFERENCE_DELETE     34
#define NODETYPE_GET_CONFERENCE_MEMBER 35
#define NODETYPE_CONFERENCE_REQUEST    36

#define NODETYPE_BLOCK     37
#define NODETYPE_ENTER	   38
#define NODETYPE_RETURN      39


#define NODETYPE_STOREPRO			40//�洢����
#define NODETYPE_CHANGEVOICE		41//����

#define NODETYPE_ENDCALL_BLOCK			42//���н�����������
#define NODETYPE_ENDCALLEVENT_CHECK		43//���н����¼����
#define NODETYPE_ENDCALLEVENT_END		44//���н����¼��������

#define NODETYPE_CONFER_PLAY        45//�������
#define NODETYPE_CONFER_RECORD	    46//����¼��
#define NODETYPE_EXPRESS			47//���ʽ
#define NODETYPE_FUNCTION           48//����
#define NODETYPE_GOTO               49//����
#define NODETYPE_ROUTE_CALL         50//����·��
#define NODETYPE_TRACE_INFO         51//��Ϣ����
#define NODETYPE_MOUDLE             52//����ģ���ļ��ӵ�
#define NODETYPE_MOUDLE_BEGIN       53//ģ�鿪ʼ
#define NODETYPE_MOUDLE_END         54//ģ�����
#define NODETYPE_CUSTOM             55//���ƽڵ�
#define NODETYPE_CC_CALLAGENT       56//���ƽڵ�
#define NODETYPE_CC_TRANSFER     57//���ƽڵ�

//����֧���ͳ�������
#define NEXTTYPE_NEXT			1
#define NEXTTYPE_ERROR			2
#define NEXTTYPE_BUSY			3
#define NEXTTYPE_NO_DIALTONE	4
#define NEXTTYPE_NO_ANSWER 		5
#define NEXTTYPE_NO_RINGBACK	6
#define NEXTTYPE_CONNECT		7
#define NEXTTYPE_NOMATCH		8//û��ƥ��ıȽϣ�����Compare�ڵ�	
#define NEXTTYPE_DTMF_0			60
#define NEXTTYPE_DTMF_1			61
#define NEXTTYPE_DTMF_2			62
#define NEXTTYPE_DTMF_3			63
#define NEXTTYPE_DTMF_4			64
#define NEXTTYPE_DTMF_5			65
#define NEXTTYPE_DTMF_6			66
#define NEXTTYPE_DTMF_7			67
#define NEXTTYPE_DTMF_8			68
#define NEXTTYPE_DTMF_9			69
#define NEXTTYPE_DTMF_10		70 //DTMF #
#define NEXTTYPE_DTMF_11		71//DTMF *
#define NEXTTYPE_DTMF_TIMEOUT	72
#define NEXTTYPE_OFFHOOK		20//ժ��
#define NEXTTYPE_HANGUP			21//�һ�
#define NEXTTYPE_HOOKFLASH		22//�Ĳ��
#define NEXTTYPE_RECDTMF		23//�յ�����
#define NEXTTYPE_RECFAXTONE		24//�յ��Է��ȴ����մ����ź�
#define NEXTTYPE_AGENT_READY	25//Agent Ready
#define NEXTTYPE_AGENT_NOTREADY	26//Agent NotReady
#define NEXTTYPE_CONFDESTROYED	27//����ͨ�����ڵĻ�����Դ���ͷţ�
#define NEXTTYPE_AGENT_LOGIN_MSG	28
#define NEXTTYPE_AGENT_OUT_MSG		29
#define NEXTTYPE_AGENT_BUSY_MSG		30
#define NEXTTYPE_AGENT_INSERVE_MSG	31
#define NEXTTYPE_AGENT_ANSWER_MSG	32
#define NEXTTYPE_AGENT_MAKECALL_MSG 33
#define NEXTTYPE_AGENT_TRANSFER_MSG 34
#define NEXTTYPE_AGENT_CONFER_MSG	35
#define NEXTTYPE_AGENT_HANGUP_MSG	36
#define NEXTTYPE_AGENT_SENDDATA_MSG 37 //��ϯ���໥������Ϣʱ��
#define NEXTTYPE_AGENT_SENDFAX_MSG	38
#define NEXTTYPE_RINGING			39//��������
#define NEXTTYPE_NOTHING			40//û���¼�����
#define NEXTTYPE_EOF_FALSE			41//���ݿ��ѯ�ļ�¼Ϊ��
#define NEXTTYPE_AGENT_HOLD_MSG		42//
#define NEXTTYPE_AGENT_PICKUPHOLD_MSG	43//
#define NEXTTYPE_AGENT_MONITOR_MSG		44//
#define NEXTTYPE_AGENT_DISMONITOR_MSG	45//
#define NEXTTYPE_AGENT_BUILDCONFER_MSG	46//��������
#define NEXTTYPE_ELAPSE_TIME			47//�ӿ�ʼ��ʱ���涨ʱ�佫�������¼�
#define NEXTTYPE_CANCEL_TRANSFER		48//ȡ������ת��
#define NEXTTYPE_AGENT_GETACTIVECALL	49//ȡ�û����

//�¼����ͳ���,ֻ�ж�ȡ�ñ����ĵط����ܽ�ͨ�����¼�������ֵΪ0

#define EVENTTYPE_OFFHOOK				1
#define EVENTTYPE_HANGUP				2
#define EVENTTYPE_HOOKFLASH 			3
#define EVENTTYPE_RECDTMF  				4
#define EVENTTYPE_RECFAXTONE  			5
#define EVENTTYPE_AGENT_READY 		    6
#define EVENTTYPE_AGENT_NOTREADY  	    7
#define EVENTTYPE_CONFDESTROYED 		8
#define EVENTTYPE_AGENT_LOGIN_MSG		9
#define EVENTTYPE_AGENT_OUT_MSG			10
#define EVENTTYPE_AGENT_BUSY_MSG 		11
#define EVENTTYPE_AGENT_INSERVE_MSG		12
#define EVENTTYPE_AGENT_ANSWER_MSG		13
#define EVENTTYPE_AGENT_MAKECALL_MSG	14
#define EVENTTYPE_AGENT_TRANSFER_MSG	15
#define EVENTTYPE_AGENT_CONFER_MSG		16
#define EVENTTYPE_AGENT_HANGUP_MSG		17
#define EVENTTYPE_AGENT_SENDDATA_MSG	18
#define EVENTTYPE_AGENT_SENDFAX_MSG		19
#define EVENTTYPE_RINGING		        20
#define EVENTTYPE_NOTHING				21//û���¼�����
#define EVENTTYPE_AGENT_HOLD_MSG		22
#define EVENTTYPE_AGENT_PICKUPHOLD_MSG	23
#define EVENTTYPE_AGENT_MONITOR_MSG		24
#define EVENTTYPE_AGENT_DISMONITOR_MSG	25
#define EVENTTYPE_AGENT_BUILDCONFER_MSG	26
#define EVENTTYPE_ELAPSE_TIME			27
#define EVENTTYPE_AGENT_CANCEL_TRANSFER	28//ȡ������ת��
#define EVENTTYPE_AGENT_GETACTIVECALL	29

//Action ����
#define ACTION_EXPRESS			0 //"Express( <SaveTo> ; <  > )");
#define ACTION_LEFT				1// "Left( <SaveTo> ; <string> ; <length> )");
#define ACTION_MID				2// "Mid( <SaveTo> ; <string> ; <start> ; <length> )");
#define ACTION_LEN				3// "Len( <SaveTo> ; <string> )");
#define ACTION_RIGHT			4// "Right( <SaveTo> ; <string> ; <length> )");
#define ACTION_GETHOLDCHNL		5// "GetHoldChnl( <SaveTo> )");
#define ACTION_AGENTISFREE		6// "AgentIsFree( <SaveTo> ; <AgentDN> )");
#define ACTION_GETLINKCHNLVAR	7// "GetLinkChnlVar( <SaveTo> ; <Variable> )");
#define ACTION_GETLINKCHNLTYPE	8//"GetLinkChnlType( <SaveTo> )");
#define ACTION_GETFREECHNL		9// "GetFreeChnl( <SaveTo> ; <ChnlType> ; <Begin> ; <End> )");
#define ACTION_GETCHNLINDEX		10// "GetChnlIndex( <SaveTo> ; <ChnlType> ; <LocalDN> )");

class AFX_EXT_CLASS CDBConnVar : public CObject  
{
protected:
	DECLARE_SERIAL(CDBConnVar);
public:
	CDBConnVar();
	CDBConnVar( const CDBConnVar &s );
	CDBConnVar & operator =( const CDBConnVar &s );
	virtual ~CDBConnVar();
public:
	CString m_strName;
	CString m_strConn;
	int m_nVarDataType;
	int m_nKey;

public:
	virtual void Serialize(CArchive &ar);
};

typedef AFX_EXT_CLASS CTypedPtrList<CObList, CDBConnVar*> CDBConnVarList;

/////////////////////////////////////////////////////////////////////////////
// CDLLVariable document
//////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CDLLVariable : public CObject  
{
protected:
	DECLARE_SERIAL(CDLLVariable);
public:
	HINSTANCE m_hDLL;
	CString m_strName;
	int m_nVarDataType;
	int m_nKey;
	CString m_strDLLFileName;
	virtual void Serialize(CArchive &ar);
	CDLLVariable();
	CDLLVariable( const CDLLVariable &s );
	CDLLVariable & operator =( const CDLLVariable &s );

	virtual ~CDLLVariable();

};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CDLLVariable*> CFDLLVariableList;


////////////////////////////////////////////////////////////////
//CNormalVar
///////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CNormalVar : public CObject  
{
protected:
	DECLARE_SERIAL(CNormalVar);
public:
	CNormalVar();
	CNormalVar( const CNormalVar &s );
	CNormalVar & operator =( const CNormalVar &s );
	virtual ~CNormalVar();
	inline int	GetDataType( CString strDataType ) ;
	inline CString	GetDataTypeName( int nType ) ;
public:
	CString m_strName;
	int m_nVarDataType;
	int m_nKey;
	CString m_strVariable;
	CString m_strDescription;
public:
	virtual void Serialize(CArchive &ar);
};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CNormalVar*> CNormalVarList;


////////////////////////////////////////////////////////////////
//CParameter
///////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CParameter : public CObject  
{
protected:
	DECLARE_SERIAL(CParameter);
public:
	CParameter();
	CParameter( const CParameter &s );
	CParameter & operator =( const CParameter &s );
	virtual ~CParameter();
public:
	int     m_nKey;//������Key
	CString m_strName;//������
	int		m_nVarKey;//������Ӧ������Key
	CString m_strVariable;//������Ӧ��������ı���
	CString m_strDescription;//��������
	CString m_strValues;//����ֵ
public:
	virtual void Serialize(CArchive &ar);
};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CParameter*> CParameterList;

////////////////////////////////////////////////////////////////
//CRecordsetVar
///////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CRecordsetVar : public CObject  
{
protected:
	DECLARE_SERIAL(CRecordsetVar);
public:
	CRecordsetVar();
	CRecordsetVar( const CRecordsetVar &s );
	CRecordsetVar & operator =( const CRecordsetVar &s );
	virtual ~CRecordsetVar();
public:
	CString m_strName;
	int m_nVarDataType;
	int m_nKey;

	
public:
	int m_nSQLType;
	CString m_strDataBase;
	CString m_strSQL;

	virtual void Serialize(CArchive &ar);
};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CRecordsetVar*> CRecordsetVarList;
////////////////////////////////////////////////////////////////
//CSystemVar
///////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CSystemVar : public CObject  
{
protected:
	DECLARE_SERIAL(CSystemVar);
public:
	CSystemVar();
	CSystemVar( const CSystemVar &s );
	CSystemVar & operator =( const CSystemVar &s );
	virtual ~CSystemVar();
public:
	CString m_strName;
	int m_nVarDataType;
	int m_nKey;
public:
	virtual void Serialize(CArchive &ar);
};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CSystemVar*> SystemVariableList;


//////////////////////////////////////////////////////////////////////////
//CLineObj
///////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS  CLineObj : public CObject  
{
protected:
	DECLARE_SERIAL(CLineObj);
public:
	CLineObj();
	CLineObj( const CLineObj &s );
	CLineObj & operator =( const CLineObj &s );
	virtual ~CLineObj();

public:

	inline void OnDrawRubberLine( CWnd* pWnd , CPoint point );
	void OnDraw(CDC *pDC);
	virtual void Serialize(CArchive& ar);

public:
	CRect m_BeginRect;
	CRect m_EndRect;
	CPoint m_OldPoint;
	int m_nOffset;//ƫ��������Բ��


	CPoint m_BeginPoint;
	CString m_strCaption;
	COLORREF m_crLineColor;
	COLORREF m_crOldLineColor;
};

//////////////////////////////////////////////////////////////////////////
//CNextNode
///////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CNextNode : public CObject  
{
protected:
	DECLARE_SERIAL(CNextNode);
public:
	inline BOOL Intersects(const CRect &rect);//�������ж�
	inline void SetNodeInfo(CPoint point, LPCTSTR lpszName, LPCTSTR lpszData, int nDataType, int nType ,int nIndex );
	inline void MoveTo( CPoint point , CRect BeginRect );
	inline void OnDraw(CDC *pDC  );
	inline void SetNodeObj( LPCTSTR lpszName, int nType );
	CNextNode();
	CNextNode( const CNextNode &s );
	CNextNode & operator =( const CNextNode &s );

	virtual ~CNextNode();
public:
	inline void DisConnectToNodeObj( void );
	inline void ConnectToNodeObj(const char *pszNodeObjName, int nNodeObjType, int nNodeObjKey, CRect endRect );
	inline void OnDrawRubberLine( CWnd* pWnd , CPoint point );
	inline CPoint GetLinePoint( void );
	inline BOOL HitTest(CPoint point);
	long GetHigh( void );
	virtual void Serialize(CArchive& ar);
	inline void UpdateNodeInfo(CPoint point, int nIndex );
public:
	
	CString m_strData;//�����Լ�����
	int  m_nDataType;//�Լ��������ͣ�
	CString m_strName;//����
	CRect m_NodeObjRect;
	CString m_strNodeObjName;//��֮��صĽ��������ƣ����Ϊ""��ʾû����ض���
	int  m_nNodeObjType;//��֮��صĽ��������ͣ��������0��ʾû����ض���
	CLineObj m_LineObj;
	int m_nNodeObjKey;//����
	int m_nType;//0:Next; 1:Error; �����ض����ͱ������10
	CRect m_rect;

private:
	int m_nIndex;
	long m_width;
	long m_high;
};

typedef AFX_EXT_CLASS CTypedPtrList<CObList, CNextNode*> CNextNodeList;

//////////////////////////////////////////////////////////////////////////
//CNodeData
///////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CNodeData  : public CObject  
{
protected:
	DECLARE_SERIAL(CNodeData);
public:
	CNodeData();
	CNodeData( const CNodeData &s );
	CNodeData & operator =( const CNodeData &s );
	virtual ~CNodeData();
public:
	CString m_strData;//���ݽ�㱣������
	int  m_nDataType;//���ݽ����������
	CString m_strName;//���ݽ������
	/*************************************************
	//���洢����ʱ����Ϊ�����ı�־����ʼ��ֵΪ0��
	

	**************************************************/
	int  m_nKey;
public:
	virtual void Serialize(CArchive &ar);

};
typedef AFX_EXT_CLASS CTypedPtrList<CObList, CNodeData*> CNodeDataList;

/////////////////////////////////////////////////////////////////////
//CNodeObj
//
////////////////////////////////////////////////////////////////////


class AFX_EXT_CLASS CNodeObj : public CObject  
{
protected:
	DECLARE_SERIAL(CNodeObj);
	CNodeObj();
public:
	int m_nClassType;//0:����1��2��3��������
	int m_nBlockKey;
	//�������Script����ôm_nBlockKey = 0; 
	//��������ĸ�Child Block����ôm_nBlockKey�͵��ڸ�Child Block��key
public:
	
	CNodeObj(CPoint point);
	CNodeObj( const CNodeObj &s );
	CNodeObj & operator =( const CNodeObj &s );

	virtual ~CNodeObj();
public:

	CNextNodeList m_NextNodeList;//������ص���һ��Nodeobj����Ϣ
	CNodeDataList m_NodeDataList;//������ص�NodeData����Ϣ
	CRect	m_rect;//������ľ���
	CString	m_strCaption;//������ı���

	int		m_nType;//�ӵ���������
	/*************************************************************/
	//��ϵͳ�����ű�ʱ��ÿ���ű��ļ���������ӵ�һ������ʱ��ʼ������
	//�û���Ϊ100��ÿ����һ�����󣬸û�����һ�õ��µĻ�����
	//m_nKey = ���µĻ��������Ա�ʶ�ö��������������ű��ļ����
	//�ж���ĸ�ֵ��Ψһ�ģ�ͬʱ�����Ļ����洢���ű��ļ��У����ļ�
	//���´򿪱༭ʱ���Ըû���Ϊ�����������Դ���Ϊ���������m_nKeyֵ��
	//
	int     m_nKey;
	/*************************************************************/

	UINT    m_nIcoID; //ͼ��ID
	HICON	m_hIcon;//�������ͼ��

public:
	void RemoveAllNextNode( void );
	CDBConnVar * FindDBConnectVariable( CDBConnVarList* pDBLst, int nKey);
	CDLLVariable * FindDLLVariable(CFDLLVariableList *pDLLlst, int nKey);
	CRecordsetVar * FindRecordsetVar(CRecordsetVarList *pRSlst, int nKey);
	void ChangeLineColor( CPoint point );
	inline int GetVarKey( CString strVar );
	CString m_strControlMsg;
	CString m_strDescription;
	BOOL CheckConnectToThis( int nThisKey );
	void DisConnectToNode( int nKey );
	inline CString GetNodeObjTypeName();
	CNextNode * FindNextNodeObj( CString strName );
	void UpDate( void );
	CNextNode * FindNextNodeObj( int nType );
	void RemoveNodeData( void);
	void  MoveLineObj( int nNodeObjKey ,CRect EndRect  );
	inline CPoint GetLinePoint();
	void SetSelected( BOOL bSelected );

	CRect m_MoveRect;//���ڱ����϶�ʱ���߼�λ�þ���
	CNextNode * HitTestNextNode(CPoint point);
	long GetWidth( void );
	long GetHigh(void);
	inline BOOL HitTest( CPoint point );
	virtual void Serialize(CArchive& ar);
	void Remove(void);
	void RemoveNextNodeObj( CNextNode *pObj );
	BOOL AddNextNodeObj( LPCTSTR lpszName, LPCTSTR lpszData, int nDataType, int nType, int nIndex );
	inline void SetBaseInfo(  LPCTSTR lpszCaption, HICON hIcon , int nType ,int nKey );
	inline BOOL Intersects(const CRect& rect);
	void MoveTo( CPoint point);
	void OnDraw(CDC* pDC);
	inline int	GetDataType( CString strDataType ) ;
	inline CString	GetDataTypeName( int nType ) ;
private:
	long	m_width;//������Ŀ��
	long	m_high;//������ĸ߶�
	BOOL	m_bSelected;////�������Ƿ�ѡ��

private:

	void OnDrawCaption(CDC *pDC );
	void OnDrawIco(CDC *pDC, HICON hIcon );
	void OnDrawBaseRect(CDC *pDC);
	
};

typedef AFX_EXT_CLASS CTypedPtrList<CObList, CNodeObj*> CNodeObjList;


/////////////////////////////////////////////////////////////////////////////
// CCFDocument document
//////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CCFDocument : public CDocument
{

	DECLARE_DYNCREATE(CCFDocument)
public:
	CCFDocument();           // protected constructor used by dynamic creation


// Attributes
public:
	/*************************************************************/
	//��ϵͳ�����ű�ʱ��ÿ���ű��ļ���������ӵ�һ��NodeObj����ʱ��ʼ������
	//�û���Ϊ100��ÿ����һ�����󣬸û�����һ�õ��µĻ�����
	//m_nKey = ���µĻ��������Ա�ʶ�ö��������������ű��ļ����
	//�ж���ĸ�ֵ��Ψһ�ģ�ͬʱ�����Ļ����洢���ű��ļ��У����ļ�
	//���´򿪱༭ʱ���Ըû���Ϊ�����������Դ���Ϊ���������m_nBaseKeyֵ��
	//
	int     m_nBaseKey;
	/*************************************************************/
	int     m_nVarBaseKey;
	CNodeObjList m_objects;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCFDocument)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	int m_nDocType ;//0 cef�ļ� 1 cmf�ļ�
	CParameterList m_ParameterLst;//cmf�ļ������б�
	CRecordsetVarList m_RecordsetVariList;
	CDBConnVarList m_DBConnVarList;
	CNormalVarList m_NormalVarList;
	CFDLLVariableList m_DLLVariableList;
	virtual ~CCFDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCFDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





class AFX_EXT_CLASS CWaitCallObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CWaitCallObj);
	CWaitCallObj();
public:
	CWaitCallObj( CPoint point );
	CWaitCallObj( const CWaitCallObj &s );
	CWaitCallObj & operator =( const CWaitCallObj &s );
	virtual ~CWaitCallObj();
public:
	CNodeData m_CalledID;
	BOOL m_bRecCalledID;
	CNodeData m_CallerID;
	BOOL m_bRecCallerID;
	BOOL m_bClearDTMF;
	int m_nRings;
	BOOL m_bAnswerCall;	
	
public:
	virtual void Serialize(CArchive& ar);
};



class AFX_EXT_CLASS CPlayStartObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CPlayStartObj);
	CPlayStartObj();
public:
	CPlayStartObj( CPoint point );
	CPlayStartObj( const CPlayStartObj &s );
	CPlayStartObj & operator =( const CPlayStartObj &s );
	virtual ~CPlayStartObj();
public:
	CString m_strSaveDTMF;
	BOOL m_bRecDTMF;
	BOOL m_bClearDTMF;
	BOOL m_bPlayStop;
	CNodeDataList m_PlayList;
	
	CString	m_strConfNO;
	int		m_nPlayWay;

public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDialCheckObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDialCheckObj);
	CDialCheckObj();
public:
	CDialCheckObj( CPoint point );
	CDialCheckObj( const CDialCheckObj &s );
	CDialCheckObj & operator =( const CDialCheckObj &s );
	virtual ~CDialCheckObj();
public:
	BOOL	m_bAfterRings;
	BOOL	m_bNotAnswer;
	int		m_nRings;
	int		m_nTimes;
	CString	m_strPhone;
	int		m_nChnlType;
	CString	m_strPrefix;
public:
	BOOL m_bConnect;
	BOOL m_bBusy;
	BOOL m_bNoDialTone;
	BOOL m_bNoAnswer;
	BOOL m_bNoRingBack;
	BOOL m_bError;
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CReleaseCallObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CReleaseCallObj);
	CReleaseCallObj();
public:
	CReleaseCallObj( CPoint point );
	CReleaseCallObj( const CReleaseCallObj &s );
	CReleaseCallObj & operator =( const CReleaseCallObj &s );
	virtual ~CReleaseCallObj();
public:
	BOOL	m_bSaveDetail;

public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CCallHoldObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCallHoldObj);
	CCallHoldObj();
public:
	CCallHoldObj( CPoint point );
	CCallHoldObj( const CCallHoldObj &s );
	CCallHoldObj & operator =( const CCallHoldObj &s );
	virtual ~CCallHoldObj();
public:
	BOOL m_bPlayVox;//�Ƿ��ͨ������һ����������
	CString m_strPlayFile;
	int m_nCallHold;//hold ���ͣ�0:call hold 1:pickup from hold

public:
	virtual void Serialize(CArchive& ar);
};



class AFX_EXT_CLASS CTransferCheckObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CTransferCheckObj);
	CTransferCheckObj();
public:
	CTransferCheckObj( CPoint point );
	CTransferCheckObj( const CTransferCheckObj &s );
	CTransferCheckObj & operator =( const CTransferCheckObj &s );
	virtual ~CTransferCheckObj();
public:
	BOOL	m_bClearDTMF;
	BOOL	m_bAfterRings;
	BOOL	m_bNotAnswer;
	int		m_nRings;
	int		m_nTimes;
	int     m_nWaitOption;
	CString m_strPlayFile;
	int		m_nDialType;
	CString	m_strExpression;
	int		m_nTransferAim;//0 ת�Ƶ���ϯ,1 ת�Ƶ����ߣ�2 ת�Ƶ�����
	int     m_nTransferType;//0 ��ʼת��,1 ȡ��ת�ƣ�2 ���ת��
	CString	m_strCancelDTMF;
	BOOL	m_bTalkWitDN;
	CString	m_strCompleteDTMF;
public:
	virtual void Serialize(CArchive& ar);
};



class AFX_EXT_CLASS CCallLogRequestObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCallLogRequestObj);
	CCallLogRequestObj();
public:
	CCallLogRequestObj( CPoint point );
	CCallLogRequestObj( const CCallLogRequestObj &s );
	CCallLogRequestObj & operator =( const CCallLogRequestObj &s );
	virtual ~CCallLogRequestObj();
public:
	int	m_nRequestType;

public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CDLLLoadObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDLLLoadObj);
	CDLLLoadObj();
public:
	CDLLLoadObj( CPoint point );
	CDLLLoadObj( const CDLLLoadObj &s );
	CDLLLoadObj & operator =( const CDLLLoadObj &s );
	virtual ~CDLLLoadObj();
public:
	int m_nDLLKey;
	int m_nHandleType;
	CString m_strDLLFile;
	
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CDLLCallObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDLLCallObj);
	CDLLCallObj();
public:
	CDLLCallObj( CPoint point );
	CDLLCallObj( const CDLLCallObj &s );
	CDLLCallObj & operator =( const CDLLCallObj &s );
	virtual ~CDLLCallObj();
public:
	int m_nDLLKey;
	int m_nHandleType;
	CString m_strDLL;
	CString m_strFunction;	
	
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CDLLUnLoadObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDLLUnLoadObj);
	CDLLUnLoadObj();
public:
	CDLLUnLoadObj( CPoint point );
	CDLLUnLoadObj( const CDLLUnLoadObj &s );
	CDLLUnLoadObj & operator =( const CDLLUnLoadObj &s );
	virtual ~CDLLUnLoadObj();
public:
	int m_nDLLKey;
	int m_nHandleType;
	CString m_strDLL;
	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDBOpenObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDBOpenObj);
	CDBOpenObj();
public:
	CDBOpenObj( CPoint point );
	CDBOpenObj( const CDBOpenObj &s );
	CDBOpenObj & operator =( const CDBOpenObj &s );
	virtual ~CDBOpenObj();
public:
	int m_nDBKey;
	int m_nDBType;
	CString m_strConn;
	CString m_strDBName;
	
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CSQLExcuteObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CSQLExcuteObj);
	CSQLExcuteObj();
public:
	CSQLExcuteObj( CPoint point );
	CSQLExcuteObj( const CSQLExcuteObj &s );
	CSQLExcuteObj & operator =( const CSQLExcuteObj &s );
	virtual ~CSQLExcuteObj();
public:
	int m_nRecordsetKey;
	CString m_strRecordset;
	int m_nSQLType;
	CString m_strSQL;
	
	int m_nDBKey;
	int m_nDBType;
	CString m_strDB;
	
	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDBBrowseObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDBBrowseObj);
	CDBBrowseObj();
public:
	CDBBrowseObj( CPoint point );
	CDBBrowseObj( const CDBBrowseObj &s );
	CDBBrowseObj & operator =( const CDBBrowseObj &s );
	virtual ~CDBBrowseObj();
public:
	int m_nRecordsetKey;
	CString m_strRecordset;
	int m_nBrowseType;

	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDBCloseObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDBCloseObj);
	CDBCloseObj();
public:
	CDBCloseObj( CPoint point );
	CDBCloseObj( const CDBCloseObj &s );
	CDBCloseObj & operator =( const CDBCloseObj &s );
	virtual ~CDBCloseObj();
public:
	int m_nDBKey;
	int m_nDBType;
	CString m_strDB;
	
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CDTMFReciveObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDTMFReciveObj);
	CDTMFReciveObj();
public:
	CDTMFReciveObj( CPoint point );
	CDTMFReciveObj( const CDTMFReciveObj &s );
	CDTMFReciveObj & operator =( const CDTMFReciveObj &s );
	virtual ~CDTMFReciveObj();
public:
	CNodeData m_SaveDTMF;
	BOOL m_bClearDTMF;
	int m_nEndCondition;
	int m_nDTMFCount;
	CString	m_strDTMF;
	int m_nTimeOut;//��ʱ����λ��
	int m_nDTMFType;//����DTMF������ 0��������DTMF��1ֻ�������ּ�
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CEventCheckObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CEventCheckObj);
	CEventCheckObj();
public:
	CEventCheckObj( CPoint point );
	CEventCheckObj( const CEventCheckObj &s );
	CEventCheckObj & operator =( const CEventCheckObj &s );
	virtual ~CEventCheckObj();
public:
	BOOL m_bError;
	BOOL m_bOffHook;
	BOOL m_bHangUp;
	BOOL m_bHookFlash;
	BOOL m_bRecDTMF;
	BOOL m_bRecFaxTone;
	BOOL m_bAppExit;
	BOOL m_bFaxSendTone;
	BOOL m_bConfDestroyed;
	BOOL m_bAgentLoginMsg;
	BOOL m_bAgentLogOutMsg;
	BOOL m_bAgentBusyMsg;
	BOOL m_bAgentInserveMsg;
	BOOL m_bAgentAnswerMsg;
	BOOL m_bAgentMakeCallMsg;
	BOOL m_bAgentTransferMsg;
	BOOL m_bAgentConferMsg;
	BOOL m_bAgentHangUpMsg;
	BOOL m_bAgentSendDataMsg;
	BOOL m_bAgentSendFaxMsg;
	BOOL m_bRinging;
	BOOL m_bNothing;
	BOOL m_bAgentHold;
	BOOL m_bAgentPickupHold;
	BOOL m_bAgentMonitor;
	BOOL m_bAgentDisMonitor;
	BOOL m_bAgentBuildConfer;
	BOOL m_bElapseTime;
	BOOL m_bAgentGetActiveCall;
public:
	int  m_nElapseTime;
	BOOL m_bClearDTMF;
	BOOL m_bPlay;
	CString m_strPlayFile;
	int		m_nPlayType;
	CString	m_strPlayDTMF;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CSwitchObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CSwitchObj);
	CSwitchObj();
public:
	CSwitchObj( CPoint point );
	CSwitchObj( const CSwitchObj &s );
	CSwitchObj & operator =( const CSwitchObj &s );
	virtual ~CSwitchObj();
public:
	int		m_nSwitchType;//Switch����
	CString m_strAimChnl;
	int		m_nAimType;//0:System free channel 1:From
	int		m_nWaitOption;//0:play ringback tone 1:not tone 2:play file	
	CString	m_strPlayFile;
	

	int		m_nGetFreeChWay;//ȡ�������Ⲧ������ͨ����ʽ��0������������Ѱ��һ������ͨ����1��ָ����Χ��������Ѱ��һ������ͨ��
	CString	m_strBeginCH;
	CString	m_strEndCH;
	int		m_nTimeOut;//��CallAgent��CallCustomer��������Ϊ���г�ʱ

public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CRouteCallObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CRouteCallObj);
	CRouteCallObj();
public:
	CRouteCallObj( CPoint point );
	CRouteCallObj( const CRouteCallObj &s );
	CRouteCallObj & operator =( const CRouteCallObj &s );
	virtual ~CRouteCallObj();
public:
	CString m_strBeginCh;
	CString m_strEndCh;
	CString m_strTel;
	int		m_nWaitOption;//0:play ringback tone 1:not tone 2:play file	
	CString	m_strWaitFile;
	int		m_nConnOption;//0:play ringback tone 1:not tone 2:play file	
	CString	m_strConnFile;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CCompareObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCompareObj);
	CCompareObj();
public:
	CCompareObj( CPoint point );
	CCompareObj( const CCompareObj &s );
	CCompareObj & operator =( const CCompareObj &s );
	virtual ~CCompareObj();
public:
	int		m_nVarKey;
	CString	m_strVarName;
	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDTMFMenuObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDTMFMenuObj);
	CDTMFMenuObj();
public:
	CDTMFMenuObj( CPoint point );
	CDTMFMenuObj( const CDTMFMenuObj &s );
	CDTMFMenuObj & operator =( const CDTMFMenuObj &s );
	virtual ~CDTMFMenuObj();
public:
	BOOL m_bClearDTMF;
	int m_nTimeOut;
	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CRecordObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CRecordObj);
	CRecordObj();
public:
	CRecordObj( CPoint point );
	CRecordObj( const CRecordObj &s );
	CRecordObj & operator =( const CRecordObj &s );
	virtual ~CRecordObj();
public:
	CString m_strFileName;
	CString	m_strFolder;
	int		m_nFileLength;
	int		m_nRecordType;//0��ͨ¼�� 1����¼��
	CString	m_strConferNO;
	CString m_strEndDTMF;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CFaxReciveObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CFaxReciveObj);
	CFaxReciveObj();
public:
	CFaxReciveObj( CPoint point );
	CFaxReciveObj( const CFaxReciveObj &s );
	CFaxReciveObj & operator =( const CFaxReciveObj &s );
	virtual ~CFaxReciveObj();
public:
	CString m_strFileName;
	CString	m_strFolder;
	CString	m_strPage;
	
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CFaxSendObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CFaxSendObj);
	CFaxSendObj();
public:
	CFaxSendObj( CPoint point );
	CFaxSendObj( const CFaxSendObj &s );
	CFaxSendObj & operator =( const CFaxSendObj &s );
	virtual ~CFaxSendObj();
public:
	CString m_strFileName;
	CString	m_strFolder;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CConferenceBulidObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CConferenceBulidObj);
	CConferenceBulidObj();
public:
	CConferenceBulidObj( CPoint point );
	CConferenceBulidObj( const CConferenceBulidObj &s );
	CConferenceBulidObj & operator =( const CConferenceBulidObj &s );
	virtual ~CConferenceBulidObj();
public:
	BOOL m_nBuildType;
	CString m_strConferNo;
	CString m_strMaxMember;
public:
	virtual void Serialize(CArchive& ar);
};



class AFX_EXT_CLASS CConferenceDeleteObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CConferenceDeleteObj);
	CConferenceDeleteObj();
public:
	CConferenceDeleteObj( CPoint point );
	CConferenceDeleteObj( const CConferenceDeleteObj &s );
	CConferenceDeleteObj & operator =( const CConferenceDeleteObj &s );
	virtual ~CConferenceDeleteObj();
public:
	BOOL m_nDeleteType;
	//ɾ�����ͣ�һ���Ǹ���ϵͳ��ɾ����ȡ��ͨ��ָ���Ļ���
	//һ���Ǹ�������Ļ����ţ�ɾ���û���
	//ɾ������ʱ���ѻ����������ͨ���������ȥ��ͬʱ�ͷŻ�����Դ
	CString m_strConferNo;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CConferencePlayObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CConferencePlayObj);
	CConferencePlayObj();
public:
	CConferencePlayObj( CPoint point );
	CConferencePlayObj( const CConferencePlayObj &s );
	CConferencePlayObj & operator =( const CConferencePlayObj &s );
	virtual ~CConferencePlayObj();
public:
	CString	m_strConferNO;
	CString	m_strFileExpress;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CConferenceRecordObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CConferenceRecordObj);
	CConferenceRecordObj();
public:
	CConferenceRecordObj( CPoint point );
	CConferenceRecordObj( const CConferenceRecordObj &s );
	CConferenceRecordObj & operator =( const CConferenceRecordObj &s );
	virtual ~CConferenceRecordObj();
public:
	CString	m_strConferNO;
	CString	m_strFileExpress;
	UINT	m_nFileLength;
	CString	m_strFolder;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CGetConferMemberObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CGetConferMemberObj);
	CGetConferMemberObj();
public:
	CGetConferMemberObj( CPoint point );
	CGetConferMemberObj( const CGetConferMemberObj &s );
	CGetConferMemberObj & operator =( const CGetConferMemberObj &s );
	virtual ~CGetConferMemberObj();
public:
	CString m_strConferNo;
	CString m_strMemberCount;//��ǰ�����Ա��
	CString m_strMaxMemberNum; //�����������Ա��
	CString m_strMaxSpeakerNum; //�����ܹ���������Ա��
	CString m_strMaxSpeakingNum; //����ͬʱ��������Ա��
	CString m_strListenNum; //��ǰ������Ա��
	CString m_strSpeakerNum; //��ǰ�����Գ�Ա��
	CString m_strSpeakingNum; //��ǰ���ڷ��Գ�Ա��

public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CEnterConferObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CEnterConferObj);
	CEnterConferObj();
public:
	CEnterConferObj( CPoint point );
	CEnterConferObj( const CEnterConferObj &s );
	CEnterConferObj & operator =( const CEnterConferObj &s );
	virtual ~CEnterConferObj();
public:
	CString m_strConferNO;
	int m_nAction;
	CString m_strChnlIndex;//��������ͨ���ţ�-1��ʾ�Լ��������
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CLeaveConferObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CLeaveConferObj);
	CLeaveConferObj();
public:
	CLeaveConferObj( CPoint point );
	CLeaveConferObj( const CLeaveConferObj &s );
	CLeaveConferObj & operator =( const CLeaveConferObj &s );
	virtual ~CLeaveConferObj();
public:
	CString m_strConferNO;
	CString m_strChnlIndex;//��������ͨ���ţ�-1��ʾ�Լ��������

public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CDTMFSendObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CDTMFSendObj);
	CDTMFSendObj();
public:
	CDTMFSendObj( CPoint point );
	CDTMFSendObj( const CDTMFSendObj &s );
	CDTMFSendObj & operator =( const CDTMFSendObj &s );
	virtual ~CDTMFSendObj();
public:
	CString m_strDTMF;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CConferRequestObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CConferRequestObj);
	CConferRequestObj();
public:
	CConferRequestObj( CPoint point );
	CConferRequestObj( const CConferRequestObj &s );
	CConferRequestObj & operator =( const CConferRequestObj &s );
	virtual ~CConferRequestObj();
public:
	int m_nDialType;
	CString m_strDialTel;
	int m_nAction;//����������ʽ��SpeakListen ListenOnly SpeakOnly
public:
	virtual void Serialize(CArchive& ar);
};

/************************************************************
CBlockObj�ĳ�Ա�ض���enter��return�������͵ĸ�һ���ڵ㣬
�ض���һ��CNEXTObj��ͬʱ���е�CNEXTObj�ض���һ��return���������
CNEXTObj��m_strName==return��m_strCaption
CNEXTObj��m_nType==return��m_nKey

CNEXTObj��m_strNodeObjName==���ӵ��Ľӵ�����Description
CNEXTObj��m_nNodeObjKey==���ӵ��Ľӵ�����m_nKey

*************************************************************/

class AFX_EXT_CLASS CBlockObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CBlockObj);
	CBlockObj();
public:
	CBlockObj( CPoint point );
	CBlockObj( const CBlockObj &s );
	CBlockObj & operator =( const CBlockObj &s );
	virtual ~CBlockObj();
public:
	CNodeObjList m_ChildObjLst;
public:
	void PasteReplcaeDBBrowse(CBlockObj *pBlock, int nRSKey, int nNewRSKey,CString strNewRecordset );
	CNodeObj * FindNodeObj( int nKey, int &nParentKey );
	int PasteNodeObj(CDBConnVarList* pDBLst,CFDLLVariableList *pDLLlst, CRecordsetVarList *pRSlst, CBlockObj *pBlock, int nBaseKey );
	void PasteReplaceNextKey( CBlockObj *pBlock,int nNodeKey , int nNewNodeKey );
	CNodeObj * FindObj( int nKey );
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CStoreProObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CStoreProObj);
	CStoreProObj();
public:
	CStoreProObj( CPoint point );
	CStoreProObj( const CStoreProObj &s );
	CStoreProObj & operator =( const CStoreProObj &s );
	virtual ~CStoreProObj();
public:
	CString	m_strStoreProName;//�洢������
	CString	m_strParameterVar;//�������������
	CString m_strRecordset;//�洢���̷��صļ�¼��
	
	CString m_strReturn;
	BOOL m_bReturnRecordSet;

	int m_nRecordsetKey;
	int m_nDBKey;
	int m_nDBType;
	CString m_strDB;

public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CChangeVoiceObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CChangeVoiceObj);
	CChangeVoiceObj();
public:
	CChangeVoiceObj( CPoint point );
	CChangeVoiceObj( const CChangeVoiceObj &s );
	CChangeVoiceObj & operator =( const CChangeVoiceObj &s );
	virtual ~CChangeVoiceObj();
public:
	int		m_nChangeVoiceType;//�����ĳ���
	
public:
	virtual void Serialize(CArchive& ar);
};


class AFX_EXT_CLASS CEndCallBlockObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CEndCallBlockObj);
	CEndCallBlockObj();
public:
	CEndCallBlockObj( CPoint point );
	CEndCallBlockObj( const CEndCallBlockObj &s );
	CEndCallBlockObj & operator =( const CEndCallBlockObj &s );
	virtual ~CEndCallBlockObj();
public:
	CNodeObjList m_ChildObjLst;
public:
	CNodeObj * FindNodeObj(int nKey, int &nParentKey);
	CNodeObj * GetEndCallEventCheckObj( void );
	CNodeObj * FindObj( int nKey );
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CFunctionObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CFunctionObj);
	CFunctionObj();
public:
	CFunctionObj( CPoint point );
	CFunctionObj( const CFunctionObj &s );
	CFunctionObj & operator =( const CFunctionObj &s );
	virtual ~CFunctionObj();
public:
	CString m_strFunction;
	CString	m_strReturnValues;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CExpressObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CExpressObj);
	CExpressObj();
public:
	CExpressObj( CPoint point );
	CExpressObj( const CExpressObj &s );
	CExpressObj & operator =( const CExpressObj &s );
	virtual ~CExpressObj();
public:
	CString	m_strExpressValues;
	CString m_strExpress;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CGoToObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CGoToObj);
	CGoToObj();
public:
	CGoToObj( CPoint point );
	CGoToObj( const CGoToObj &s );
	CGoToObj & operator =( const CGoToObj &s );
	virtual ~CGoToObj();
public:
	int m_nNextObjKey;
	CString m_strConnPath;//����Ŀ������·��
public:
	void DisConnectToNode(int nKey);
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CTraceObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CTraceObj);
	CTraceObj();
public:
	CTraceObj( CPoint point );
	CTraceObj( const CTraceObj &s );
	CTraceObj & operator =( const CTraceObj &s );
	virtual ~CTraceObj();
public:
	CString m_strTrace;
public:
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CModuleObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CModuleObj);
	CModuleObj();
public:
	CModuleObj( CPoint point );
	CModuleObj( const CModuleObj &s );
	CModuleObj & operator =( const CModuleObj &s );
	virtual ~CModuleObj();
public:
	CNodeObj *m_pCurObj;
	CNodeObjList m_objects;
	CRecordsetVarList m_RecordsetVariList;
	CDBConnVarList m_DBConnVarList;
	CNormalVarList m_NormalVarList;
	CFDLLVariableList m_DLLVariableList;

	CParameterList m_ParameterLst;//ģ��ڵ�����б�
	CString m_strFile;//����ģ���ļ��ľ���·���Լ��ļ���
public:
	void UpdateParameterList( CParameterList *p );
	void RemoveParameterList( void );
	virtual void Serialize(CArchive& ar);
};

class AFX_EXT_CLASS CCCCallAgentObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCCCallAgentObj);
	CCCCallAgentObj();
public:
	CCCCallAgentObj( CPoint point );
	CCCCallAgentObj( const CCCCallAgentObj &s );
	CCCCallAgentObj & operator =( const CCCCallAgentObj &s );
	virtual ~CCCCallAgentObj();
public:
	UINT	m_nTimeOut;
	int		m_nWaitType;
	CString	m_strWaitPlayFile;
	int		m_nPlayType;
	CString	m_strAimDN;
	BOOL	m_bIsPlayBeforeTalk;

	CNodeDataList m_PlayList;
	

public:
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
// CModuleDocument document

class AFX_EXT_CLASS CModuleDocument : public CDocument
{
	DECLARE_DYNCREATE(CModuleDocument)
public:
	CModuleDocument();  


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleDocument)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CModuleObj m_ModuleObj;
	virtual ~CModuleDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CModuleDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern "C" _declspec(dllexport) void AssignsObjList( const CNodeObjList &s ,CNodeObjList *d,int nAssigns );


class AFX_EXT_CLASS CCustomObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCustomObj);
	CCustomObj();
public:
	CCustomObj( CPoint point );
	CCustomObj( const CCustomObj &s );
	CCustomObj & operator =( const CCustomObj &s );
	virtual ~CCustomObj();
public:
	POSITION	m_pos;
	CString     m_strFunName;
	int         m_nFunType;
	BOOL		m_bEnd;//�Ƿ�ĩβ��
public:
	void MoveFirst( void );
	POSITION MoveNextFun( void );
	virtual void Serialize(CArchive& ar);
};




class AFX_EXT_CLASS CCCTransferObj : public CNodeObj  
{
protected:
	DECLARE_SERIAL(CCCTransferObj);
	CCCTransferObj();
public:
	CCCTransferObj( CPoint point );
	CCCTransferObj( const CCCTransferObj &s );
	CCCTransferObj & operator =( const CCCTransferObj &s );
	virtual ~CCCTransferObj();
public:
	int		m_nTransferType;
	UINT	m_nTimeOut;
	int		m_nWaitType;
	CString	m_strPlayFile;
	CString	m_strCompleteDTMF;
	CString	m_strCancelDTMF;
	CString	m_strAimDN;
	BOOL	m_bTimeOut;
	BOOL	m_bTalkWitDN;
	
public:
	virtual void Serialize(CArchive& ar);
};
#endif 