#ifndef __StructO__h_
#define __StructO__h_

#define WORKBORDNUM			16     //ϵͳ�����Ŀ
#define MAXEXCHLINE			2048   //ϵͳ�����·��Ŀ
#define MAXVOCINDEX			10000  //ϵͳ����ڴ�����������������Ŀ
#define USERMAXVOCINDEX		8192   //�û���ʹ������ڴ�����������������Ŀ,�û�ʹ��Voc0--Voc8191
#define ECONFGROUPCHNUM		32 //ÿ��������ͨ����Ŀ(ÿ��DSP֧�ֵĻ���ͨ��)
#define ECONFGROUPCONFNUM	8  //ÿ�������������Ŀ(ÿ��DSP֧�ֵĻ���)
#define ECONFGROUPNUM		32 //�����ܻ�������(4��DSP*8�鿨)=1024ͨ��

#define CARDHARDWARE_DT256           2  //30--240�����ֿ�
#define CARDHARDWARE_SLAT24          3  //4--24��ģ�⿨

/////////////////����ͨ������
#define CHCLASS_NONE    0x00  //����·����
#define CHCLASS_DT      0x08  //������
#define CHCLASS_AT      0x10  //ģ����

#define CHTYPE_DTNO1    0x09  //�����м��й�һ������ 
#define CHTYPE_DTNO7    0x0A  //�����м��й��ߺ�����(TUP)
#define CHTYPE_DTDSS1   0x0B  //�����м�ISDN PRI(30B+D)
#define CHTYPE_DTNO7ISUP 0x0C  //�����м��й��ߺ�����(ISUP)
#define CHTYPE_MTP3		0x0F  //�ߺ�����MTP�����ͨ��

#define CHTYPE_TB       0x11  //ģ���м�����(��ͨ�л���),�ӵ��ž�
#define CHTYPE_AGENT    0x12  //ģ���м�����(��ϯ��)���ӵ绰��
#define CHTYPE_HB       0x13  //ģ���м̸�����(�����)����绰��/�绰���ಢ��

/////////////////������Դ����
#define RESTYPE_VOCPCM  0x01   //PCM������Դ
#define RESTYPE_DTMF    0x02   //DTMF��Դ
#define RESTYPE_FAX     0x03   //������Դ
#define RESTYPE_FSK     0x04   //fskͨѶ��Դ
#define RESTYPE_MODEM   0x05   //modemͨѶ��Դ
#define RESTYPE_HDLC    0x06   //HDLC��Դ(�ڲ�ʹ��)
#define RESTYPE_CAS     0x07   //cas��Դ

#define RESTYPE_VOIP    0x1D   //VOIPͨѶ��Դ
#define RESTYPE_BIG     0x1E   //����ͨѶ��Դ
#define RESTYPE_MFC     0x1F   //mfc��Դ(�ڲ�ʹ��)

#define RESTYPE_VC      0x18   //����(voice change)��Դ
#define RESTYPE_CONF    0x19   //�绰��������Դ 
#define RESTYPE_CONFCH  0x1A   //�绰����ͨ����Դ 
#define RESTYPE_ECONF   0x1B   //��ǿ�绰��������Դ 
#define RESTYPE_ECONFCH 0x1C   //��ǿ�绰����ͨ����Դ 

/////////////////����ͨ��״̬
#define CHSTAT_IDLE     0x00  //����

#define CHSTAT_CALLOUT  0x10  //����״̬
#define CHSTAT_CALLOSET 0x11  //����״̬setup
#define CHSTAT_CALLOING 0x12  //����״̬���ں���
#define CHSTAT_CALLORIN 0x1E  //��������
#define CHSTAT_CALLOCON 0x1F  //����ͨ��

#define CHSTAT_CALLIN   0x20  //����״̬
#define CHSTAT_CALLINTF 0x21  //��������
#define CHSTAT_CALLIPRO 0x22  //����״̬����
#define CHSTAT_CALLIRIN 0x2E  //��������
#define CHSTAT_CALLICON 0x2F  //����ͨ��

/////////////////������Դ״̬
#define RESTAT_IDLE     0x00  //����
#define RESTAT_OPENNING 0x01  //���ڴ�
#define RESTAT_INIT     0x02  //���ڳ�ʼ��
#define RESTAT_CLOSING  0x03  //���ڹر�
#define RESTAT_STOP     0x04  //������ֹͣ
#define RESTAT_CLEAR    0x05  //ֹͣ����������Դ
#define RESTAT_WORKING  0x0F  //����ʹ��
/////////////////������Դ����
#define RESPAR_NONE     0x00  //
#define RESPAR_DTMFSTOP 0x01  //�յ�DTMFʱֹͣ¼�����
#define RESPAR_PLOOPMSG  0x20  //����һ��ѭ��������Ϣ
			       //���û��ͬʱ����RESPAR_PLASTMSG,
                               //����������׷�ӵ�����β
#define RESPAR_PFIRSTMSG 0x40  //���ǵ�һ��������Ϣ
#define RESPAR_PLASTMSG  0x80  //�������һ��������Ϣ

#define RESPAR_RAGC     0x02  //¼��ʹ���Զ��������
#define RESPAR_RWAITP   0x40  //�ȴ�����������ſ�ʼ¼��
#define RESPAR_RAPPEND  0x80  //¼��ʱ׷�ӵ�ԭ�ļ�ĩβ
                               
////////�����ļ��������ļ���ʽ
#define PFDATA_ALAW     0x00  //��׼alaw pcm �޸�ʽ�ļ�
#define PFDATA_WAVE     0x01  //windows .WAV�ļ�
#define PFDATA_CLADPCM  0x02  //cirrus logic adpcm(32kbps) �޸�ʽ�ļ�
#define PFDATA_G7231    0x03  //G.723.1(6.3Kbps) �޸�ʽ�ļ�
#define PFDATA_DLGVOX6K 0x04  //dialogic vox 6K sample
#define PFDATA_DLGVOX8K 0x05  //dialogic vox 8K sample
#define PF_OFFSETPFILE	0x80  //�Ƿ�ָ�������ļ�����ʼλ��(����)

