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
	
	UINT8  SampAddr;//采集器地址
	UINT8  MasterAddr;// 单元管理器地址	
	
	SINT8 	TempRadio[3];//;  　// 温度参数补偿 放大10倍
	UINT8   ArcZero;       // 弧光零漂，		
	UINT16  ArcRadio;     // 弧光比例系数 放大10000倍 0.025 -> 250	
	UINT8  SettingSn;//管理器维护一个序号，采集器发现不一致，回复请求定值
	
	UINT8   InputBit; //开入位置
	UINT16  TempLimit;  // 温度越限定值；	
	UINT16  ArcLimit;   // 弧光越限定值；
	UINT8   TempDelay; // 温度延时；单位S
	UINT8   ArcDelay; // 弧光延时；单位S
	
	UINT8   OutFireType;    // 灭火器出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   TripType;    // 跳闸出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //跳闸单元地址
	UINT8   TripIO; // 跳闸单元IO 1~16	
}stSetting_def;

typedef struct
{
	UINT8 SysWorkMod;	//	0 - 从未配置 1-配置完成 2-逻辑完成 3-定值完成（进入正常工作）
	unUnitStatus_Def SysStatus;	//采集器状态16wei

	UINT32 SysTime;

	UINT8  Version[3];
	
	SINT16  SampTemp[3]; // 温度；  数据放大10倍，按照通道顺序存放
	UINT16  SampArc;	     // 弧光；   数据放大10倍
	UINT8   SampInput; //开入
	UINT8   SampOutput; //开出
	UINT8   SampIO;       // 采集控制器的IO状态；按位表示低4位开出，高4位开入

	UINT8 HeartEvent; // 心跳事件
	UINT8 HeartCnt;//心跳计数

	UINT8 SettingSnTmp;//临时 保存定值序号 等待定值配置后写入FLASH
	UINT8 SaveFlash; // 1 需要写FLASH

	UINT16 TripArcTimeCnt;//弧光延时
	UINT16 TripTempTimeCnt;//温度延时

	UINT16 AlmArcTimeCnt;//越限弧光延时
	UINT16 AlmTempTimeCnt;//越限温度延时

	UINT8 TripFlag;//已完成跳闸标记
	UINT8 FireFlag;//已完成灭火标记

	UINT8 TripFaileTimeCnt;//跳闸后等待开入计数 
	UINT8 TripTempCh;//跳闸 缓存温度通道
	UINT16 TripTempVal;//跳闸 缓存温度值
	UINT16 TripArcVal;//跳闸 缓存弧光值

	UINT8 ResetTripTimeCnt;//复归计数
	UINT8 ResetFireTimeCnt;//复归计数

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




extern stSysInfo_Def stSysInfo;

extern stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


/////////////////////////////////////////////////////////
//setting.c 函数
/////////////////////////////////////////////////////////
//添加事件到缓冲区
extern void AddEvent(stUserSlvEventReq_Def *SlvEvent);
//心跳事件 发送采样值
extern void SendSampling(void);
//检查重发 每250ms调用
extern void CheckResend(void);

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

