/*
@file     setting_master.h
@brief    ���ü��߼�����(����Ԫ)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#ifndef		_SETTING_MASTER_H
#define		_SETTING_MASTER_H

#include <string.h>
#include "common.h"
#include "type_slv.h"
#include "type_com.h"



#define MASTER_VERSION (0X13)

#define MASTER_ADDRU8_DEF (0X02)
#define MASTER_ADDRU16_DEF (0X0200)
#define CONSOLE_ADDR_DEF (0X0005)

#define M_PWIN_ERR_MIN (9.0)
#define M_PWIN_ERR_MAX (29.0)
#define M_PW50_ERR_MIN (4.4)
#define M_PW50_ERR_MAX (5.6)
#define M_PW90_ERR_MIN (8.0)
#define M_PW90_ERR_MAX (10.0)
#define M_PW240_ERR_MIN (22.0)
#define M_PW240_ERR_MAX (26.0)
#define M_PW33_ERR_MIN (3.0)
#define M_PW33_ERR_MAX (3.6)
#define M_PWR_ERR_CNT (40)//40s

#pragma pack(1)
typedef struct
{
	UINT16 SysCheck1;
	UINT16 SysCheck2;
	
	
	UINT8  U8MasterAddr;// ��Ԫ��������ַ	0xHH  (�ɺ���д��)
	UINT16  U16MasterAddr;// ��Ԫ��������ַ	0xHH00  stConfig�ڵ�ADDR ֻ��д���ж�
	//UINT16  U16ConsoleAddr;//̨����������ַ	0x00XX ��stConfig

	UINT8    NET1ID;     //̨��LORA ͨ������ID
	UINT8    NET2ID;	   //��բLORA ͨ������ID		
	UINT16 NET1FR;     //̨��LORA ͨ������Ƶ��
	UINT16 NET2FR;     //��բLORA ͨ������Ƶ��

	UINT16   ArcZero;       // ������Ư��
	UINT16   ArcRadio;       // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
	SINT8    TempRadio[3];//[3]; ��// �¶Ȳ������� �Ŵ�10��

	UINT8  SampCnt;//�ɼ�������
	stUnitConfigReq_Def stConfig;
	stUnitConfig_Def stSampUnit[SAMPLING_UNIT_CNT_MAX];
	stLimitReq_Def stLimit;

	UINT8 uiCrc8;//����FLASH���ݵ�CRC8
	
}stMSetting_def;

typedef struct
{
	UINT16  UnitID;       // װ��ID;
	
	UINT8 HeartLostCnt;//��������
	UINT8 HeartLostFlag;//������ʧ���

	UINT8 SetMasterIDFlag; //�Ƿ����óɹ� û�гɹ���Ҫ ���д
	UINT8 SettingReq;//��Ҫ���Ͷ�ֵ
	UINT8 RespMask ;//�յ��������Ļظ�6 7 8

	UINT8 TempOverEventFlag;//�¶�Խ���ѷ���
	UINT8 ArcOverEventFlag;//����Խ���ѷ���
	UINT8 TripEventFlag;//��բ�ѷ���
	UINT8 FireEventFlag;//����ѷ���
	UINT8 TripFailedFlag;//��ʧ���ѷ��� TRIPFAILED
	UINT8 TripSendErrFlag;//��բ����ʧ��
}stUnitVar_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - ��ʼ������ 1-�յ����� 2-���ü�� 3-�����·�4-��������
	unUnitStatus_Def SysStatus;	//������״̬16wei

	UINT32 SysTime;

	SINT16  Temp1;//�¶�1  ; ���ݷŴ�10��                
	SINT16  Temp2 ;//�¶�2; ���ݷŴ�10��
	SINT16  Temp3 ;//�¶�3 ;���ݷŴ�10��
	UINT16  Arc;     // ����;   ���ݷŴ�10�� ; ��Ϊ��բ��Ԫʱ�����ֶα�ʾ���״̬ ��λ����
	UINT8   SampIO;      // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����
	UINT8   Version; // ��4λ����4λ

	UINT8   ConfigVersion;// ̨���ظ� ���ð汾 //��Ԫ���������ֲ�ͬ��ȡ��������
	UINT8   SettingVersion;//̨���ظ�Խ�ް汾  //��Ԫ���������ֲ�ͬ��ȡԽ������

	UINT16 PwrErrCnt[5]; //pwr ����

	UINT8 HeartEvent; // �����¼�
	UINT8 HeartCnt;//��������

	UINT8 TripEvent;//trip�¼�
	UINT8 TripSendCnt;//���ͼ���

	UINT8 TripRestCnt;//�������

	UINT8 ReadConfigFlag ;//������ͣ ����
	UINT8 ReadConfigIndex ;//̨�����òɼ��� ���
	UINT8 SampUnitCntTmp;//���ո�������
	UINT8 SaveFalshCfg;//��ҪдFLASH ���
	
	UINT8 TripUnitCnt;//��բ��Ԫ����
	UINT8 TripHeartSn;// ��0 - TripUnitCnt-1  ѭ������
	UINT16 TripUnitAddrBuf[10];//��բ��Ԫ��ַ

	UINT8 AllUnitCnt;//  ���� + �ɼ���Ԫ+  ��բ��Ԫ ������
	UINT8 SendUnitSn;// ��0 - AllUnitCnt-1  ѭ������

	stUnit_Def stUnit[SAMPLING_UNIT_CNT_MAX+11];//���е�Ԫ�Ĳ���
	stUnitVar_def stUnitVar[SAMPLING_UNIT_CNT_MAX+11];//���е�Ԫ ����
	
}stMSysVar_def;

typedef struct
{
	stMSetting_def stSetting;
	stMSysVar_def stSysVar;
}stMSysInfo_Def;


#define EVENT_CNT_MAX (100)
typedef struct
{
	UINT8 EventStatus;// 0-�� ������� 1 -����׼��2-������
	UINT8 EventSendCnt;//���ͼ���
	stEventReq_Def stEvent;
}stMasterEvent_Def;

typedef struct
{
	UINT8 Len;// buf ����
	UINT8 Res;//
	char Buf[DEBUG_BUF_LEN];
}stMDebug_Def;

/*
typedef struct
{
	UINT8 ReSendFlag;// �ط����
	UINT8 ReSendCnt;// ����
	stActiveReq_Def stActive;
}stMSendTrip_Def;
*/