//////////                                                             
////////���崫���ļ��ı����ʽ
#define FAXFILE_NOFORMAT 0x00  //�޸�ʽ�ļ�����ͨ�ֱ��ʣ�һά����
#define FAXFILE_TIFF     0x01  //TIFF�����ļ�
////////���崫�����ݵĿ��
#define FAXVRES_LOW      0x00  //��ͨ��ֱ�ֱ���(98/Ӣ��)
#define FAXVRES_HIGH     0x20  //�ߴ�ֱ�ֱ���(196/Ӣ��)

#define FAXWIDTH_A4      0x00  //1728���
#define FAXWIDTH_A3      0x40  //2432���
#define FAXWIDTH_B4      0x80  //2048���
                
/////////����MODEM���ַ���
#define MODEM_ANS		 0x00  //Ӧ��
#define MODEM_CALL		 0x01  //����

/////////���峣��
#define MSGFROMECONF     0x01  //��Ϣ��Դ����ǿ�ͻ���

//�ж��Ƿ�Ϊ������
inline unsigned short IsCLASS_DT(unsigned short chtype)
{  
	switch (chtype)
	{
		case CHTYPE_DTNO1://    0x09  //�����м��й�һ������ 
	  case CHTYPE_DTNO7://    0x0A  //�����м��й��ߺ�����(TUP)
		case CHTYPE_DTDSS1://   0x0B  //�����м�ISDN PRI(30B+D)
	  case CHTYPE_DTNO7ISUP:// 0x0C  //�����м��й��ߺ�����(ISUP);
	    return 1;
	  default:
	  	return 0;
	}
}

//�ж��Ƿ�Ϊģ����
inline unsigned short IsCLASS_AT(unsigned short chtype)
{  
	switch (chtype)
	{
		case CHTYPE_TB://       0x11  //ģ���м�����(��ͨ�л���),�ӵ��ž�
		case CHTYPE_AGENT://    0x12  //ģ���м�����(��ϯ��)���ӵ绰��
		case CHTYPE_HB:   //    0x13  //ģ���м̸�����(�����)����绰��/�绰���ಢ��
	    return 1;
	  default:
	  	return 0;
	}
}

/////////////////������Ϣ,�������´�Ҳ�����ϴ�
#define MSG_NULL              0     //����Ϣ,��Ϣ������,DataLen�Ծ�������
                                    
#define MSG_RELEASE           1     //ͨ���ͷ�
 #define F_R_NORMAL            0    //�����ͷ�
 #define F_R_CALLOUTERR        1    //����ʧ���ͷ�
  #define P_R_NODIALTONE        1   //û�м�⵽������ 
  #define P_R_NOANSWER          2   //û����Ӧ
  #define P_R_RINGTIMEOUT       3   //���峬ʱ �����˽ӣ�
  #define P_R_LINEUSED          4  //��·�����Է�ռ�û�û�и�λ���
  #define P_R_CONGESTION        12  //�豸ӵ��
  #define P_R_INCOMPLETE        15  //��ַ��ȫ
  #define P_R_FAILURE           16  //���й���
  #define P_R_CALLBARRING       18  //����ܾ�
  #define P_R_BUSYLOCAL         19  //�л�æ
  #define P_R_BUSYDDD           20  //����æ
  #define P_R_NULLDN            22  //�պ�
  #define P_R_NODIGITALPATH     26  //û������ͨ·
 #define F_R_RESET	           2    //��·��ԭ


#define MSG_CALL              2     //ͨ������/���к���
 #define F_C_NORMAL            0    //��ͨ����
  //����TB �� AGENT ����ʱ
  #define P_C_NORMAL            0    //TB=���dialtone,AGENT=��׼������+FSK���з���                     
  #define P_C_1                 1    //TB=�����dialtone;AGENT=��׼������+DTMF���з���
  #define P_C_2                 2    //AGENT=��2��������+DTMF���з���
  #define P_C_3                 3    //AGENT=��3��������+DTMF���з���
  #define P_C_4                 4    //AGENT=��4��������+DTMF���з��� 
  //����TB �� AGENT ����ʱ,Param=0
  //����NO1����,Param��4λΪKAֵ(0����Ĭ��ֵKA=1),Param��4λΪKDֵ(0����Ĭ��ֵKD=1)
  //����NO7����,Param 	 //bit5:bit4 ��ʾԭʼ�������(00=��ͨ��01=���ڱ��ã�10=������Ч��11=����)
  			 //bit3:bit2 ��ʾ�������(00=��ͨ��01=���ڱ��ã�10=������Ч��11=����)
					//bit1��ʾ�Ƿ����к�����ʾ����(0=������,1=����),
				    //bit0��ʾ�Ƿ��ǲ���ȫ����(0=��ȫ���У�1=����ȫ����)
  //����DSS1����,Param��ʾ�Ƿ��ǲ���ȫ����(0=��ȫ���У�1=����ȫ����)
 #define F_C_OVLP              1    //�ص�����
  
#define MSG_CALLEDACK         3     //����ժ��Ӧ��
 #define  F_ACK_ANC            0    //�Ʒ�
 #define  F_ACK_ANN            1    //���Ʒ�

#define MSG_FLASH             4     //�Ĳ��

#define MSG_ACM               5    //�ϴ���Ϣ���������ɹ�(�Է��Ѿ�����),�п����ղ�������Ϣ��ֱ���յ�Connect��Ϣ
					               //�´���Ϣ���Է�(���з�)����ַȫ(�����������)
 #define F_ACM_IDLE            0   //���п���
 #define F_ACM_BUSY            1   //��·æ
 #define F_ACM_NULL            2   //��·�պ�
 #define F_ACM_FAULT           3   //��·����ʧ��


