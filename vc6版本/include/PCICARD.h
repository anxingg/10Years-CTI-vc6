#ifndef __PCICARD_H__
#define __PCICARD_H__

long  __stdcall PCIGETCARDNUM(); //���Ŀǰʵ�ʴ��ڲ�����Ŀ�����Ŀ
long  __stdcall PCIGETCONFIGCARDNUM();//���������ļ�������ʹ�õĿ���Ŀ

/*  GETCARDHARDWARESERIAL �������ؽ����λ������
    bit[15.. 0] = CardHardwareSerialNo // ʵ�ʵı���Ӳ�����к�
    bit[19..16] = CardHardwareLine     // Ӳ��֧�ֵ�ͨ����Ŀ
	                                   // ������:1--8 ��ʾ֧��1--8��E1   
									   // ģ����:1--6 ��ʾ֧��4--24��(��4��Ϊ��λ)
    bit[23..20] = CardHardwareRes      // ���ϴ��е���Դ����bit3=fsk,bit2=fax
    bit[27..24] = CardSwitch           //����(�������߿��غ�0--15)
	bit[31..28] = CardHardwareFlag     //�����ͱ�־(1=4��ģ�⿨,2=���ֿ�,3=24��ģ�⿨)
*/

long  __stdcall PCIGETCARDHARDWARESERIAL(long CARDNO); //���ض�Ӧλ�õĿ���Ӳ�����к�
long  __stdcall PCIGETCARDHARDWARETYPE(long CARDNO); //���ض�Ӧλ�õĿ���Ӳ������
long  __stdcall PCIGETLOGICCHNUM(long CHTYPE); //��ȡָ��������·���õ��߼�ͨ����Ŀ
long  __stdcall PCIGETTOTALLOGICCHNUM(); //��ȡϵͳ������������·���õ��߼�ͨ����Ŀ�ܺ� 
long  __stdcall PCIGRABMSG(RMSG *PMSG,unsigned short MSGNUM);//������������Ϣ�������Ƿ��յ���Ϣ�����������أ�
long  __stdcall PCISENDMSG(RMSG *PMSG); //������������Ϣ�������Ƿ��ͳɹ���Ϣ�����������أ�
BOOL  __stdcall PCIINITCARD(long PARAM); //��ʼ��Ӳ��
BOOL  __stdcall PCIEXITCARD(); //�ر�Ӳ��
//long  __stdcall PCIGETSLOT(long CHTYPE,long CHANNEL,long PARAM);//ȡ�߼�ͨ���ŵ�����ʱ϶
#endif