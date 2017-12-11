#ifndef __FIHO_PHONIC_H__
#define __FIHO_PHONIC_H__

#include "ctidef.h"

#ifdef	__cplusplus
extern "C"{
#endif


	
typedef enum enumPhonicEvent
{
	eventState	= 0,		//ͨ��״̬,iParam1=��״̬,iParam2=��״̬
	eventDeviceTimer,		//ͨ����ʱ��
	eventSignal,			//�ѷ���
	eventAlarm,				//�ѷ���
	eventIdle,				//ͨ������
	eventCallIn,			//����,iParam1=ָ����벿�ֵ��ַ�ָ��,iParam2=ָ��Ի�ڲ��ֵ��ַ�ָ��
	eventAnswer,			//����Ӧ������ժ��
	eventCallOutFinish,		//���ߺ������
	eventCallFail,			//��·����ʧ��
	eventHangup,			//��·�һ�
	eventDTMF,				//��⵽DTMF,iParam1=DTMF��ASCII��ֵ
	eventSignalMonitor,		//�ѷ���
	eventPlayEnd,			//��������,iParam1=�ѷ�����ֽ���
	eventRecordEnd,			//¼������,iParam1=��¼����ֽ���
	eventFaxError,			//����ʧ��,iParam1=����ʧ��ԭ��
	eventOneFaxPageOver,	//�������,iParam1=0,���һҳ;iParam1=1,ȫ�����
	eventFlash,				//��⵽�������˲��
	eventSendCallerIDEnd,   //�ѷ���
	eventSendV23End,        //�ѷ���
	eventV23DataReady,      //�ѷ���
	eventRing,              //���� 
	eventVoice,             //��⵽���� 
	eventSilence,           //��⵽���� 
	eventHanging,			//��������
	eventDetectedFaxTone,   //��⵽������ 	  
	eventFaxToneMissing,    //�ѷ���
	eventRealTimeData,      //�ѷ���
	eventAnalogSignal,      //�ѷ���
	eventDtmfOn,            //��⵽��������,iParam1=DTMF��ASCII��ֵ
	eventDtmfOff,           //��⵽��������,iParam1=DTMF��ASCII��ֵ 
	eventPolarity,          //��⵽���߼��Ա仯,iParam1����
	eventEnergy,            //�ѷ���
	eventVad,               //�ѷ���
	eventIPArrive,          //�ѷ���
	eventIPCallFail,        //�ѷ���
	eventIPAnswer,          //�ѷ���
	eventIPHangup,          //�ѷ���   
	eventIPRegisterResult,  //�ѷ���
	eventFlashOver,         //��ʾ�����Ĳ�ɵĶ���(����tpi_FlashEx())���
	eventRingBack,          //��⵽���ߵĻ�����
	eventV21Data,           //�ѷ���  
	eventFaxModulationOn,   //�ѷ���
	eventFaxModulationOff,  //�ѷ��� 
	eventFaxSpeed,          //���浱ǰ�Ĵ�������,iParam1=��������
	eventFaxResolution,     //���浱ǰ�Ĵ���ֱ���,iParam1=����ֱ���
	eventFaxRemoteID,       //���汾�صĴ����ʶ(),iParam1=ָ���ʶ���ַ�ָ��
	eventFaxTCFOK,          //����ѵ������ 
	eventRecordStreamReady, //��ʽ����׼������,iParam1=ָ����ʽ�������ݵ��ַ�ָ��,iParam2=���ݴ�С
	eventDialTone,          //���沦���������,iParam1=0,û�м�⵽������;iParam1=1,��⵽�˲�����
	eventTTSDataReady,
	eventMax
}EPhonicEvent;

                                    
typedef void (CALLBACK * ProcPhonicFireEventCallBack)(
				EPhonicEvent	eventType,
				int				channelType,	
				int				channelID, 
				int				iParam1,
				int				iParam2);

//�¼�֪ͨ�ص�����
void WINAPI tpi_SetEventNotifyCallBackProc( ProcPhonicFireEventCallBack procFireEvent );

int WINAPI tpi_GetPhonicEvent( 
				EPhonicEvent *	pEventType,
				int			 *	pChannelType,	
				int			 *	pChannelID, 
				int			 *	pParam1,
				int			 *	pParam2);

//ϵͳ
int  WINAPI tpi_OpenDevice();
void WINAPI tpi_CloseDevice();


int  WINAPI tpi_GetCardCount(int lCardType);
int  WINAPI tpi_GetChannelCount(int channelType);
int  WINAPI tpi_GetLineCount(void);
int  WINAPI tpi_GetLineID(int channelType, int channelID);
int  WINAPI tpi_GetChannelType(int lineID);
int  WINAPI tpi_GetChannelID(int lineID);



//��ʱ��
int  WINAPI tpi_SetDeviceTimer(int channelType, int channelID, int timer);
int  WINAPI tpi_ClearDeviceTimer(int channelType, int channelID);
int  WINAPI tpi_GetChannelState(int channelType, int channelID);

//���н���
int  WINAPI tpi_GetChannelCallerID(int channelType, int channelID,char * callerIDBuf,char * dateTimeBuf);
int  WINAPI tpi_MakeCall(int channelType, int channelID, const char * callerID, const char * phoneNumber, int overtime);
int  WINAPI tpi_MakeCallEx(int channelType, int channelID, const char * callerID, const char * phoneNumber,int toneTime, int silenceTime, int scale,int overtime);
int  WINAPI tpi_EnableHookOffDetect(int channelType, int channelID, BOOL bEnable);
int  WINAPI tpi_Answer(int channelType, int channelID, int bConn);
int  WINAPI tpi_Hangup(int channelType, int channelID, int cause);

//����
int  WINAPI tpi_TalkWith(int destType, int destID, int srcType, int srcID);
int  WINAPI tpi_BreakTalk(int channelType, int channelID);
int  WINAPI tpi_ListenTo(int destType, int destID, int srcType, int srcID);
int  WINAPI tpi_BreakListen(int destChannelType, int destChannelID);
int  WINAPI tpi_SetLineListenVolume(int channelType, int channelID, int volume);


//¼����
int  WINAPI tpi_SetVoiceVolume(int channelType, int channelID, int volume, int type);
int  WINAPI tpi_SetVoiceCoder(int channelType, int channelID, int lCoder);
int  WINAPI tpi_GetVoiceCoder(int channelType, int channelID);

int  WINAPI tpi_AddPromptVoiceFromFile(const char * fileName);
int  WINAPI tpi_AddPromptVoiceFromMemory(char* pVoiceBuffer, int voiceSize);
void WINAPI tpi_ResetPromptVoice(void);
int  WINAPI tpi_PlayPromptVoice(int channelType, int channelID, int pmtIndex, int loopCount);
int  WINAPI tpi_PlayPromptVoiceArray(int channelType, int channelID, int* pPmtIndexArray, int pmtCount, int loopCount);

int  WINAPI tpi_PlayFile(int channelType, int channelID, const char * fileName, int offset, int voiceSize);
int  WINAPI tpi_PlayMemory(int channelType, int channelID, char* pVoiceBuffer, int voiceSize);
int  WINAPI tpi_PlayTone(int channelType, int channelID, float fFrequency, int dwTimeLength, int dwScale, const char * attrib);
int  WINAPI tpi_SendDtmf(int channelType, int channelID, const char * dtmfString, int toneTime, int silenceTime, int scale);
int  WINAPI tpi_PlayPause(int channelType, int channelID, int bPause);
int  WINAPI tpi_StopPlay(int channelType, int channelID);
int  WINAPI tpi_StopPlayEx(int channelType, int channelID,int voiceType);

int  WINAPI tpi_RecordFile(int channelType, int channelID, const char * fileName, int offset, int voiceSize);
int  WINAPI tpi_RecordMemory(int channelType, int channelID, char* pVoiceBuffer, int voiceSize);
int  WINAPI tpi_RecordPause(int channelType, int channelID, int bPause);
int  WINAPI tpi_StopRecord(int channelType, int channelID);
int  WINAPI tpi_StopRecordEx(int channelType, int channelID,int voiceType);

//��ʽ¼����
int	 WINAPI   tpi_StartRecordStream(int channelType,int channelID, long blocksize);
int	 WINAPI   tpi_PlayStream(int channelType,int channelID,char* pVoiceData, long size);

//TTS
int  WINAPI tpi_PlayText(int channelType, int channelID, const char * text, int isMale, int speed);
int  WINAPI tpi_PlayTextFile(int channelType, int channelID, char* text, int isMale, int speed);
int  WINAPI tpi_ConvertTextToVoiceFile(char *pText, int isMale, int speed, int iTextIsFileName, char* pVocName);

//�绰����
int  WINAPI   tpi_CreateConference();//���ػ�����
int  WINAPI	  tpi_JoinConference(int channelType, int channelID, int conferResourceGrpID, int conferSetID, int Attenuation, int NoiseSuppression);
int  WINAPI   tpi_ListenConference(int channelType, int channelID, int conferResourceGrpID, int conferSetID);
int  WINAPI   tpi_ExitConference(int channelType, int channelID);
int  WINAPI   tpi_GetChannelConferenceHandle(int channelType, int channelID);//���ص绰ͨ�����ڵĻ�����
int  WINAPI   tpi_SetConferenceMemberVolumeEx(int channelType,int channelID,int iVolume,int Type);//Type==0Ϊ�����ԱMic����,Type==1�����ԱSpeaker����
int  WINAPI   tpi_ReleaseConference(int conferenceHandle);



//FAX
int  WINAPI tpi_faxSetResolution( int channelType,int channelID,int res);
int  WINAPI tpi_faxSetLocalID(int channelType, int channelID, const char * IDString);
int  WINAPI tpi_faxSendFax(int channelType, int channelID, const char * fileName);
int  WINAPI tpi_faxReceiveFax(int channelType, int channelID, const char * fileName);
void WINAPI tpi_faxStopFax(int channelType, int channelID);

int  WINAPI tpi_faxOpenConvert();
void WINAPI tpi_faxCloseConvert();
int  WINAPI tpi_faxCreateFaxFile(int cvtChannelID, const char * fileName, int Resolution, int scanLineNum);
void WINAPI tpi_faxCloseFaxFile(int cvtChannelID);
int  WINAPI tpi_faxAddTextLine(int cvtChannelID, const char * strText, int horiDoubleFlag, int vertiDoubleFlag, int FontSize);
int  WINAPI tpi_faxAddBmpFile(int cvtChannelID, const char * BmpFileName, int DoubleBitFlag);
int  WINAPI tpi_faxAddDotLine(int cvtChannelID, const char * szDot, int DotSize, int DotFlag);
int  WINAPI tpi_faxLeftLine(int cvtChannelID);


// ϵ�к�
int  WINAPI  tpi_GetCardPass(int iCardNo);
int  WINAPI  tpi_GetCardUserNo(int iCardNo);
int  WINAPI  tpi_SetVoiceThreshold(int channelType, int channelID,int lVoiceThreshold,int lVoiceCount);
int  WINAPI  tpi_SetSilenceThreshold(int channelType, int channelID,int lSilenceThreshold);
int  WINAPI  tpi_SetSilenceThresholdEx(int channelType, int channelID,int lSilenceThreshold,int lSilenceTime,BOOL bDisableEvent);


int  WINAPI  tpi_Flash(int channelType, int channelID);
int  WINAPI  tpi_FlashEx(int channelType, int channelID,int lFlashTime);
int  WINAPI  tpi_EnableDtmfPredetect(int channelType, int channelID, LONG iEnable);
int  WINAPI  tpi_SetDtmfDetectTime(int channelType, int channelID, int time);
int  WINAPI  tpi_StartDetectDialTone(int channelType, int channelID,int iThreshold=25,int judgeTime=3000);
int  WINAPI  tpi_PowerDown(int channelType, int channelID);
int  WINAPI  tpi_LinePolarity(int channelType, int channelID,int value);
int  WINAPI  tpi_SetRingParameter(int channelType, int channelID,int OnTime,int OffTime);
int  WINAPI  tpi_SetCallerIDType(int channelType, int channelID,int type);
int  WINAPI  tpi_MeasureVoltage(int channelType, int channelID);       //�����������·�ĵ�ѹ
int  WINAPI  tpi_SetVthr1(int channelType, int channelID,int voltage); //���ñ������·ժ���һ���ѹ��ƽ��ֵ
int  WINAPI  tpi_SetVthr0(int channelType, int channelID,int voltage); //���ñ������·���յĵ�ѹ��һ�����õ�����·ժ����ѹ������.
int  WINAPI  tpi_GetVthr1(int channelType, int channelID); //ȡ��ժ���һ���ѹ������ֵ
int  WINAPI  tpi_GetVthr0(int channelType, int channelID); //ȡ����·���յĵ�ѹ������ֵ
int  WINAPI  tpi_MuteEncoderWhenDtmf(long channelType, long channelID, long Mec) ;
int  WINAPI  tpi_PlayFile2(int channelType, int channelID, const char * fileName, int offset, int voiceSize);
#ifdef	__cplusplus
};
#endif

#endif //__FIHO_PHONIC_H__