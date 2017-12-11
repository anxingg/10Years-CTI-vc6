#ifndef NORMARCHBOARDS_H
#define NORMARCHBOARDS_H

extern "C" _declspec(dllexport) bool NMB_Init(int nTotalChnl,/*ͨ����*/ 
											  int nBoardsType=0,/*�������:0����ģ�⿨;2һ�����3����һ�����4����7������TUP;5����7������ISUP;*/
											  int nFax=0,/*�Ƿ��д��棺0�޴��棻1�д���*/ 
											  int nChangeVoice=0,/*�Ƿ��б�����0�ޱ�����1�б���*/
											  int conference=0,/*�Ƿ�����ǿ���飺0��ͨ���飻1��ǿ����*/
											  int nSwitchType=0/*ͨ�������ӷ�ʽ��0��ͨ���ӣ�1��������*/
											  );/*��ʼ���忨*/
//ͨ����������
extern "C" _declspec(dllexport) int  NMB_GetChnlType( int nIndex/*ͨ�����*/ );/*ȡ��ͨ������*/
extern "C" _declspec(dllexport) bool NMB_AddIndexPlay( int nIndex, CString strFileName );//�������������ļ�
extern "C" _declspec(dllexport) bool NMB_StartIndexPlay( int nIndex );//��ʼһ���ļ���������
extern "C" _declspec(dllexport) bool NMB_CheckIndexPlayEnd (int nIndex );//��������ļ������Ƿ����
extern "C" _declspec(dllexport) bool NMB_StopIndexPlay( int nIndex );//��ʼһ���ļ���������

//�����������
extern "C" _declspec(dllexport) bool NMB_StartConfPlay( int nGroupNo/*�����*/,char *FileName );//��ʼ�������
extern "C" _declspec(dllexport) void NMB_StopConfPlay( int nGroupNo  );//ֹͣ�������
extern "C" _declspec(dllexport) bool NMB_CheckConfPlayEnd( int nGroupNo );//����������Ƿ����

//ͨ��¼������
extern "C" _declspec(dllexport) bool NMB_StartRecordFile(int nIndex,CString strFileName,DWORD RecordLen );//��ʼ�ļ�¼��
extern "C" _declspec(dllexport) bool NMB_CheckRecordFileEnd(int nIndex );//����ļ�¼���Ƿ����
extern "C" _declspec(dllexport) void NMB_StopRecordFile(int nIndex);//ֹͣ�ļ�¼��

//����¼������
extern "C" _declspec(dllexport) bool NMB_StartConfRecord( int nGroupNo,CString strFileName ,DWORD Length));//��ʼ����¼��
extern "C" _declspec(dllexport) bool NMB_CheckConfRecordEnd( int nGroupNo );//������¼���Ƿ����
extern "C" _declspec(dllexport) void NMB_StopConfRecord( int nGroupNo );//ֹͣ����¼��

//DTMF����
extern "C" _declspec(dllexport) void NMB_InitDtmfBuf( int nIndex );//���ϵͳ��DTMF������
extern "C" _declspec(dllexport) bool NMB_GetDtmfString( int nIndex,CString &strDtmf );//�õ�ͨ����DTMF�����������ӻ���������ȥ
extern "C" _declspec(dllexport) bool NMB_SendDtmfString( int nIndex, CString &strDtmf );//����DTMF������

extern "C" _declspec(dllexport) bool NMB_GetCallerIDStr( int nIndex, CString &strCallerID/*���к���*/, CString &strCalledID/*���к���*/ );//ȡ��������

//extern "C" _declspec(dllexport) int NMB_GetChnlState( int nIndex );//�õ�ͨ��״̬

#endif 