#define UPMSG_CALLOUTCHANNEL  6    //�ϴ���Ϣ���䵽�ĺ���ͨ��
 #define F_CC_NORMAL           0    //���䵽����ͨ��(=ChIndex)
 #define F_CC_NOCHANNEL        1    //���䲻������ͨ��(ChIndex)
  #define P_CC_CHBUSY           1   //ͨ��ʹ����
  #define P_CC_CHCALLMASK       2   //��������ͨ����������ֹ
  #define P_CC_CHNOTEXIST       3   //ͨ���Ų�����
  #define P_CC_PCMNOSYN         4   //�����߶˿�û��ͬ��
  #define P_CC_CANNOTASSIGNCH   5   //��֧���û����õĺ���ͨ��ָ����ʽ
  #define P_CC_REMOTENOANSWER   6   //�Է�û����Ӧ
  #define P_CC_DPCUNAVAILABLE   7   //DPC���ɵ���
  #define P_CC_ADDRESSERR		8   //�������ô���
 #define F_CC_HANDLE           2    // ���䵽�������(ChIndex),�����ص�����     
                              
#define DOWNMSG_ROUTER        6    //ͨ����������
 #define F_R_SILENCE           0   //ͨ���������
 #define F_R_MONITOR           1   //�������
 #define F_R_TALK              2   //˫��ͨ��
 #define F_R_TALK_RP		   3   //˫��ͨ����ͨ���пɻ��¼������	
 #define F_R_MONITOR_P		   4   //ǿ�����ӵ����ڷ�����Դ		   
 #define F_R_TRANSTO_CT        13  //ֱ�ӷ��͵�CT����һ��ʱ϶ 
 #define F_R_MONITOR_CT        14  //ֱ�Ӽ���CT����һ��ʱ϶ 

#define MSG_CONF              7    //����
 #define F_CF_CREATE           1   //����绰������Դ������һ��������        
 #define F_CF_CLOSE            2   //�رյ绰������Դ���ر�һ��������        
 #define F_CF_LINKCH           3   //���ӵ绰����ͨ������·ͨ��,�����߼�ͨ����������
 #define F_CF_UNLINKCH         4   //�Ͽ��绰����ͨ������·ͨ��������,�Ͽ��߼�ͨ���ͻ����ҵ�����
 #define F_CF_MONITOR          5   //������������,�߼�ͨ��������������������(��ǿ����)
 #define F_CF_PARAM			   6   //���û������(��ǿ����)

								   //������������ֻռ��һ·����ͨ����Դ(������������)  
 #define D_ECF_USEAGC          0x80//������ʹ���Զ��������
 #define D_ECF_CHNUMASK        0x7F//�����Ҵ�С����	
 
 #define P_CF_OK               0  //ָ���Ĺ�����ȷ���
 #define P_CF_FUNCTIONERR      1  //ָ����FUNCTION����
 #define P_CF_CONFNOTEXIST     2  //�����ҺŷǷ�
 #define P_CF_LOGICHNOTEXIST   3  //�������߼�ͨ���Ƿ�
 #define P_CF_CONFNOTCREATE    4  //�ر�/����/����/���û������ʱָ��������û�б�����
 #define P_CF_NOCONFCHRES      5  //����/����/����ʱ����ͨ����Դ����
 #define P_CF_CR_CONFSIZE	   6  //����ʱ�����Ҵ�С����	
 #define P_CF_CR_NOCONFRES     7  //����ʱ��������Դ�����ָ���������Ѿ������� 
 #define P_CF_LK_LOGICHINCONF  8  //����/����ʱָ�����߼�ͨ���Ѿ�������Ļ���������
 #define P_CF_UL_LOGICHNOTCONF 9  //�Ͽ�����ʱָ���߼�ͨ����û����ĳ��������
 #define P_CF_ST_NOTSAMEGROUP  10 //���û������ʱָ���ı���������������߻��鲻��ͬһ������

#define MSG_ONOFF             8     //ͨ���ر�/��(����ʹ��/����ʹ��)
 #define F_OF_OFF              0   //�ر�
 #define F_OF_ON               1   //��
 #define P_OF_CALLIN           1   //�������
 #define P_OF_CALLOUT          2   //�������
 
#define DOWNMSG_SETPARAM      9    //�´���Ϣ���ò��� 
 #define F_PM_DEVICE             0  //ÿ���豸����
 #define F_PM_GLOBAL	         1  //ȫ�ֲ���
 
#define UPMSG_PARAMERROR      9    //�ϴ���Ϣ���ò���(���д���)
 #define F_PM_SLOT				15  //����ʱ϶λ��
 

#define UPMSG_ALARM           10    //�ϴ���Ϣ��·�澯(�����)
 #define F_AM_CH				0   //ͨ���澯
  #define P_AM_NO1_CHREOK        0   //��·���������У�
  #define P_AM_NO1_CHREMOTEBLOCK 1   //Զ�˱���
  #define P_AM_NO1_CHCONFUSE     2   //100:������·�յ��Է�ռ��
  #define P_AM_NO1_CHNOCONFIRM   3   //101:����ʱ�Է�ռ��֤ʵʱ��̫��(>200ms),ʵ��Ϊ>800ms
  #define P_AM_NO1_CHERRORREL1   4   //102:����ʱ�Է�ռ��֤ʵ������ժ��ǰ���������ͷ�����·(>2s)
  #define P_AM_NO1_CHERRORREL2   5   //103:����ʱ�Է�ժ�����������ͷ�����·
  #define P_AM_TB_OK  		     0    //������ֱ����ѹ�ָ�����
  #define P_AM_TB_NOPOWER		 6    //������û��ֱ����ѹ(���߻��·)
 #define F_AM_PCMSYN			1   //PCMͬ���澯
  #define P_AM_SYNOK             0    //ͬ������
  #define P_AM_SYNNOCARRY		 1    //�������ز�
  #define P_AM_SYNFAS		     2    //����FASͬ������
  #define P_AM_SYNCAS			 3    //����CAS MFͬ������ֻ��CAS��ʽ�£�
  #define P_AM_SYNSLIP			 4    //���Ի���������
  #define P_AM_SYNALARMA		 5    //Զ��Aλ�澯
  #define P_AM_SYNALARMY		 6    //Զ��CAS MF Yλ�澯��ֻ��CAS��ʽ�£�
  #define P_AM_SYNALLONE		 7    //����ȫ1����
  #define P_AM_SYNJITTER		 8    //����������·�澯
 #define F_AM_MTP				2    //�ߺ�����MTP�澯
  #define P_L3L4_STOP			 1    //������·�ж�
  #define P_L3L4_LINKOK			 2    //��·��ͨ����ͨ��·��	  
  #define P_L3L4_LINKFAIL        3    //��·�жϣ�ʧ����·��	  
  #define P_L3L4_DPCOK           4    //DPC �ɵ���	  
  #define P_L3L4_DPCNOLINK       5    //DPC ���ɵ���
	  
