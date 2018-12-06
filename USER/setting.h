#ifndef		_SETTING_H
#define		_SETTING_H

#include<string.h>
#include "common.h"
#include "type_slv.h"
#include "type_com.h"

typedef struct
{
	UINT16 SysCheck1;
	UINT16 SysCheck2;
	
	UINT8  SampAddr;//�ɼ�����ַ
	UINT8  MasterAddr;// ��Ԫ��������ַ	
	
	SINT8 	TempRadio[3];//;  ��// �¶Ȳ������� �Ŵ�10��
	UINT8   ArcZero;       // ������Ư��		
	UINT16  ArcRadio;     // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
	UINT8  SettingSn;//������ά��һ����ţ��ɼ������ֲ�һ�£��ظ�����ֵ
	
	UINT8   InputBit; //����λ��
	UINT16  TempLimit;  // �¶�Խ�޶�ֵ��	
	UINT16  ArcLimit;   // ����Խ�޶�ֵ��
	UINT8   TempDelay; // �¶���ʱ����λS
	UINT8   ArcDelay; // ������ʱ����λS
	
	UINT8   OutFireType;    // ����������߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   TripType;    // ��բ�����߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //��բ��Ԫ��ַ
	UINT8   TripIO; // ��բ��ԪIO 1~16	
}stSetting_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - ��δ���� 1-������� 2-�߼���� 3-��ֵ��ɣ���������������
	unUnitStatus_Def SysStatus;	//�ɼ���״̬16wei

	UINT32 SysTime;

	UINT8  Version[3];
	
	SINT16  SampTemp[3]; // �¶ȣ�  ���ݷŴ�10��������ͨ��˳����
	UINT16  SampArc;	     // ���⣻   ���ݷŴ�10��
	UINT8   SampInput; //����
	UINT8   SampOutput; //����
	UINT8   SampIO;       // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����

	UINT8 HeartEvent; // �����¼�
	UINT8 HeartCnt;//��������

	UINT8 SettingSnTmp;//��ʱ ���涨ֵ��� �ȴ���ֵ���ú�д��FLASH
	UINT8 SaveFlash; // 1 ��ҪдFLASH

	UINT16 TripArcTimeCnt;//������ʱ
	UINT16 TripTempTimeCnt;//�¶���ʱ

	UINT16 AlmArcTimeCnt;//Խ�޻�����ʱ
	UINT16 AlmTempTimeCnt;//Խ���¶���ʱ

	UINT8 TripFlag;//�������բ���
	UINT8 FireFlag;//����������

	UINT8 TripFaileTimeCnt;//��բ��ȴ�������� 
	UINT8 TripTempCh;//��բ �����¶�ͨ��
	UINT16 TripTempVal;//��բ �����¶�ֵ
	UINT16 TripArcVal;//��բ ���满��ֵ

	UINT8 ResetTripTimeCnt;//�������
	UINT8 ResetFireTimeCnt;//�������

	UINT8 TripEvent;//trip�¼�
	UINT8 TripSendCnt;//���ͼ���
	
}stSysVar_def;

typedef struct
{
	stSetting_def stSetting;
	stSysVar_def stSysVar;
}stSysInfo_Def;



#define EVENT_BUF_CNT (10)
typedef struct
{
	UINT8 EventStatus;// 0-�� ������� 1 -����׼��2-������
	UINT8 SendCnt;//���ͼ���
	stUserSlvEventReq_Def stSlvEvent;
}stSysEven_Def;




extern stSysInfo_Def stSysInfo;

extern stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


/////////////////////////////////////////////////////////
//setting.c ����
/////////////////////////////////////////////////////////
//����¼���������
extern void AddEvent(stUserSlvEventReq_Def *SlvEvent);
//�����¼� ���Ͳ���ֵ
extern void SendSampling(void);
//����ط� ÿ250ms����
extern void CheckResend(void);

////////////////////////////////////////////////////
//docom.c ����
////////////////////////////////////////////////////
extern UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen);
//�����¶�ֵ
extern void SlvSampTempReq(void);
//���ͻ���ֵ
extern void SlvSampArcReq(void);
// ������բ�¼�
extern void SlvTripReq(void);
// ���ͼ�¼�¼�
extern void SlvEventReq(stSysEven_Def *SysEven);


#endif

