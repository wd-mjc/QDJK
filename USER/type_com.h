#ifndef TYPE_COMMON_H_
#define TYPE_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
//适用于台区和单元管理器的通信
//通信方式LORA 
//文件升级功能适用于IR串口通信
//跳闸命令功能单元管理器和跳闸单元的LORA或串口
//台区网口配置命令使用于台区和服务器
/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
//    YL-800MN-100MW   应用数据帧格式
//    帧类    帧序 命令  负荷  目标 ACK 请 发送 路由  数据  用户 校验
//    型         号       类型  长度  地址  求      半径 方式  长度  数据
//    1byte      1byte     1byte     1byte      2byte    1byte     1byte    1byte    1byte     N byte   1byte
//    帧头                                      |  帧负荷                                                       | XOR
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
//台区上电工作流程

/////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

//命令码宏定义

//采样值命令
#define UNIT_SAMPLING_CMD 0X01
//跳闸命令
#define UNIT_SENDTRIP_CMD 0X02
//事件命令
#define UNIT_SENDEVENT_CMD 0X03
//程序升级命令
#define UNIT_UPDATA_CMD 0X04
//读越限值命令
#define UNIT_SETTING_READ_CMD 0X05
//写越限值命令
//#define UNIT_SETTING_WRITE_CMD 0X06
//读配置命令
#define UNIT_CONFIGUNIT_READ_CMD 0X07
//写配置命令
//#define UNIT_CONFIGUNIT_WRITE_CMD 0X08

//#define UNIT_CONFIGEVENT_READ_CMD 0X09
//#define UNIT_CONFIGEVENT_WRITE_CMD 0X0A

//#define UNIT_NETWORK_RED_CMD 0X0B
//#define UNIT_NETWORK_WRITE_CMD 0X0C

//写地址
#define UNIT_ADDR_WRITE_CMD 0X0D
//读地址
#define UNIT_ADDR_READ_CMD 0X0E
//读采集器配置
#define UNIT_SETTING_CFG_CMD 0X0F

//读调试信息
#define UNIT_READ_DBG_CMD 0X11

/////////////////////////////////////////////////////////////////////////////
//所有报文的正确回复将原命令码或上UNIT_RESP_CMD_MASK
//所有报文的错误回复将原命令码或上UNIT_ERR_CMD_MASK
/////////////////////////////////////////////////////////////////////////////
#define UNIT_RESP_CMD_MASK 0X80
#define UNIT_ERR_CMD_MASK 0X40
#define UNIT_CMD_MASK 0x1F

/////////////////////////////////////////////////////////////////////////////
//LORA模块发送应用数据长度定义为50字节
/////////////////////////////////////////////////////////////////////////////
#define USER_DATA_LEN_MAX (50)
#define LORA_DATA_LEN_MAX (USER_DATA_LEN_MAX +5)
#define LORA_FRAME_LEN_MAX (USER_DATA_LEN_MAX+16)
#define LORARX_FRAME_LEN_MAX (USER_DATA_LEN_MAX+29)//接收长度

//台区最多管理500个单元管理器
#define MASTER_UNIT_CNT_MAX (500)
//单元管理器最多管理50个采集单元
#define SAMPLING_UNIT_CNT_MAX (50)

//心跳时间间隔单位s
#define HEART_INTV_TIMESEC_MAX (10) 
//心跳超时秒数
#define HEART_OVER_TIMESEC_MAX (60) 
//温度延时单位s
#define TEMP_DELAY_TIMESEC_MAX (600) 
#define TEMP_DELAY_TIMESEC_MIN (10) 
//弧光延时单位s
#define ARC_DELAY_TIMESEC_MAX (240) 
#define ARC_DELAY_TIMESEC_MIN (10) 
//温度越限值
#define TEMP_OVER_SETTING_MIN (100) //值已放大10倍
#define TEMP_OVER_SETTING_DEF (100)
//弧光越限值
#define ARC_OVER_SETTING_MIN (50) //值已放大10倍
#define ARC_OVER_SETTING_DEF (150) //值已放大10倍