#define UPMSG_DAMAGE          11    //�ϴ���ϢӲ�������(Ӳ����)

/////////////////�Ȳ����Ϣ
#define MSG_PnP               12    //�����γ���Ϣ
 #define F_PnP_PLUG            1    //�ϴ���Ϣ�����
 #define F_PnP_QUERYPULL       2    //�ϴ���Ϣϣ���γ�
 #define F_PnP_PULL            3    //�ϴ���Ϣ��γ�
 #define F_PnP_USE             4    //�´���Ϣʹ�ð�
 #define F_PnP_UNUSE           5    //�´���Ϣֹͣʹ�ð�(����γ�)

#define MSG_USERDATA		  13    //�û�ֱ���շ�������ص���Ϣ��
 #define F_UD_ITU			    0   //ITU��ʽ
 #define F_UD_USER			    1	//�û��Զ����ʽ

#define UPMSG_SYNREF          14    //ͬ���ο�
#define DOWNMSG_ADDINDEX      14    //���һ���ڴ�����(�ļ���ʽ)

/////////////////��ý����Ϣ
#define MSG_MEDIA             15    //��ý����Ϣ(VOICE,DTMF,FAX,FSK,MODEM,CAS,HDLC)
 #define F_MEDIA_None          0    //��
 #define F_MEDIA_Tx_Release    1    //�ͷŷ�����Դ 
 #define F_MEDIA_Rx_Release    2    //�ͷŽ�����Դ 
 #define F_MEDIA_Tx_File       4    //�����ļ�
 #define F_MEDIA_Tx_Memory     5    //�����ڴ� 
 #define F_MEDIA_Tx_Index      6    //�����ڴ����� 
 #define F_MEDIA_Tx_TTStr      7    //�ı���
 #define F_MEDIA_Tx_TTSFile    8    //�ı��ļ�
 #define F_MEDIA_Tx_DTMF       9    //���� dtmf
 #define F_MEDIA_Tx_CPSign     10   //�����ź���

 #define F_MEDIA_Rx_File       12    //���յ��ļ�
 #define F_MEDIA_Rx_Memory     13    //������Դ�����ڴ淽ʽ���� 
 #define F_MEDIA_Rx_Pause      14    //������ͣ
 #define F_MEDIA_Rx_Resume     15    //�������¿�ʼ

 #define F_MEDIA_SetParam      11    //����ָ��ͨ����ý�����(�·�)

 #define F_MEDIA_Error         11    //��Դ����������(�ϴ�)
  #define P_MEDIA_AllocOk        0   //����ɹ�
  #define P_MEDIA_AllocErr       1   //�������
  #define P_MEDIA_BusyErr        2   //ʹ����
  #define P_MEDIA_FullErr        3   //��Դ�����޷�����������(��ǰ�������)
  #define P_MEDIA_TypeErr        4   //�������Դ���ͺ�(����ͨ��������ǰһ����Դ��û���ͷ�)
  #define P_MEDIA_FuncErr        5   //��֧�ֵ���Դ������ʽ
  #define P_MEDIA_FileErr        6   //�ļ�����



////////////////����ֹͣԭ�� when
#define RESTOP_DTMF     0x01  //�յ�DTMFֹͣ
#define RESTOP_USER     0x02  //�û�ֹͣ
#define RESTOP_FINISH   0x03  //��Ȼ����ֹͣ
#define RESTOP_COMMERR  0x04  //���ֻ�ͨѶʧ��ֹͣ
#define RESTOP_CHREL    0x05  //��Ӧ����·ͨ���ͷ�
#define RESTOP_FILERR   0x06  //�ļ�д����



#ifndef _min_
  #define _min_(a,b) (((unsigned long)a < (unsigned long)b) ? a : b)
#endif

#define MAXCALLLEN     32  //������б��к��볤��
/////////////////������Ϣ�ṹ
struct RMSG  //���ڴ�����Ϣ,�ֶ̽ṹ(8�ֽڳ�)�ͳ��ṹ(264�ֽڳ�).
{   
  unsigned short MsgType     : 4;  //��Ϣ����      
  unsigned short Function    : 4;  //����
  unsigned short Param       : 8;  //����
  
  								   
  unsigned short ChIndex     :11;  //ͨ���߼���(0--2047) 
  unsigned short ChType      : 5;  //ͨ������(��CHTYPE����)	       
  
  unsigned short LinkChIndex :11; //��ص���һ��ͨ���߼���(0--2047)      
  unsigned short LinkChType  : 5; //��ص���һ��ͨ������·����      
  
