/*
@file     setting_samp.h
@brief    配置及逻辑管理(采集单元)
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
	
	UINT8  SampAddr;//采集器地址
	UINT8  MasterAddr;// 单元管理器地址	

	UINT8   AlrmTempPrec;// 温度 告警定值 是动作定值的百分之几
	UINT8   AlrmArcPrec;  // 温度 告警定值 是动作定值的百分之几
	UINT8   CfgVersion; // 高4位，低4位
	
	SINT8 	TempRadio[3];//[3];  　// 温度参数补偿 放大10倍
	UINT16   ArcZero;       // 弧光零漂
	UINT16  ArcRadio;     // 弧光比例系数 放大10000倍 0.025 -> 250	
	//UINT8  SettingSn;//管理器维护一个序号，采集器发现不一致，回复请求定值
	
	UINT8   InputBit; //开入位置
	UINT16  TempLimit;  // 温度越限定值；	
	UINT16  ArcLimit;   // 弧光越限定值；
	UINT8   TempDelay; // 温度延时；单位S
	UINT8   ArcDelay; // 弧光延时；单位S
	
	UINT8   OutFireType;    // 灭火器出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   TripType;    // 跳闸出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //跳闸单元地址
	UINT16   TripBO; // 跳闸单元IO 1~16

	UINT8 uiCrc8;//所有FLASH数据的CRC8
}stSetting_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - 从未配置 1-配置完成 2-逻辑完成 3-定值完成（进入正常工作）
	unUnitStatus_Def SysStatus;	//采集器状态16wei

	UINT32 SysTime;

	UINT8  Version;// 高4位，低4位;
	
	UINT8   ConfigVersion;// 单元管理器回复 配置版本 
	UINT8   SettingVersion;//单元管理器回复越限版本  
	UINT8   ReadSettingFlag;// 正在配置，不发送心跳
	UINT8   ReadSettingTime;//读取延时 重发
	UINT8   ReadSettingCnt;//读取次数 判断不能读取
	
	SINT16  SampTemp[3]; // 温度；  数据放大10倍，按照通道顺序存放
	UINT16  SampArc;	     // 弧光；   数据放大10倍
	UINT8   SampInput; //开入
	UINT8   SampOutput; //开出
	UINT8   SampIO;       // 采集控制器的IO状态；按位表示低4位开出，高4位开入

	UINT16 TempErrCnt[3];
	UINT16 ArcErrCnt;
	UINT16 PwrErrCnt[5]; //pwr 计数
	
	UINT8 HeartEvent; // 心跳事件
	UINT8 HeartCnt;//心跳计数

	//UINT8 SettingSnTmp;//临时 保存定值序号 等待定值配置后写入FLASH
	UINT8   CfgVersionTmp; // 高4位，低4位 临时 保存定值序号 等待定值配置后写入FLASH
	UINT8 SaveFlash; // 1 需要写FLASH

	UINT16 TripArcTimeCnt;//弧光延时
	UINT16 TripTempTimeCnt;//温度延时

	UINT16 AlmArcTimeCnt;//越限弧光延时
	UINT16 AlmTempTimeCnt;//越限温度延时

	UINT8 TripFlag;//已完成跳闸标记
	UINT8 FireFlag;//已完成灭火标记

	UINT8 FireDelay;//灭火延时
	UINT8 FireStatus;//按位解析

	UINT8 TripFaileTimeCnt;//跳闸后等待开入计数 
	UINT8 TripTempCh;//跳闸 缓存温度通道
	UINT16 TripTempVal;//跳闸 缓存温度值
	UINT16 TripArcVal;//跳闸 缓存弧光值

	UINT8 ResetTripTimeCnt;//复归计数
	UINT8 ResetFireTimeCnt;//复归计数

	UINT8 ResetTripFlagTimeCnt;//复归信号计数
	UINT8 ResetFireFlagTimeCnt;//复归信号计数

	UINT8 TripEvent;//trip事件
	UINT8 TripSendCnt;//发送计数

}stSysVar_def;

typedef struct
{
	stSetting_def stSetting;
	stSysVar_def stSysVar;
}stSysInfo_Def;



#define EVENT_BUF_CNT (10)
typedef struct
{
	UINT8 EventStatus;// 0-空 或已完成 1 -数据准备2-发送中
	UINT8 SendCnt;//发送计数
	stUserSlvEventReq_Def stSlvEvent;
}stSysEven_Def;

typedef struct
{
	UINT8 Len;// buf 长度
	UINT8 Res;//
	char Buf[DEBUG_BUF_LEN];
}stSDebug_Def;

extern stSDebug_Def stSDebug;


extern stSysInfo_Def stSysInfo;

extern stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


/////////////////////////////////////////////////////////
//setting.c 函数
/////////////////////////////////////////////////////////

//设置地址
extern void WriteUnitAddr(stUnitSetNetAddr_Def *addr);
//读地址
extern void ReadUnitAddr(stUnitSetNetAddr_Def *addr);
//读配置
extern void ReadSettingCfg(stUnitSettingCfg_Def *SettingCfg);
//读DBG
extern void ReadUnitDbg(stUnitReadDbg_Def *pDbg);

//添加事件到缓冲区
extern void AddEvent(stUserSlvEventReq_Def *SlvEvent);
//心跳事件 发送采样值
extern void SendSampling(void);
//检查重发 每250ms调用
extern void CheckResend(void);

//主函数调用
extern void SysInitSampUnit(void);
extern void SampTimerTask(void);
extern void sResetSystem(void);

////////////////////////////////////////////////////
//docom.c 函数
////////////////////////////////////////////////////
extern UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen);
//发送温度值
extern void SlvSampTempReq(void);
//发送弧光值
extern void SlvSampArcReq(void);
// 发送跳闸事件
extern void SlvTripReq(void);
// 发送记录事件
extern void SlvEventReq(stSysEven_Def *SysEven);


#endif