#define LORA_NET1ID_DEF (1)//台区网络编号
#define LORA_NET2ID_DEF (2)//跳闸网络编号
#define LORA_NET1FR_DEF (434)//台区频率
#define LORA_NET2FR_DEF (435)//跳闸频率


#pragma pack(1)

/*
typedef struct
{
	UINT8 SendFrameId;//0-不分帧 非零-分帧序号
	UINT8 SendDataLen;// 数据长度
	UINT8 SendData[6];
}stSendPacket_Def;
*/	
typedef struct
{
	UINT8 UserCmd; // 命令码
	UINT16 UserSoursAddr; // 源地址
	UINT8 DataLen;//数据长度
	UINT8 UserData[USER_DATA_LEN_MAX + 2];// 数据长度 最大50字节  +2 方便程序
	UINT8 Crc8;//CRC8     从命令码UserCmd 到数据结束
}stUserPacket_Def;

typedef struct
{
	UINT8  LoraType;      // 帧类型： 05 表示发送用户数据
	UINT8  LoraSn;      // 固定0
	UINT8  LoraCmd; //01 发送请求
	UINT8  LoraLen;//负荷长度：是指从负荷长度后一个字节到校验前一个字节的个数
	UINT16 LoraDestAddr;//发送的目标地址 (大端格式)
	UINT8 LoraAck;//00 表示无需 ACK 应答、01 表示需要 ACK 应答
	UINT8 LoraRadius;//发送半径：07 表示最大 7 跳
	UINT8 LoraRoute;//路由方式：00 禁止路由、01 自动路由、02 强制路由
	UINT8 LoraDateLen;//数据长度：表示用户有 字节的数据需要发送	
	stUserPacket_Def UserPacket; // 包的内容
	UINT8 LoraXor;//效验：XOR 效验值 
	//此校验值为从帧头第一个字节（帧类型字节）开始到
	//校验字节之前所有字节异或运算的结果。检查时全帧所有字节异或运算结果为 0 即为校验
	//正确。
}stLoraStruct_Def;

typedef struct
{
	UINT8  LoraType;      // 帧类型： 05 表示发送用户数据
	UINT8  LoraSn;      // 固定0
	UINT8  LoraCmd; //0x82 接收
	UINT8  LoraLen;//负荷长度：是指从负荷长度后一个字节到校验前一个字节的个数
	UINT16 LoraDestAddr;//源地址(大端格式)
	UINT8 LoraRss;//信号强度
	//UINT8 LoraAck;//00 表示无需 ACK 应答、01 表示需要 ACK 应答
	//UINT8 LoraRadius;//发送半径：07 表示最大 7 跳
	//UINT8 LoraRoute;//路由方式：00 禁止路由、01 自动路由、02 强制路由
	UINT8 LoraDateLen;//数据长度：表示用户有 字节的数据需要发送	
	stUserPacket_Def UserPacket; // 包的内容
	UINT8 LoraRelayCnt; //中继数量
	UINT16 LoraRelay[7];//最大7个中继
	UINT8 LoraXor;//效验：XOR 效验值 
	//此校验值为从帧头第一个字节（帧类型字节）开始到
	//校验字节之前所有字节异或运算的结果。检查时全帧所有字节异或运算结果为 0 即为校验
	//正确。
}stLoraRxStruct_Def;