  unsigned short DataLen     : 8;  //0��ʾû�к���Ĵ�union����,�����ʾunion��ʵ����Ч����
				                   //1--255��ʾ�к���Ĵ�union����			  
  unsigned short DataSpec    : 8; //��������
// DataSpec��5bit;     //���г���(0--31)	  
// ���г���(0--31)= DataLen>MAXCALLLEN?DataLen-MAXCALLLEN:0;	  


  union  //ֻ�е� DataLen >0  ʱ��Ч
  {
    struct
	{
      char  Called[MAXCALLLEN];	//���к���(0--31)	    	  
      char  Caller[MAXCALLLEN];    //���к���(0--31)(���ܰ����������ڸ������к�ԭʼ����)	    	  
	};	
	unsigned char  DataB[256];  
	unsigned short DataW[128];	
	unsigned long  DataDW[64];	
  };

public:
  RMSG() //���캯��
  {Clear();};
  
  void Clear() 
  { ((long *)this)[0]=((long *)this)[1]=0;  };

  void InitDataLen(unsigned short datalen=0)
  {	DataLen=datalen; }

  void InitCallNull()
  {DataLen=DataSpec=0;};

  void SetToNullMsg(unsigned short datalen) //����Ϊ����Ϣ
  {Clear();InitDataLen(datalen);};

  unsigned short AddByte(unsigned char data)
  {if (DataLen==255) return 0; DataB[DataLen++]=data;
       return 1;};

  unsigned short AddWord(unsigned short data)
  {if (DataLen>=254) return 0; DataB[DataLen++]=data&0xFF;
       DataB[DataLen++]=data>>8;return 2;};

  unsigned short AddStr(char *data)
  {
     unsigned short len=strlen(data);
	 if(len+DataLen>=255) 
		 return 0; //�������
	 RtlCopyMemory(&DataB[DataLen],data,len+1);//��0	 
	 DataLen+=len+1;	 
	 return len+1;
  }
///////used for call msg
  
  unsigned short GetCalledLen()
  {return DataSpec % MAXCALLLEN;};

  unsigned short GetCallerLen()
  {return DataLen > MAXCALLLEN ? DataLen-MAXCALLLEN:0;};

  void SetCalledLen(unsigned short len)
  {
	 DataSpec = len % MAXCALLLEN;
	
	 if(0==GetCallerLen())//ԭ�������к���,�����ܳ���
	    DataLen=DataSpec ;	
  };

  void SetCallerLen(unsigned short len)
  {
    if(len>0)
      DataLen=MAXCALLLEN+(len % MAXCALLLEN);
    else
      DataLen=DataSpec;
  }

  void ClearCalled()
  {SetCalledLen(0);};

  void ClearCaller()
  {SetCallerLen(0);};

  void AppendCaller(char code)
  {
	  unsigned short len=GetCallerLen();
	  if(len<MAXCALLLEN-1) 
	  {
	    Caller[len++]=code; 
        SetCallerLen(len);
		Caller[len]=0; 
	  }
  }

  void AppendCalled(char code)
  {
	  unsigned short len=GetCalledLen();
	  if(len<MAXCALLLEN-1)
	  {
		  Called[len++]=code; 
		  SetCalledLen(len);
		  Called[len]=0; 
	  }
  }

  void AppendCalledTerm() //��ӽ�β0
  {
	unsigned short len=GetCalledLen();
	if(len) //������Ϊ0ʱ������ʹ��Called
	  if(len<MAXCALLLEN)
  	    Called[len]=0; 
  }

  void AppendCallerTerm() //��ӽ�β0
  {
    unsigned short len=GetCallerLen();
	if(len) //������Ϊ0ʱ������ʹ��Caller
	  if(len<MAXCALLLEN)
  	    Caller[len]=0; 
  }
  
  void AppendCaller(const char *phone,unsigned short len) 
  {     
	 if(len>0)
	 {
       unsigned short l=GetCallerLen();
       if(l+len>(MAXCALLLEN-1))
		 len=MAXCALLLEN-1-l;
       RtlCopyMemory(&Caller[l], phone,len);	 
	   SetCallerLen(len+l);
	   //AppendCallerTerm();
	 }
  }

  void AppendCaller(const char *phone) 
  {
    unsigned short len=strlen(phone);
	AppendCaller(phone,len); 
  }
  
  
  void AppendCalled(const char *phone,unsigned short len) 
  {
	 if(len>0)
	 {
       unsigned short l=GetCalledLen();
       if(l+len>(MAXCALLLEN-1))
		 len=MAXCALLLEN-1-l;
       RtlCopyMemory(&Called[l], phone,len);	 
	   SetCalledLen(len+l);
	   //AppendCalledTerm();
	 }
  }

  void AppendCalled(const char *phone) 
  {
    unsigned short len=strlen(phone);
	AppendCalled(phone,len); 
  }

  const char *GetCalled()
  {	  return GetCalledLen()?Called:"";  }//ע�⵱����Ϊ0ʱ������ʹ��called
  
  const char *GetCaller()
  {	  return GetCallerLen()?Caller:"";  }//ע�⵱����Ϊ0ʱ������ʹ��caller
///////used for call msg

  
  unsigned short IsNullMsg(){return (MsgType==MSG_NULL);};

  unsigned short DsOfMsg()//��Ϣ�Ĵ�С(˫�ֱ�ʾ)
  {return 2+((unsigned short)DataLen+3)/4;};

  const RMSG &operator =(const RMSG &m) //����ṹ��ֵ����
  {
    unsigned int i;
    ((long *)this)[0]=((long *)&m)[0];     //copy first 4 bytes
	((long *)this)[1]=((long *)&m)[1]; //copy second 4 bytes
		
	if(DataLen) //m is long struct,so copy union data
	{
      for(i=0;i<((unsigned int)DataLen+3)/4;i++)
	    ((long *)this)[2+i]=((long *)&m)[2+i];
	}
    return *this;
  };
};

