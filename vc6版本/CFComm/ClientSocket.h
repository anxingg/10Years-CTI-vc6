
#ifndef CLENTSOCKET_H
#define CLENTSOCKET_H


const int CLIENT_SOCKET_BUF_LEN = 241;//socket���պͷ������ݻ�������С
/*==================================================================
note: CLIENT_SOCKET_BUF_LEN > sizeof(CFC_MESSAGE_PACK) + 1
===================================================================*/

//recive STATE const define
const int CLIENT_SOCKET_STATE1 = 0;//bengin recive
const int CLIENT_SOCKET_STATE2 = 1;
const int CLIENT_SOCKET_STATE3 = 2;

//define socket recive buf result enum type
typedef enum
{
	Unfinishing,
	Finishing,
	Closed,
	Error

}Client_SocketRecResult_T;


//==========================================================================//
//CClientSocket��һ��������ģʽ��Socket�࣬��ʹ��Ioctlsocket(long cmd, u_long *argp)
//��������ģʽģʽʱһ������Ϊ������ģʽ
//
//==========================================================================//
class CClientSocket  
{
public:

	Client_SocketRecResult_T Recive(char *Buf, int nBufLen);
	Client_SocketRecResult_T Recive( char *Buf, char cIdentifier , int nBufLen );
	CClientSocket();
	virtual ~CClientSocket();
	CClientSocket(const CClientSocket& s); 
public:
	bool m_bNetAbend;
	SOCKET Create(int af, int type, int protocol );
	bool ReadBuf( char *buf, int bufLen );
	Client_SocketRecResult_T Send(char *Buf, int nBufLen);
	void SetAddr( sockaddr_in addr);
	int Close();
	int Ioctlsocket(long cmd, u_long *argp);
	bool m_bConnect;
	SOCKET m_socket;
	sockaddr_in m_addr;
private:
	void UpadteSendVariable();
	void UpadteReciveVariable();

	int GetLastError( );
private:
	int m_nRecPos ;
	int m_nRevBytes;
	int m_nReciveState;
	char m_szRecBuf[ CLIENT_SOCKET_BUF_LEN ];

	int m_nSendPos ;
	int m_nSendBytes;
	int m_nSendState;
	char m_szSendBuf[ CLIENT_SOCKET_BUF_LEN ];
	
};

#endif 