////////////////////////////////////////////////
//状态定义
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiSTATUS_FLASH_ERROR:1; //FALSH 错误记录
		/*bit  1*/ UINT16 uiSTATUS_PWR_ERROR:1; //电源电压异常
		/*bit  2*/UINT16 uiSTATUS_TEMP_ERROR:1;//温度传感器异常
		/*bit  3*/UINT16 uiSTATUS_ARC_ERROR:1;//弧光传感器异常
		/*bit  4*/ UINT16 uiSTATUS_LINKLOST_WARN:1; //连接断开
		/*bit  5*/ UINT16 uiSTATUS_SETTING_WARN:1; //定值出错
		/*bit  6*/ UINT16 uiSTATUS_CONFIG_WARN:1; //配置出错
		/*bit  7*/ UINT16 uiSTATUS_INPUT_WARN:1; //(开入变位)
		/*bit  8*/ UINT16 uiSTATUS_TEMPOVER_WARN:1;//温度越限
		/*bit  9*/ UINT16 uiSTATUS_ARCOVER_WARN:1;//弧光越限
		/*bit  10*/ UINT16 uiSTATUS_TRIP_WARN:1; //单元跳闸
		/*bit  11*/ UINT16 uiSTATUS_FIRE_WARN:1; //单元灭火
		/*bit  12*/ UINT16 uiSTATUS_SENDTRIP_ERROR:1;//发送跳闸通信错误	
		/*bit  13*/ UINT16 uiSTATUS_TRIPFAILED_WARN:1;//跳闸失败 没有检测到开入信号
		/*bit  14*/ UINT16 uiSTATUS_Res:2;
		
		/*bit  12*/ // UINT16 uiSTATUS_SETMODIFY_WARN:1;//设置修改告警(台区记录)
		/*bit  13*/ // UINT16 uiSTATUS_NETWORK_WARN:1;//网络(包括4G)异常(台区记录)		
	}bit;
}unUnitStatus_Def;


////////////////////////////////////////////////
//采样数据
//也是心跳报文
//单元管理器发送到台区 
typedef struct 
{
	UINT8   UnitType;  //1：台区管理器   2：单元管理器  3：采集控制器  4：跳闸控制器
	UINT16  UnitID;       // 装置ID
	unUnitStatus_Def  Status;    // 装置自检状态    0x0：工作正常                     
	SINT16  Temp1;//温度1  ; 数据放大10倍                
	SINT16  Temp2 ;//温度2; 数据放大10倍
	SINT16  Temp3 ;//温度3 ;数据放大10倍
	UINT16  ArcOrDO;     // 弧光;   数据放大10倍 ; 当为跳闸单元时，该字段表示输出状态 按位解析
	UINT8   IO;      // 采集控制器的IO状态；按位表示低4位开出，高4位开入
	UINT8   Version; // 高4位，低4位
}stUnit_Def;
//请求应用数据：
typedef struct  
{
	UINT8  UnitCnt;        // 以下stUnit的个数 最多3个
	stUnit_Def  pUnit;         // stUnit_Def 的个数,
}stSampleReq_Def;
typedef struct  
{
	UINT32  Time;// 回复时间，实现对时；  
	UINT8   ConfigVersion;// 台区回复 配置版本 //单元管理器发现不同读取配置数据
	UINT8   SettingVersion;//台区回复越限版本  //单元管理器发现不同读取越限数据
}stSampleResp_Def;


////////////////////////////////////////////////
//动作命令
typedef struct  
{
	UINT16   MasterID;         // 单元管理器ID，本小区全局唯一 0xHH00
	UINT16   SampleID;       // 采集控制器ID，本小区全局唯一0x00HH
	UINT16   TripUnitID;  // 跳闸控制器ID， 本小区全局唯一
	//UINT8    TripDO;      // 跳闸单元上的出口编号0 ~15
	UINT16   TripDOBit;   // 跳闸单元上的出口 按位解析BIT0对应DO1
}stActiveReq_Def;

//回复空


