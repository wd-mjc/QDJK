#ifndef TYPE_SLV_H_
#define TYPE_SLV_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
/////////////协议///////////
//使用CAN2.0B协议的扩展帧格式
//1   11  1   1  18   1   1  1  4  0~64  16  2  7
//SOF ID SRR IDE EID RTR  1  0 DLC DATA CRC ACK EOF
//DEST ADDR 使用ID的后8位
//SOUR ADDR 使用EID的后8位
//
//单元管理器ADDR 16位  高8位为单元编号，低8位为0
//采集器ADDR 16位 高8位与管理器高8位相同，低8位从1到255编号
//单元管理器发送数据 DEST 使用采集器编号，SOUR 使用高8位 屏蔽模式接收 只接收高8位相同数据帧
//采集器发送数据 DEST  使用高8位，SOUR 使用采集器编号    屏蔽模式接收配置后修改为标识符列表模式
*/

#include "common.h"
#include "type_com.h"

//命令码宏定义

//温度采样值命令
#define SLV_SAMP_TEMP_CMD 0X01
//弧光采样值命令
#define SLV_SAMP_ARC_CMD 0X02
//跳闸命令
#define SLV_SEND_TRIP_CMD 0X03
//事件命令
#define SLV_SEND_EVENT_CMD 0X04
//逻辑命令
#define SLV_LOGIC_SET_CMD 0X05
//定值命令
#define SLV_SETTING_SET_CMD 0X06
//参数设置命令
#define SLV_ZERO_SET_CMD 0X07
//管理器设置命令
#define SLV_MASTER_SET_CMD 0X08

/////////////////////////////////////////////////////////////////////////////
//所有报文的正确回复将原命令码或上UNIT_RESP_CMD_MASK
//所有报文的错误回复将原命令码或上UNIT_ERR_CMD_MASK
/////////////////////////////////////////////////////////////////////////////
#define SLV_RESP_CMD_MASK 0X80
#define SLV_ERR_CMD_MASK 0X40
#define SLV_CMD_MASK 0x1F



#define ARC_RADIO_MAX (1000) // 0.1 * 10000
#define TEMP_SETTING_MIN (TEMP_OVER_SETTING_MIN) //放大10倍
#define ARC_SETTING_MIN (ARC_OVER_SETTING_MIN) //放大10倍

#define LOGIC_ARC_TEMPOR 0 //弧光 + 温度
#define LOGIC_TEMPOR 1 //单温度或关系
#define LOGIC_ARCONLY 2 // 单弧光
#define LOGIC_ARC_TEMP_INPUT 3 //温度+弧光+开入
#define LOGIC_TEMPAND 4//多温度与关系

#define TRIP_FAILE_TIMEOUT (20) //单位秒
#define TRIP_RESET_TIMEOUT (60) //单位秒
#define FIRE_RESET_TIMEOUT (60) //单位秒
#define TRIP_RESET_FLAG_TIMEOUT (60) //单位秒
#define FIRE_RESET_FLAG_TIMEOUT (60) //单位秒

#define FIRE_RELAY_DELAYCNT (2)	// 2s后继电器返回


/*
/////////////采集器工作流程///////////
//读FLASH,获取控制器master地址，若无（首次）等待控制器配置。
//若有，判断定值是否正确，正确，进入正常工作模式；错误，进去心跳模式，发送定值错误信息，等待从新配置。
/////////////配置工作流程///////////
//1.单元管理器发送工作参数设置 cmd = 0x07
//2.采集器回复版本  cmd = 0x87
//3.单元管理器发送逻辑 cmd = 0x05  连续3次 间隔1s 无回复到1
//4.采集器回复 cmd = 0x85
//5.单元管理器发送定值 cmd = 0x06  连续3次 间隔1s 无回复到1
//6.采集器回复 cmd = 0x86
//7.单元管理器正常工作==>定时发送采样数据
*/

#pragma pack(1)



//////////////////////////////////////////////////////////////////
////////////////////////事件发送/////////////////////////////
//////////////////////////////////////////////////////////////////
//记录事件类型
typedef union
{
	UINT8 uiAll;
	struct
		{
		/*bit  0*/ UINT8 uiEventType_Record:1; //无参数
		/*bit  1*/ UINT8 uiEventType_Over:1; //越限记录
		/*bit  2*/ UINT8 uiEventType_Input:1; //开入变位
		/*bit  3*/ UINT8 uiEventType_Trip:1; //动作记录
		/*bit  4*/ UINT8 uiEventType_Fire:1; //灭火记录
		/*bit  5*/ UINT8 uiEventType_Res:3; //
		/*bit  6*/ 
	}bit;
}unEventType;

//无参数类型data1数据 data2 =0
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiEvent_Record_FLASHERR:1; //FALSH 错误记录
		/*bit  1*/ UINT16 uiEvent_Record_ADCERR:1; //ADC错误记录
		/*bit  2*/ UINT16 uiEvent_Record_COMLINK:1; //通信连接
		/*bit  3*/ UINT16 uiEvent_Record_COMLOST:1; //通信断开
		/*bit  4*/ UINT16 uiEvent_Record_SETERR:1; //设置错误
		/*bit  5*/ UINT16 uiEvent_Record_SENDTRIPERR:1;//发送跳闸通信错误
		/*bit  6*/ UINT16 uiEvent_Record_SENDEVENTERR:1;//发送事件通信错误
		/*bit  7*/ UINT16 uiEvent_Record_TripFailed:1;//跳闸失败 没有检测到开入信号
		/*bit  7*/ UINT16 uiEvent_Record_Res:8; //
	}bit;
}unEventRecordData1;

