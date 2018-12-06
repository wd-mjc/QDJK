/*
@file     setting_trip.h
@brief    配置及逻辑管理(跳闸单元)
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
#define TRIP_UNIT_RESET_DEF (5)//5s 继电器返回
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
	
	//UINT8  SampAddr;//采集器地址
	//UINT16  MasterAddr;// 单元管理器地址	
	UINT16 TripAddr; //跳闸单元地址

	//UINT8    NET1ID;     //台区LORA 通信网络ID
	UINT8    NET2ID;	   //跳闸LORA 通信网络ID		
	UINT16 NET2FR;     //跳闸LORA 通信网络频率

	UINT8 uiCrc8;//所有FLASH数据的CRC8

}stTSetting_def;

typedef struct
{
	UINT16 MasterAddr;
	UINT8 HeartLostCnt;//心跳计数
	UINT8 HeartLostFlag;//心跳丢失标记
}stMasterAddr_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - 初始化 1-（进入正常工作）

	UINT8   Version; // 高4位，低4位
	unUnitStatus_Def SysStatus;	//采集器状态16wei

	UINT32 Time;

	UINT16 PwrErrCnt[5]; //pwr 计数

	UINT16 TripDo;//单元出口状态 按位解析

	UINT8 TripChDelay[TRIP_UNIT_TRIPCH];//通道跳闸后延时恢复

	UINT16 TripResetDelay;//复归延时

	UINT8 SaveFalshCfg;//需要写FLASH 标记
	
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
//变量
/////////////////////////////////////////////////////////
extern stTSysInfo_Def stTSysInfo;


/////////////////////////////////////////////////////////
//setting 函数
/////////////////////////////////////////////////////////

//设置地址
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//读地址
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//读DBG
extern void ReadUnitDbg(stUnitReadDbg_Def *pDbg);

extern void AddMasterAddr(UINT16 Addr);
extern UINT8 CheckMasterAddr(UINT16 Addr);

extern void SysInitTripUnit(void);
extern void TripTimerTask(void);
extern void tResetSystem(void);

////////////////////////////////////////////////////
//docom 函数
////////////////////////////////////////////////////





#endif