////////////////////////////////////////////////
//事件数据
typedef struct  
{
	//UINT8  WarnLevel; // 告警级别   1-低2-中3-高  (在台区)
	UINT32  Time;// 事件发生的事件，精确到秒，统一使用Unicode时间；
	UINT8   UnitType;  //1：台区管理器   2：单元管理器  3：采集控制器  4：跳闸控制器
	UINT16  ADDR;   //设备地址
	unUnitStatus_Def   EventType;//数据内容解析方式   (只解析越限和动作，其他事件通过心跳在台区记录)
	SINT16  Temp1;//温度1   数据放大10倍
	SINT16  Temp2 ;//温度2 数据放大10倍
	SINT16  Temp3 ;//温度3 数据放大10倍
	UINT16  Arc1;// 弧光 数据放大10倍
	UINT8  SampIO;//采集器IO 低4位开出，高4位开入
	UINT16  TripUnitID; // 跳闸控制器ID
	//UINT8  TripDO;//跳闸单元上的出口编号
	UINT16   TripDOBit;   // 跳闸单元上的出口 按位解析BIT0对应DO1
}stEventReq_Def;
//回复空


////////////////////////////////////////////////
//在线升级
//说明 单元管理器收到升级命令后停止所有工作
//            当传输完成后单元管理器再做数据校验判断 正确后再继续
//		  传输过程中帧间间隔时间超过5s，单元管理器判定升级失败进入原有程序正常工作
//FileSize = 程序文件实际大小+全文的数据校验码CRC16的2字节，
//在最后一帧文件内容的末尾+全文的数据校验码值。
typedef struct  
{
	UINT8   UnitType;  //1：台区管理器   2：单元管理器  3：采集控制器  4：跳闸控制器
	UINT32  FileSize; // 文件的总长度；  
	UINT16  CntMax;//总共分CNT 次发送
	UINT16  CntIndex;// 本次报文位于文件的地几个索引位置
	UINT8   Len; // 本次报文的长度；
	UINT8   *Data;  //  文件内容
}stUpdataFileReq_Def;

//回复空


////////////////////////////////////////////////
//越限配置数据 读取
typedef struct  
{
	UINT8  SettingVersion;//高4位 低4位  //台区上电先读取该位 与 当前使用版本对比 
										//如果不相同再写入配置数据
	UINT8   TripType;  //跳闸逻辑； 0：温度+ 弧光  1：温度 2：弧光 3.弧光+温度+ 开入 4. 温度与
	UINT8   FireType;    // 灭火器出口逻辑； 0：温度+ 弧光  1：温度 2：弧光 3.弧光+温度+ 开入 4. 温度与
	SINT16  TempLimit;  // 温度越限定值；动作定值 
	UINT16  ArcLimit;   // 弧光越限定值；  动作定值
	UINT8   TempDelay; // 温度延时；单位S
	UINT8   ArcDelay; // 弧光延时；单位S
	UINT8   AlrmTempPrec;// 温度 告警定值 是动作定值的百分之几
	UINT8   AlrmArcPrec;  // 温度 告警定值 是动作定值的百分之几
}stLimitReq_Def;
//单元管理器读取 数据空
//台区回复stLimitReq_Def

/*
////////////////////////////////////////////////
//事件配置数据 (写入 读取)
//暂时不使用
typedef struct  
{
	unUnitStatus_Def  OperTypeMask; // 事件类型  掩码 
	//UINT8  WarnLevel;// 告警级别 1-低2-中3-高 (在台区)
	UINT16  EnableMask;  // 主动上送功能掩码 1- 暂停：0-恢复
}stEventSet_Def;
//回复空
*/

/*
////////////////////////////////////////////////
//台区网口配置数据(写入 读取)
typedef struct  
{
	UINT8  Mac[6];   // mac地址
	UINT8  ip[4];  // IP地址；  
	UINT8  mask[4];   // 掩码地址；
	UINT8  gate[4];    // 网关地址；
	UINT8  DNS[4];    // DNS地址；
	
	UINT8  ip1[4]; // web服务器 IP地址；  
	UINT8  mask1[4]; // web服务器掩码地址；
	UINT8  port1;    // web服务器端口号；
	UINT8  ip2[4];  // 程序服务器IP地址2；  
	UINT8  mask2[4];   // 程序服务器掩码地址2；
	UINT8  port2;    // 程序服务器端口号2；
}stNet_Def;
//回复空
*/