void inline CV8bitTo4bit(unsigned char *b8,unsigned char *b4,unsigned short num)//ת��8bit����4bit��
{//ע��b4 ��b8�����ص�
  for(unsigned short i=0;i<num;i++)
  {
    if(i&1)
      b4[i/2]|=b8[i]<<4;  
    else
	  b4[i/2]=b8[i] & 0x0F;     
  }
}

void inline CV4bitTo8bit(unsigned char *b4,unsigned char *b8,unsigned short num)//ת��8bit����4bit��
{//ע��b4 ��b8�����ص�
  for(short i=num-1;i>=0;i--)
  {    
    if(i&1)
      b8[i]=b4[i>>1]>>4;  
    else
	  b8[i]=b4[i>>1] & 0x0F;     
  }
}

#define ALARMSYNOK        0    //ͬ������
#define ALARMSYNNOCARRY   1    //�������ز�
#define ALARMSYNFAS       2    //����FASͬ������
#define ALARMSYNCAS       3    //����CAS MFͬ������ֻ��CAS��ʽ�£�
#define ALARMSYNSLIP      4    //���Ի���������
#define ALARMSYNALARMA    5    //Զ��Aλ�澯
#define ALARMSYNALARMY    6    //Զ��CAS MF Yλ�澯��ֻ��CAS��ʽ�£�
#define ALARMSYNALLONE    7    //����ȫ1����
#define ALARMSYNJITTER    8    //����������·�澯



inline const char *GetChTypeNameStr(unsigned short chtype)//ȡͨ�����͵�����
{
 switch (chtype)
 {
  case CHTYPE_DTNO1:	return "һ��ͨ��";
  case CHTYPE_DTNO7:	return "�ߺ�ͨ��";
  case CHTYPE_DTDSS1:	return "ISDNͨ��";
  case CHTYPE_DTNO7ISUP:return "ISUPͨ��";
  case CHTYPE_MTP3:		return "MTP3�����ͨ��";
  case CHTYPE_TB:		return "����ͨ��";
  case CHTYPE_AGENT:	return "����ͨ��";
  case CHTYPE_HB:		return "����ͨ��";
  
  case RESTYPE_VOCPCM:  return "������Դ";
  case RESTYPE_DTMF  :  return "DTMF��Դ";
  case RESTYPE_FAX   :  return "������Դ";
  case RESTYPE_FSK   :  return "fsk��Դ";
  case RESTYPE_MODEM :  return "modem��Դ";
  case RESTYPE_HDLC  :  return "HDLC��Դ";
  case RESTYPE_CAS   :  return "cas��Դ";
  case RESTYPE_VOIP  :  return "VOIP��Դ";
  case RESTYPE_BIG   :  return "����ͨѶ��Դ";
  case RESTYPE_MFC   :  return "mfc��Դ";
  case RESTYPE_VC    :  return "������Դ";
  case RESTYPE_CONF  :  return "��������Դ"; 
  case RESTYPE_CONFCH:  return "����ͨ����Դ"; 
  case RESTYPE_ECONF  : return "��ǿ��������Դ"; 
  case RESTYPE_ECONFCH: return "��ǿ����ͨ����Դ"; 
  default:				return "δ֪����";
 }
}
 
inline const char *GetResErrReason(unsigned char rsn)//����ý�������
{
 switch(rsn)
  {
  case P_MEDIA_AllocOk: return "����ý����Դ�ɹ�";
  case P_MEDIA_AllocErr: return "����ý����Դʧ��";
  case P_MEDIA_BusyErr: return "��Դռ����";
  case P_MEDIA_FullErr: return "��Դ��������";
  case P_MEDIA_TypeErr: return "ý����Դ���ʹ���";
  case P_MEDIA_FuncErr: return "��֧�ֵ���Դ������ʽ";
  case P_MEDIA_FileErr: return "�ļ���������";
  default:return "δ֪����";
  }
}


inline const char *GetMsgTypeNameStr(unsigned short msgtype, int rt)
{
	if(rt == 0)		//������Ϣ
	{
		switch(msgtype)
		{
		case(MSG_RELEASE):		return "�ͷ�ͨ��";
		case(MSG_CALL):			return "ͨ������";
		case(MSG_ACM):			return "��ַ��ȫ";
		case(MSG_CALLEDACK):	return "�ҷ�Ӧ��";
		case(MSG_FLASH):		return "�Ĳ��";
		case(DOWNMSG_ROUTER):	return "ͨ������";
		case(MSG_CONF):			return "�绰����";
		case(MSG_ONOFF):		return "ͨ������";
		case(DOWNMSG_SETPARAM):	return "���ò���";
		case(MSG_USERDATA):		return "������Ϣ��";
		case(DOWNMSG_ADDINDEX): return "�����������";
		case(MSG_MEDIA):		return "��ý��";
		default:				return "δ֪��Ϣ"; 
		}
	}
	else	//������Ϣ
	{
		switch(msgtype)
		{
		case(MSG_RELEASE):			return "ͨ���ͷ�";
		case(MSG_CALL):				return "ͨ������";
		case(MSG_ACM):				return "��ַ��ȫ";
		case(MSG_CALLEDACK):		return "�Է�Ӧ��";
		case(MSG_FLASH):			return "�Ĳ��";
		case(UPMSG_CALLOUTCHANNEL):	return "�������ͨ��";
		case(MSG_CONF):				return "�绰������";
		case(MSG_ONOFF):			return "ͨ������";
		case(UPMSG_PARAMERROR):		return "�������ô���";
		case(UPMSG_ALARM):			return "��·�澯";
		case(UPMSG_DAMAGE):			return "Ӳ������";
		case(MSG_USERDATA):		    return "������Ϣ��";
		case(MSG_MEDIA):			return "��ý��";
		default:					return "δ֪��Ϣ"; 
		}
	}
}

inline const char *GetReleaseFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_R_NORMAL):		return "��ͨ�ͷ�";
	case(F_R_CALLOUTERR):	return "����ʧ���ͷ�";
	default:				return "δ֪";
	}
}