//越限记录类型data1 = 通道 data2 = 具体值
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiEvent_Over_TEMP1:1; //温度通道1
		/*bit  1*/ UINT16 uiEvent_Over_TEMP2:1; //温度通道2
		/*bit  2*/ UINT16 uiEvent_Over_TEMP3:1; //温度通道3
		/*bit  3*/ UINT16 uiEvent_Over_ARC:1; //弧光
		/*bit  4*/ UINT16 uiEvent_Over_Res:12; //
	}bit;
}unEventOverData1;
//开入变位记录类型data1=开入值

//跳闸记录类型data1=温度通道 data2 = 温度值 data3 =弧光值

//灭火记录类型data1=温度通道 data2 = 温度值 data3 =弧光值

typedef union
{
	UINT16 Data1;
	unEventRecordData1 RecordData1;
	unEventOverData1 OverData1;
	UINT16 InputData1;
	UINT16 TempChData1;
}unEventData1;

typedef union
{
	UINT16 Data2;
	UINT16 TempData2;
	UINT16 ArcData2;
}unEventData2;

typedef union
{
	UINT16 Data3;
	UINT16 ArcData3;
}unEventData3;

/////////////////////////////////////////////////////////////////////////////////////




//上送温度
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	UINT8   Version; // 高4位，低4位
	SINT16  SampTemp[3]; // 温度；  数据放大10倍，按照通道顺序存放
}stUserSlvSampTemp_Def;

//上送弧光
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	UINT8   CfgVersion; // 高4位，低4位 单元管理器判定版本不一致 (CONFIG + SETTING)
	unUnitStatus_Def SlvStatus; // 状态
	UINT16  SampArc;	     // 弧光；   数据放大10倍
	UINT8   SampIO;       // 采集控制器的IO状态；按位表示低4位开出，高4位开入
	
}stUserSlvSampArc_Def;

//回复时间
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	UINT32  Time;  // 回复时间，实现对时； 
	UINT8   ConfigVersion;// 单元管理器回复 配置版本 
	UINT8   SettingVersion;//单元管理器回复越限版本  
}stUserSlvSampResp_Def;

//发送动作
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	UINT16 TripUnitAddr;  // 跳闸控制器地址
	UINT16 TripBO;      // 跳闸单元IO BIT 
}stUserSlvTripReq_Def;

//动作回复
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
}stUserSlvTripResp_Def;

//事件
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	unEventType EventType;//数据内容解析方式
	unEventData1 Data1;
	unEventData2 Data2;
	unEventData3 Data3;
}stUserSlvEventReq_Def;

//事件回复
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
}stUserSlvEventResp_Def;



//逻辑配置
typedef struct
{
	UINT8   UserSlvCmd; // 命令码 05
	UINT8   OutFireType;    // 灭火器出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   TripType;    // 跳闸出口逻辑； 0：温度+ 弧光  1：温度；2：弧光 3.温度+ 弧光 +烟雾开入
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //跳闸单元地址
	UINT16   TripBO; // 跳闸单元IO 1~16
}stUserSlvLogicReq_Def;
//定值配置
typedef struct
{
	UINT8 UserSlvCmd; // 命令码 06
	UINT8   InputBit; //开入位置
	UINT16  TempLimit;  // 温度越限定值；	放大10倍
	UINT16  ArcLimit;   // 弧光越限定值；         放大10倍
	UINT8   TempDelay; // 温度延时；单位S
	UINT8   ArcDelay; // 弧光延时；单位S

}stUserSlvSettingReq_Def;

//逻辑回复
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
}stUserSlvLogicResp_Def;

//参数设置
typedef struct
{
	UINT8 UserSlvCmd; // 命令码  07
	SINT8  TempRadio[3];//[3];  　// 温度参数补偿 放大10倍
	UINT16   ArcZero;       // 弧光零漂
	UINT16   ArcRadio;       // 弧光比例系数 放大10000倍 0.025 -> 250	
}stUserSlvZeroReq_Def;
//设置回复
typedef struct
{
	UINT8 UserSlvCmd; // 命令码

}stUserSlvZeroResp_Def;

typedef struct
{
	UINT8 UserSlvCmd; // 命令码  08
	UINT8  MasterAddr;// 单元管理器地址
	UINT8   AlrmTempPrec;// 温度 告警定值 是动作定值的百分之几
	UINT8   AlrmArcPrec;  // 温度 告警定值 是动作定值的百分之几
	UINT8   CfgVersion; // 高4位，低4位 定值版本  (CONFIG + SETTING)
}stUserSlvMasterIdReq_Def;
//设置回复
typedef struct
{
	UINT8 UserSlvCmd; // 命令码
	UINT8 SettingReq;//采集器发现CfgVersion不一致，回复请求定值
}stUserSlvMasterIdResp_Def;


#ifdef __cplusplus
}
#endif

#endif 