/*
////////////////////////////////////////////////
//台区云端配置数据(写入 读取)
typedef struct  
{
	UINT8  ip1[4]; // web服务器 IP地址；  
	UINT8  mask1[4]; // web服务器掩码地址；
	UINT8  port1;    // web服务器端口号；
	UINT8  ip2[4];  // 程序服务器IP地址2；  
	UINT8  mask2[4];   // 程序服务器掩码地址2；
	UINT8  port2;    // 程序服务器端口号2；
}stCloud_Def;
//回复空
*/

////////////////////////////////////////////////
//采集器配置数据读取
typedef struct  
{
	//UINT16   MasterID;   // 单元管理器ID，本小区全局唯一
	UINT16   SampleID;   // 采集控制器ID，本小区全局唯一
	UINT16   TripUnitID;  // 跳闸控制器ID， 本小区全局唯一
	//UINT8     TripDO;       // 跳闸单元上的出口编号，
	UINT16   TripDOBit;   // 跳闸单元上的出口 按位解析BIT0对应DO1
        SINT8    TempRadio[3];//[3]; 　// 温度参数补偿 放大10倍
	UINT16   ArcZero;       // 弧光零漂，
	UINT16   ArcRadio;       // 弧光比例系数 放大10000倍 0.025 -> 250	
}stUnitConfig_Def;
typedef struct  
{
	UINT16 ConsoleID;//台区地址 
	UINT16   MasterID;   // 单元管理器ID，本小区全局唯一
	UINT8  ConfigVersion;//高4位 低4位  //台区上电先读取该位 与 当前使用版本对比 
										//如果不相同再写入配置数据
	UINT8  UnitConfigMax;// 该单元管理器 总共采集单元个数  最大SAMPLING_UNIT_CNT_MAX
	UINT8  UnitStartNo;        // 本次开始序号 ( 序号从0到UnitConfigMax-1)
	UINT8  UnitCnt;        // 本次单元个数 ,每次最大3个
	stUnitConfig_Def  *pUint;         // stUnit 的个数,
}stUnitConfigReq_Def;

//单元管理器读取台区
//台区回复stUnitConfigReq_Def
typedef struct  
{
	UINT8  UnitStartNo;        // 本次开始序号 ( 序号从0到UnitConfigMax-1)
	UINT8  UnitCnt;        // 本次单元个数 ,每次最大3个
}stUnitConfigRead_Def;


/*
////////////////////////////////////////////////
//采集控制器的调试命令（写入、查看）
typedef struct  
{
	UINT16   MasterID;        // 单元管理器ID，本小区全局唯一
	UINT16  SampleID;      // 采集控制器ID，本小区全局唯一
	SINT8  TempRadio;  　// 温度参数补偿 放大10倍
	UINT16   ArcZero;       // 弧光零漂，
	UINT16   ArcRadio;       // 弧光比例系数 放大10000倍 0.025 -> 250	
}stUnitZeroReq_Def;
//回复空
*/

////////////////////////////////////////////////
//红外点对点通信
//起始|保留|命令|长度|数据|校验|结束
//0xCA      0x00      0x0D       0xLL      DATA    CRC8     0xCF

#define REDNOTE_FRAM_LENMAX (76)
#define REDNOTE_DATA_LENMAX (70)
#define REDNOTE_FRAM_LENDEF (6)
#define REDNOTE_FRAM_START (0xCA)
#define REDNOTE_FRAM_END (0xCF)
#define DEBUG_BUF_LEN (50)
typedef struct  
{
	UINT8 StartFlg;        // 起始0xCA
	UINT8 Type;        // 0x00-上行(设备->主机) 0x01 -下行(主机->设备)
	UINT8 Cmd;         //命令码
	UINT8 Len;  //数据长度
	UINT8 Data[REDNOTE_DATA_LENMAX+2];//数据
	UINT8 Crc8; //从TYPE开始到数据结束
	UINT8 EndFlg;//结束 0xCF
}stRedNote_Def;