inline const char *GetReleaseParamNameStr(unsigned short param)
{
	switch(param)
	{
	case(P_R_NODIALTONE):	return "�޲�����";
	case(P_R_NOANSWER):		return "û����Ӧ";
	case(P_R_RINGTIMEOUT):	return "���峬ʱ";
	case(P_R_LINEUSED):		return "��·ռ��";
	case(P_R_CONGESTION):	return "�豸ӵ��";
	case(P_R_INCOMPLETE):	return "��ַ��ȫ";
	case(P_R_FAILURE):		return "���й���";
	case(P_R_CALLBARRING):	return "����ܾ�";
	case(P_R_BUSYLOCAL):	return "�л�æ";
	case(P_R_BUSYDDD):		return "����æ";
	case(P_R_NULLDN):		return "�պ�";
	case(P_R_NODIGITALPATH):return "������ͨ·";
	default:				return "δ֪";
	}
}

inline const char *GetCallFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_C_NORMAL):		return "��ͨ����";
	case(F_C_OVLP):			return "�ص�����";
	default:				return "δ֪����";
	}
}

inline const char *GetCallParamNameStr(unsigned short chtype, unsigned short param)
{
	switch(param)
	{
	case(P_C_NORMAL):
		switch(chtype)
		{
			case(CHTYPE_AGENT):		return "��׼������+FSK���з���";
			case(CHTYPE_TB):		return "��Ⲧ����";
			case(CHTYPE_DTNO1):
			case(CHTYPE_DTNO7):
			case(CHTYPE_DTNO7ISUP):
			case(CHTYPE_DTDSS1):	return "��׼����";
		}
		break;
	case(P_C_1):
		switch(chtype)
		{
			case(CHTYPE_AGENT):		return "DTMF���з���+��׼������";
			case(CHTYPE_TB):		return "����Ⲧ����";
			case(CHTYPE_DTNO1):		return "KA(1),����Ϊ��ͨ,����";
			case(CHTYPE_DTNO7):
			case(CHTYPE_DTNO7ISUP):
			case(CHTYPE_DTDSS1):	return "����ȫ����";
		}
		break;
	case(P_C_2):
		switch(chtype)
		{
			case(CHTYPE_AGENT):		return "DTMF���з���+��2��������";
			case(CHTYPE_DTNO1):		return "KA(2),����Ϊ��ͨ,����";
			case(CHTYPE_DTNO7):		
			case(CHTYPE_DTNO7ISUP):	return "���к�����ʾ����";
		}
		break;
	case(P_C_3):
		switch(chtype)
		{
		case(CHTYPE_AGENT):			return "DTMF���з���+��3��������";
		case(CHTYPE_DTNO1):			return "KA(3),����Ϊ��ͨ,Ӫҵ��";
		case(CHTYPE_DTNO7):
		case(CHTYPE_DTNO7ISUP):		return "����ȫ����+���к�����ʾ����";
		}
		break;
	case(P_C_4):
		switch(chtype)
		{
		case(CHTYPE_AGENT):			return "DTMF���з���+��4��������";
		case(CHTYPE_DTNO1):			return "KA(2),����";
		}
		break;
/*				
	case(P_C_5):
		switch(chtype)
		{
		case(CHTYPE_DTDSS1):		return "KA(5),����Ϊ��ͨ,���";	
		}
		break;
*/				
	//default:					
	}
	return "δ֪����";

}

inline const char *GetAcmFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_ACM_IDLE):		return "���п���";
	case(F_ACM_BUSY):		return "��·æ";
	case(F_ACM_NULL):		return "��·�պ�";
	case(F_ACM_FAULT):		return "����ʧ��";
	default:				return "δ֪����";
	}
}

inline const char *GetCallOutChannelFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_CC_HANDLE):		return "���䵽���";
	case(F_CC_NORMAL):		return "���䵽ͨ��";
	case(F_CC_NOCHANNEL):	return "���䲻��ͨ��";
	default:				return "δ֪����";
	}
}

inline const char *GetCallOutChannelParamNameStr(unsigned short param)
{
	switch(param)
	{
	case(P_CC_CHBUSY):			return "ͨ��ʹ����";
	case(P_CC_CHCALLMASK):		return "������ͨ��������ֹ";
	case(P_CC_CHNOTEXIST):		return "ͨ���Ų�����";
	case(P_CC_PCMNOSYN):		return "�����߶˿�û��ͬ��";
	case(P_CC_CANNOTASSIGNCH):	return "��֧���û����õĺ���ͨ��ָ����ʽ";
	case(P_CC_REMOTENOANSWER):	return "�Է�û����Ӧ";
	case(P_CC_DPCUNAVAILABLE):	return "DPC���ɵ���";
	case(P_CC_ADDRESSERR):		return "�������ô���";
	default:					return "δ֪����";
	}
}

inline const char *GetConfFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_CF_CREATE):		return "��������";
	case(F_CF_CLOSE):		return "�رջ���";
	case(F_CF_LINKCH):		return "�μӻ���";
	case(F_CF_UNLINKCH):	return "�˳�����";
	case(F_CF_MONITOR):		return "��������";
	case(F_CF_PARAM):		return "�������";
	default:				return "δ֪����";
	}
}

inline const char *GetAlarmFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_AM_CH):		return "ͨ���澯";
	case(F_AM_PCMSYN):	return "PCMͬ���澯";
	case(F_AM_MTP):     return "�ߺ�����MTP�澯";
	default:			return "δ֪����";
	}
}