extern stActiveReq_Def stMSendTrip;

extern stMDebug_Def stMDebug;


extern stMSysInfo_Def stMSysInfo;

extern stMasterEvent_Def stMasterEvent[EVENT_CNT_MAX];


/////////////////////////////////////////////////////////
//setting ����
/////////////////////////////////////////////////////////

//���õ�ַ
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//����ַ
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//��DBG
extern void ReadUnitDbg(stUnitReadDbg_Def *pDbg);

extern void Init_SettingVar( void );
extern void UnitAddrLinkLost(UINT16 Addr);

extern stUnit_Def *GetUnitByAddr(UINT16 Addr);
extern stUnit_Def *GetUnitByIndex(UINT8 Index);
extern stUnitConfig_Def *GetConfigByAddr(UINT16 Addr);
extern stUnitConfig_Def *GetConfigByIndex(UINT8 Index);
extern stUnitVar_def *GetVarByAddr(UINT16 Addr);
extern stUnitVar_def *GetVarByIndex(UINT8 Index);

extern void SendUnitSampling(void);
extern void MasterCheckLogic(void);
extern void MasterCheckEvent(void);
// ������բ�ط�
extern void ReSendUnitTripReq(stActiveReq_Def *pstActiveReq);

//����������
extern void SysInitMaster(void);
extern void MasterTimerTask(void);

extern void mResetSystem(void);


////////////////////////////////////////////////////
//docom ����
////////////////////////////////////////////////////

extern void UnitSendReadConfig(UINT8 Start,UINT8 Cnt);
extern void UnitSendReadSetting(void);

extern void mSlvSetMasterReq(UINT16 SampAddr);
extern void mSlvSetZeroReq(UINT16 SampAddr);
extern void mSlvSetLogicReq(UINT16 SampAddr);
extern void mSlvSetSettingReq(UINT16 SampAddr);


#endif