//红外读写地址
typedef struct  
{
	UINT16  ConsoleID;    //台区地址     格式 0x00MM (高8位固定0 MM = 1~254)
	UINT16  MasterID;    // 单元管理器ID       格式 0xMM00 (低8位固定0 MM = 1~254)
	UINT16  UnitID;        // 采集器ID 格式 0xYYMM (YY=单元管理器高8位MM = 1~254)
					    // 跳闸单元ID 格式 0xFFMM(高8位固定FF ,MM = 1~254)
	UINT8    NET1ID;     //台区LORA 通信网络ID
	UINT8    NET2ID;	   //跳闸LORA 通信网络ID			
	UINT16 NET1FR;     //台区LORA 通信网络频率
	UINT16 NET2FR;     //跳闸LORA 通信网络频率
	UINT16   ArcZero;       // 弧光零漂，
	UINT16   ArcRadio;       // 弧光比例系数 放大10000倍 0.025 -> 250	
	SINT8    TempRadio[3];//[3]; 　// 温度参数补偿 放大10倍
	
}stUnitSetNetAddr_Def;
//回复空

//发送命令码UNIT_SETTING_CFG_CMD
//回复stUnitSettingCfg_Def
//红外读采集器配置
typedef struct  
{
	UINT16  UnitID;        // 采集器ID 格式 0xYYMM (YY=单元管理器高8位MM = 1~254)
					    // 跳闸单元ID 格式 0xFFMM(高8位固定FF ,MM = 1~254
		
	UINT8   TripType;  //跳闸逻辑； 0：温度+ 弧光  1：温度 2：弧光 3.弧光+温度+ 开入 4. 温度与
	UINT8   FireType;    // 灭火器出口逻辑； 0：温度+ 弧光  1：温度 2：弧光 3.弧光+温度+ 开入 4. 温度与
	SINT16  TempLimit;  // 温度越限定值；动作定值 
	UINT16  ArcLimit;   // 弧光越限定值；  动作定值
	UINT8   TempDelay; // 温度延时；单位S
	UINT8   ArcDelay; // 弧光延时；单位S
	UINT8   AlrmTempPrec;// 温度 告警定值 是动作定值的百分之几
	UINT8   AlrmArcPrec;  // 温度 告警定值 是动作定值的百分之几

	UINT16   TripUnitID;  // 跳闸控制器ID， 本小区全局唯一
	UINT16   TripDOBit;   // 跳闸单元上的出口 按位解析BIT0对应DO1	
}stUnitSettingCfg_Def;

//发送命令码UNIT_READ_DBG_CMD
//回复stUnitReadDbg_Def
//红外读调试信息
typedef struct  
{
	UINT16  UnitID;        // 采集器ID 格式 0xYYMM (YY=单元管理器高8位MM = 1~254)
					    // 跳闸单元ID 格式 0xFFMM(高8位固定FF ,MM = 1~254
		
	UINT8   UnitType;  //1：台区管理器   2：单元管理器  3：采集控制器  4：跳闸控制器
	unUnitStatus_Def  Status;    // 装置自检状态    0x0：工作正常                     
	SINT16  Temp1;//温度1  ; 数据放大10倍                
	SINT16  Temp2 ;//温度2; 数据放大10倍
	SINT16  Temp3 ;//温度3 ;数据放大10倍
	UINT16  ArcOrDO;     // 弧光;   数据放大10倍 ; 当为跳闸单元时，该字段表示输出状态 按位解析
	UINT8   IO;      // 采集控制器的IO状态；按位表示低4位开出，高4位开入
	UINT8   Version; // 高4位，低4位          
	char buf[DEBUG_BUF_LEN];//打印信息
}stUnitReadDbg_Def;

////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /*COMMTYPE_H_*/

