/*
@file     setting_samp.h
@brief    ���ü��߼�����(�ɼ���Ԫ)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#ifndef		_SETTING_SAMP_H
#define		_SETTING_SAMP_H

#include<string.h>
#include "common.h"
#include "type_slv.h"

#define SAMP_UINT_VERSION (0X13)

#define S_MASTER_ADDR (0X02)
#define S_SAMP_ADDR (0X01)
//AlrmTempPrec
#define S_ALRM_PREC (80)
//ArcRadio
#define S_ARC_RADIO (100)//(0.01)
#define S_ARC_ZERO (420)//
#define S_ARC_RADIOMAX (ARC_RADIO_MAX) //(0.1)

#define S_TEMP_ERR_MIN (-500)
#define S_TEMP_ERR_MAX (2700)
#define S_ARC_ERR_MIN (10)
#define S_ARC_ERR_MAX (4000)
#define S_SENSOR_ERR_CNT (240)//60s

#define S_PWIN_ERR_MIN (9.0)
#define S_PWIN_ERR_MAX (29.0)
#define S_PW50_ERR_MIN (4.4)
#define S_PW50_ERR_MAX (5.6)
#define S_PW90_ERR_MIN (8.0)
#define S_PW90_ERR_MAX (10.0)
#define S_PW240_ERR_MIN (22.0)
#define S_PW240_ERR_MAX (26.0)
#define S_PW33_ERR_MIN (3.0)
#define S_PW33_ERR_MAX (3.6)
#define S_PWR_ERR_CNT (160)//40s

#pragma pack(1)

typedef struct
{
	UINT16 SysCheck1;
	UINT16 SysCheck2;
	
	UINT8  SampAddr;//�ɼ�����ַ
	UINT8  MasterAddr;// ��Ԫ��������ַ	

	UINT8   AlrmTempPrec;// �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   AlrmArcPrec;  // �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   CfgVersion; // ��4λ����4λ
	
	SINT8 	TempRadio[3];//[3];  ��// �¶Ȳ������� �Ŵ�10��
	UINT16   ArcZero;       // ������Ư
	UINT16  ArcRadio;     // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
	//UINT8  SettingSn;//������ά��һ����ţ��ɼ������ֲ�һ�£��ظ�����ֵ
	
	UINT8   InputBit; //����λ��
	UINT16  TempLimit;  // �¶�Խ�޶�ֵ��	
	UINT16  ArcLimit;   // ����Խ�޶�ֵ��
	UINT8   TempDelay; // �¶���ʱ����λS
	UINT8   ArcDelay; // ������ʱ����λS
	
	UINT8   OutFireType;    // ����������߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   TripType;    // ��բ�����߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //��բ��Ԫ��ַ
	UINT16   TripBO; // ��բ��ԪIO 1~16

	UINT8 uiCrc8;//����FLASH���ݵ�CRC8
}stSetting_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - ��δ���� 1-������� 2-�߼���� 3-��ֵ��ɣ���������������
	unUnitStatus_Def SysStatus;	//�ɼ���״̬16wei

	UINT32 SysTime;

	UINT8  Version;// ��4λ����4λ;
	
	UINT8   ConfigVersion;// ��Ԫ�������ظ� ���ð汾 
	UINT8   SettingVersion;//��Ԫ�������ظ�Խ�ް汾  
	UINT8   ReadSettingFlag;// �������ã�����������
	UINT8   ReadSettingTime;//��ȡ��ʱ �ط�
	UINT8   ReadSettingCnt;//��ȡ���� �жϲ��ܶ�ȡ
	
	SINT16  SampTemp[3]; // �¶ȣ�  ���ݷŴ�10��������ͨ��˳����
	UINT16  SampArc;	     // ���⣻   ���ݷŴ�10��
	UINT8   SampInput; //����
	UINT8   SampOutput; //����
	UINT8   SampIO;       // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����

	UINT16 TempErrCnt[3];
	UINT16 ArcErrCnt;
	UINT16 PwrErrCnt[5]; //pwr ����
	
	UINT8 HeartEvent; // �����¼�
	UINT8 HeartCnt;//��������

	//UINT8 SettingSnTmp;//��ʱ ���涨ֵ��� �ȴ���ֵ���ú�д��FLASH
	UINT8   CfgVersionTmp; // ��4λ����4λ ��ʱ ���涨ֵ��� �ȴ���ֵ���ú�д��FLASH
	UINT8 SaveFlash; // 1 ��ҪдFLASH

	UINT16 TripArcTimeCnt;//������ʱ
	UINT16 TripTempTimeCnt;//�¶���ʱ

	UINT16 AlmArcTimeCnt;//Խ�޻�����ʱ
	UINT16 AlmTempTimeCnt;//Խ���¶���ʱ

	UINT8 TripFlag;//�������բ���
	UINT8 FireFlag;//����������

	UINT8 FireDelay;//�����ʱ
	UINT8 FireStatus;//��λ����

	UINT8 TripFaileTimeCnt;//��բ��ȴ�������� 
	UINT8 TripTempCh;//��բ �����¶�ͨ��
	UINT16 TripTempVal;//��բ �����¶�ֵ
	UINT16 TripArcVal;//��բ ���满��ֵ

	UINT8 ResetTripTimeCnt;//�������
	UINT8 ResetFireTimeCnt;//�������

	UINT8 ResetTripFlagTimeCnt;//�����źż���
	UINT8 ResetFireFlagTimeCnt;//�����źż���

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

typedef struct
{
	UINT8 Len;// buf ����
	UINT8 Res;//
	char Buf[DEBUG_BUF_LEN];
}stSDebug_Def;

extern stSDebug_Def stSDebug;


extern stSysInfo_Def stSysInfo;

extern stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


/////////////////////////////////////////////////////////
//setting.c ����
/////////////////////////////////////////////////////////

//���õ�ַ
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//����ַ
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//������
extern void ReadSettingCfg(stUnitSettingCfg_Def *SettingCfg);
//��DBG
extern void ReadUnitDbg(stUnitReadDbg_Def *pDbg);

//����¼���������
extern void AddEvent(stUserSlvEventReq_Def *SlvEvent);
//�����¼� ���Ͳ���ֵ
extern void SendSampling(void);
//����ط� ÿ250ms����
extern void CheckResend(void);

//����������
extern void SysInitSampUnit(void);
extern void SampTimerTask(void);
extern void sResetSystem(void);

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

