/*
@file     setting_trip.h
@brief    ���ü��߼�����(��բ��Ԫ)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#ifndef		_SETTING_TRIP_H
#define		_SETTING_TRIP_H

#include<string.h>
#include "common.h"
#include "type_slv.h"



#define TRIP_UNIT_VERSION_DEF (0X13)

#define TRIP_FLASH_SAVE_FLAG (0x5343)

#define MASTER_ADDR_CNTMAX (16)
#define TRIP_UNIT_TRIPCH (16)
#define TRIP_UNIT_RESET_DEF (5)//5s �̵�������
#define TRIP_UNIT_ADDR_DEF (0XFF01)
#define TRIP_UNIT_ADDR_MASK (0XFF00)

#define T_PWIN_ERR_MIN (9.0)
#define T_PWIN_ERR_MAX (29.0)
#define T_PW50_ERR_MIN (4.4)
#define T_PW50_ERR_MAX (5.6)
#define T_PW90_ERR_MIN (8.0)
#define T_PW90_ERR_MAX (10.0)
#define T_PW240_ERR_MIN (22.0)
#define T_PW240_ERR_MAX (26.0)
#define T_PW33_ERR_MIN (3.0)
#define T_PW33_ERR_MAX (3.6)
#define T_PWR_ERR_CNT (40)//40s

#pragma pack(1)
typedef struct
{
	UINT16 SysCheck1;
	UINT16 SysCheck2;
	
	//UINT8  SampAddr;//�ɼ�����ַ
	//UINT16  MasterAddr;// ��Ԫ��������ַ	
	UINT16 TripAddr; //��բ��Ԫ��ַ

	//UINT8    NET1ID;     //̨��LORA ͨ������ID
	UINT8    NET2ID;	   //��բLORA ͨ������ID		
	UINT16 NET2FR;     //��բLORA ͨ������Ƶ��

	UINT8 uiCrc8;//����FLASH���ݵ�CRC8

}stTSetting_def;

typedef struct
{
	UINT16 MasterAddr;
	UINT8 HeartLostCnt;//��������
	UINT8 HeartLostFlag;//������ʧ���
}stMasterAddr_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - ��ʼ�� 1-����������������

	UINT8   Version; // ��4λ����4λ
	unUnitStatus_Def SysStatus;	//�ɼ���״̬16wei

	UINT32 Time;

	UINT16 PwrErrCnt[5]; //pwr ����

	UINT16 TripDo;//��Ԫ����״̬ ��λ����

	UINT8 TripChDelay[TRIP_UNIT_TRIPCH];//ͨ����բ����ʱ�ָ�

	UINT16 TripResetDelay;//������ʱ

	UINT8 SaveFalshCfg;//��ҪдFLASH ���
	
	UINT8 MasterAddrCnt;

	//UINT16 MasterAddrBuf[MASTER_ADDR_CNTMAX];
	stMasterAddr_def stMasterAddr[MASTER_ADDR_CNTMAX];
	
}stTSysVar_def;

typedef struct
{
	stTSetting_def stSetting;
	stTSysVar_def stSysVar;
}stTSysInfo_Def;

/////////////////////////////////////////////////////////
//����
/////////////////////////////////////////////////////////
extern stTSysInfo_Def stTSysInfo;


/////////////////////////////////////////////////////////
//setting ����
/////////////////////////////////////////////////////////

//���õ�ַ
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//����ַ
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//��DBG
extern void ReadUnitDbg(stUnitReadDbg_Def *pDbg);

extern void AddMasterAddr(UINT16 Addr);
extern UINT8 CheckMasterAddr(UINT16 Addr);

extern void SysInitTripUnit(void);
extern void TripTimerTask(void);
extern void tResetSystem(void);

////////////////////////////////////////////////////
//docom ����
////////////////////////////////////////////////////





#endif

