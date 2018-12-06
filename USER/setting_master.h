/*
@file     setting_master.h
@brief    配置及逻辑管理(管理单元)
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
	
	
	UINT8  U8MasterAddr;// 单元管理器地址	0xHH  (由红外写入)
	UINT16  U16MasterAddr;// 单元管理器地址	0xHH00  stConfig内的ADDR 只做写入判断
	//UINT16  U16ConsoleAddr;//台区管理器地址	0x00XX 在stConfig

	UINT8    NET1ID;     //台区LORA 通信网络ID
	UINT8    NET2ID;	   //跳闸LORA 通信网络ID		
	UINT16 NET1FR;     //台区LORA 通信网络频率
	UINT16 NET2FR;     //跳闸LORA 通信网络频率

	UINT16   ArcZero;       // 弧光零漂，
	UINT16   ArcRadio;       // 弧光比例系数 放大10000倍 0.025 -> 250	
	SINT8    TempRadio[3];//[3]; 　// 温度参数补偿 放大10倍

	UINT8  SampCnt;//采集器个数
	stUnitConfigReq_Def stConfig;
	stUnitConfig_Def stSampUnit[SAMPLING_UNIT_CNT_MAX];
	stLimitReq_Def stLimit;

	UINT8 uiCrc8;//所有FLASH数据的CRC8
	
}stMSetting_def;

typedef struct
{
	UINT16  UnitID;       // 装置ID;
	
	UINT8 HeartLostCnt;//心跳计数
	UINT8 HeartLostFlag;//心跳丢失标记

	UINT8 SetMasterIDFlag; //是否设置成功 没有成功需要 间隔写
	UINT8 SettingReq;//需要发送定值
	UINT8 RespMask ;//收到三个报文回复6 7 8

	UINT8 TempOverEventFlag;//温度越限已发送
	UINT8 ArcOverEventFlag;//弧光越限已发送
	UINT8 TripEventFlag;//跳闸已发送
	UINT8 FireEventFlag;//灭火已发送
	UINT8 TripFailedFlag;//跳失败已发送 TRIPFAILED
	UINT8 TripSendErrFlag;//跳闸发送失败
}stUnitVar_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - 初始化工作 1-收到心跳 2-配置检查 3-配置下发4-正常工作
	unUnitStatus_Def SysStatus;	//管理器状态16wei

	UINT32 SysTime;

	SINT16  Temp1;//温度1  ; 数据放大10倍                
	SINT16  Temp2 ;//温度2; 数据放大10倍
	SINT16  Temp3 ;//温度3 ;数据放大10倍
	UINT16  Arc;     // 弧光;   数据放大10倍 ; 当为跳闸单元时，该字段表示输出状态 按位解析
	UINT8   SampIO;      // 采集控制器的IO状态；按位表示低4位开出，高4位开入
	UINT8   Version; // 高4位，低4位

	UINT8   ConfigVersion;// 台区回复 配置版本 //单元管理器发现不同读取配置数据
	UINT8   SettingVersion;//台区回复越限版本  //单元管理器发现不同读取越限数据

	UINT16 PwrErrCnt[5]; //pwr 计数

	UINT8 HeartEvent; // 心跳事件
	UINT8 HeartCnt;//心跳计数

	UINT8 TripEvent;//trip事件
	UINT8 TripSendCnt;//发送计数

	UINT8 TripRestCnt;//复归计数

	UINT8 ReadConfigFlag ;//读配置停 心跳
	UINT8 ReadConfigIndex ;//台区配置采集器 序号
	UINT8 SampUnitCntTmp;//接收个数缓存
	UINT8 SaveFalshCfg;//需要写FLASH 标记
	
	UINT8 TripUnitCnt;//跳闸单元个数
	UINT8 TripHeartSn;// 从0 - TripUnitCnt-1  循环发送
	UINT16 TripUnitAddrBuf[10];//跳闸单元地址

	UINT8 AllUnitCnt;//  自身 + 采集单元+  跳闸单元 的总数
	UINT8 SendUnitSn;// 从0 - AllUnitCnt-1  循环发送

	stUnit_Def stUnit[SAMPLING_UNIT_CNT_MAX+11];//所有单元的采样
	stUnitVar_def stUnitVar[SAMPLING_UNIT_CNT_MAX+11];//所有单元 变量
	
}stMSysVar_def;

typedef struct
{
	stMSetting_def stSetting;
	stMSysVar_def stSysVar;
}stMSysInfo_Def;


#define EVENT_CNT_MAX (100)
typedef struct
{
	UINT8 EventStatus;// 0-空 或已完成 1 -数据准备2-发送中
	UINT8 EventSendCnt;//发送计数
	stEventReq_Def stEvent;
}stMasterEvent_Def;

typedef struct
{
	UINT8 Len;// buf 长度
	UINT8 Res;//
	char Buf[DEBUG_BUF_LEN];
}stMDebug_Def;

/*
typedef struct
{
	UINT8 ReSendFlag;// 重发标记
	UINT8 ReSendCnt;// 计数
	stActiveReq_Def stActive;
}stMSendTrip_Def;
*/

extern stActiveReq_Def stMSendTrip;

extern stMDebug_Def stMDebug;


extern stMSysInfo_Def stMSysInfo;

extern stMasterEvent_Def stMasterEvent[EVENT_CNT_MAX];


/////////////////////////////////////////////////////////
//setting 函数
/////////////////////////////////////////////////////////

//设置地址
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//读地址
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//读DBG
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
// 发送跳闸重发
extern void ReSendUnitTripReq(stActiveReq_Def *pstActiveReq);

//主函数调用
extern void SysInitMaster(void);
extern void MasterTimerTask(void);

extern void mResetSystem(void);


////////////////////////////////////////////////////
//docom 函数
////////////////////////////////////////////////////

extern void UnitSendReadConfig(UINT8 Start,UINT8 Cnt);
extern void UnitSendReadSetting(void);

extern void mSlvSetMasterReq(UINT16 SampAddr);
extern void mSlvSetZeroReq(UINT16 SampAddr);
extern void mSlvSetLogicReq(UINT16 SampAddr);
extern void mSlvSetSettingReq(UINT16 SampAddr);


#endif