inline const char *GetAlarmParamNameStr(unsigned short func, unsigned short param)
{
	switch(func)
	{
	case(F_AM_CH):
		switch(param)
		{
		case(P_AM_NO1_CHREOK):			return "��·����";
		case(P_AM_NO1_CHREMOTEBLOCK):	return "Զ�˱���";
		case(P_AM_NO1_CHCONFUSE):		return "�յ��Է�ռ��";
		case(P_AM_NO1_CHNOCONFIRM):		return "ռ��֤ʵʱ��̫��";
		case(P_AM_NO1_CHERRORREL1):		return "ռ��֤ʵ������ժ��ǰ�쳣�ͷ�";
		case(P_AM_NO1_CHERRORREL2):		return "�Է�ժ�����쳣�ͷ�";
		case(P_AM_TB_NOPOWER):			return "������ֱ����ѹ";
		}
		break;

	case(F_AM_PCMSYN):
		switch(param)
		{
		case(P_AM_SYNOK):		return "ͬ������";
		case(P_AM_SYNNOCARRY):	return "�������ز�";
		case(P_AM_SYNFAS):		return "����FASͬ������";
		case(P_AM_SYNCAS):		return "����CAS MFͬ������";
		case(P_AM_SYNSLIP):		return "���Ի���������";
		case(P_AM_SYNALARMA):	return "Զ��Aλ�澯";
		case(P_AM_SYNALARMY):	return "Զ��CAS MF Yλ�澯";
		case(P_AM_SYNALLONE):	return "����ȫ1����";
		case(P_AM_SYNJITTER):	return "����������·�澯";
		}
		break;

	case (F_AM_MTP):
		switch(param)
		{
		case P_L3L4_STOP:		return "������·�ж�";
		case P_L3L4_LINKOK:		return "��·��ͨ����ͨ��·��";	  
		case P_L3L4_LINKFAIL:   return "��·�жϣ�ʧ����·��";	  
		case P_L3L4_DPCOK:      return "DPC �ɵ���";	  
		case P_L3L4_DPCNOLINK:  return "DPC ���ɵ���";
		}
		break;
	}
	return "δ֪����";
}

inline const char *GetMediaFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_MEDIA_Tx_Release):	return "�ͷŷ�����Դ";
	case(F_MEDIA_Rx_Release):	return "�ͷŽ�����Դ";
	case(F_MEDIA_Tx_File):		return "�����ļ�";
	case(F_MEDIA_Tx_Memory):	return "�����ڴ�";
	case(F_MEDIA_Tx_Index):		return "�����ڴ�����";
	case(F_MEDIA_Tx_TTStr):		return "�ı���";
	case(F_MEDIA_Tx_TTSFile):	return "�ı��ļ�";
	case(F_MEDIA_Tx_DTMF):		return "����DTMF";
	case(F_MEDIA_Tx_CPSign):	return "�����ź���";
	case(F_MEDIA_Rx_File):		return "���յ��ļ�";
	case(F_MEDIA_Rx_Memory):	return "�ڴ淽ʽ����";
	case(F_MEDIA_Rx_Pause):		return "������ͣ";
	case(F_MEDIA_Rx_Resume):	return "�������¿�ʼ";
	case(F_MEDIA_Error):		return "��Դ����/���";//(�·�Ϊ�������ϴ�Ϊ���)
	default:					return "δ֪����";
	}
}


inline const char *GetStopReason(unsigned char rsn)//����ý��ֹͣԭ��
{
  switch(rsn)
  {
  case RESTOP_DTMF: return "��DTMF���";
  case RESTOP_USER: return "��������ֹ";
  case RESTOP_FINISH: return "�ɹ����";
  case RESTOP_COMMERR: return "ͨѶ����";
  case RESTOP_CHREL: return "�����·�ͷ�";
  case RESTOP_FILERR: return "д���ļ�ʱʧ��";
  default:return "δ֪����";
  }
}

inline const char *GetRouterFuncNameStr(unsigned short func)
{
	switch(func)
	{
	case(F_R_SILENCE):		return "�������";
	case(F_R_MONITOR):		return "�������";
	case(F_R_TALK):			return "˫��ͨ��";
	case(F_R_TALK_RP):      return "˫��ͨ�����������";
	case(F_R_MONITOR_P):    return "���ӷ�����Դ";	
	case(F_R_TRANSTO_CT):	return "���͵�CT����һ��ʱ϶";
	case(F_R_MONITOR_CT):	return "����CT����һ��ʱ϶";
	default:				return "δ֪����";
	}
}

inline const char *GetCPToneNameStr(unsigned short cp)
{
	switch(cp)
	{
	case 1:return "������";
	case 2:return "������";
	case 3:return "æ��";
	default:return "δ֪�ź���";
	}
}

inline const char *GetVocFileFormatNameStr(unsigned short fmt)
{
	switch(fmt)
	{
    case 0:return "ALAW PCM";
	case 1:return "WAVE";
	case 2:return "CIRRUS ADPCM";
	case 3:return "G.723.1";
	case 4:return "Dialogic 6K";
	case 5:return "Dialogic 8K";
	default:return "δ֪��ʽ";
	}
}

inline const char *GetConfResultStr(unsigned char result)
{
	switch(result)
	{		
	case P_CF_OK               :return "��ȷ���";
	case P_CF_FUNCTIONERR      :return "FUNCTION ����";
	case P_CF_CONFNOTEXIST     :return "�����ҺŷǷ�";
	case P_CF_LOGICHNOTEXIST   :return "�������߼�ͨ���Ƿ�";
	case P_CF_CONFNOTCREATE    :return "ָ��������û�б�����";
	case P_CF_NOCONFCHRES      :return "����ͨ����Դ����";
	case P_CF_CR_CONFSIZE	   :return "���������Ҵ�С����";	
	case P_CF_CR_NOCONFRES     :return "������������Դ������ѱ�����"; 
	case P_CF_LK_LOGICHINCONF  :return "�������߼�ͨ�����������������";
	case P_CF_UL_LOGICHNOTCONF :return "�������߼�ͨ��û���ڻ�����";
	case P_CF_ST_NOTSAMEGROUP  :return "����������������߻��鲻��ͬһ����";
	default:return "δ֪���";
	}
}
#endif
