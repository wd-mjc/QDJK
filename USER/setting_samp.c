/*
@file     setting_samp.c
@brief    配置及逻辑管理(采集单元)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#include<string.h>
//#include "setting.h"
#include "type_slv.h"
#include "setting_samp.h"

#include "cand.h"
#include "ff.h"
#include "pt100.h"
#include "adc.h"
#include "board_io.h"


stSysInfo_Def stSysInfo;

stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];
stSDebug_Def stSDebug;

#ifdef BOARD_SAMP

//设置地址
void WriteUnitAddr(stUnitSetNetAddr_Def *addr)
{
     //MJC DEBUG
	stSysInfo.stSetting.MasterAddr = addr->UnitID>>8;
	stSysInfo.stSetting.SampAddr = addr->UnitID&0x00ff;

	stSysInfo.stSetting.ArcZero = addr->ArcZero ;
	stSysInfo.stSetting.ArcRadio = addr->ArcRadio ;
	stSysInfo.stSetting.TempRadio[0] = addr->TempRadio[0] ;
	stSysInfo.stSetting.TempRadio[1] = addr->TempRadio[1] ;
	stSysInfo.stSetting.TempRadio[2] = addr->TempRadio[2] ;

	stSysInfo.stSysVar.SaveFlash =1;
	stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN =0;
	stSysInfo.stSetting.CfgVersion =0;//重新获取配置
	//修改CAN配置
	//初始化CAN
	CAN_Mask_Init(2,stSysInfo.stSetting.MasterAddr,stSysInfo.stSetting.SampAddr);
    
}
//读地址
void ReadUnitAddr(stUnitSetNetAddr_Def *addr)
{
	addr->ConsoleID =0;
	addr->MasterID = stSysInfo.stSetting.MasterAddr<<8;
	addr->UnitID = addr->MasterID |stSysInfo.stSetting.SampAddr;
	addr->NET1ID = 0;
	addr->NET2ID =0;
	addr->NET1FR =0;
	addr->NET2FR =0;

	addr->ArcZero =stSysInfo.stSetting.ArcZero;
	addr->ArcRadio =stSysInfo.stSetting.ArcRadio;
	addr->TempRadio[0] =stSysInfo.stSetting.TempRadio[0];
	addr->TempRadio[1] =stSysInfo.stSetting.TempRadio[1];
	addr->TempRadio[2] =stSysInfo.stSetting.TempRadio[2];
}

//读配置
void ReadSettingCfg(stUnitSettingCfg_Def *SettingCfg)
{

	SettingCfg->UnitID = (stSysInfo.stSetting.MasterAddr<<8) |stSysInfo.stSetting.SampAddr;

	SettingCfg->TripType = stSysInfo.stSetting.TripType;
	SettingCfg->FireType = stSysInfo.stSetting.OutFireType;
	SettingCfg->TempLimit = stSysInfo.stSetting.TempLimit;
	SettingCfg->ArcLimit = stSysInfo.stSetting.ArcLimit;
	SettingCfg->TempDelay = stSysInfo.stSetting.TempDelay;
	SettingCfg->ArcDelay = stSysInfo.stSetting.ArcDelay;
	SettingCfg->AlrmTempPrec = stSysInfo.stSetting.AlrmTempPrec;
	SettingCfg->AlrmArcPrec = stSysInfo.stSetting.AlrmArcPrec;
		
	SettingCfg->TripUnitID = stSysInfo.stSetting.TripAddr;
	SettingCfg->TripDOBit = stSysInfo.stSetting.TripBO;
}

//读DBG
void ReadUnitDbg(stUnitReadDbg_Def *pDbg)
{
	if(pDbg ==NULL)
		return;

	pDbg->UnitID = (stSysInfo.stSetting.MasterAddr<<8) |stSysInfo.stSetting.SampAddr;

	pDbg->UnitType = 3;
	pDbg->Status.uiAll = stSysInfo.stSysVar.SysStatus.uiAll;
	pDbg->Temp1 = stSysInfo.stSysVar.SampTemp[0];
	pDbg->Temp2 = stSysInfo.stSysVar.SampTemp[1];
	pDbg->Temp3 = stSysInfo.stSysVar.SampTemp[2];
	pDbg->ArcOrDO = stSysInfo.stSysVar.SampArc;
	pDbg->IO = stSysInfo.stSysVar.SampIO;
	pDbg->Version = stSysInfo.stSysVar.Version;

	if(stSDebug.Len + 12 < DEBUG_BUF_LEN)
		sprintf(&stSDebug.Buf[stSDebug.Len],"CfgVer:%d\r\n",stSysInfo.stSetting.CfgVersion);
	
	strncpy(pDbg->buf,stSDebug.Buf,DEBUG_BUF_LEN);
}

//添加事件到缓冲区
void AddEvent(stUserSlvEventReq_Def *SlvEvent)
{
	UINT8 ui;
	for(ui=0;ui<EVENT_BUF_CNT;ui++)
	{
		if(stSlvEventBuf[ui].EventStatus ==0)
		{
			memcpy((UINT8 *)&stSlvEventBuf[ui].stSlvEvent,(UINT8 *)SlvEvent,sizeof(stUserSlvEventReq_Def));
			stSlvEventBuf[ui].SendCnt =0;//再次清零
			//memset((UINT8*)&stSlvEventBuf[ui].stSlvEvent,0,sizeof(stUserSlvEventReq_Def));
			stSlvEventBuf[ui].EventStatus =1;//值标志
			break;
		}
	}	
}

//心跳事件 发送采样值
//每 1  S 调用
UINT8 Heart_Event_Flag =0;
void SendSampling(void)
{
	if(stSysInfo.stSysVar.ReadSettingFlag)
		return;
	if(stSysInfo.stSysVar.SysWorkMod !=3 )
		return;
	if(Heart_Event_Flag)
	{
		SlvSampTempReq();
	}	
	else
	{
		if(stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN | stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN)
		{
			SlvSampTempReq();
			delay_ms(2);			
		}
		SlvSampArcReq();
	}	
	Heart_Event_Flag ^= 1;

	if(stSysInfo.stSysVar.HeartCnt <= 6)
		stSysInfo.stSysVar.HeartCnt ++;
	if(stSysInfo.stSysVar.HeartCnt >= 6)
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
 //               stSysInfo.stSysVar.HeartCnt = 0;
	}
}

//检查重发
//每250ms调用
void CheckResend(void)
{
	UINT8 ui;
	if(stSysInfo.stSysVar.TripEvent)//检查发送跳闸
	{
		SlvTripReq();
	}
	for(ui=0;ui<EVENT_BUF_CNT;ui++)//检查发送事件
	{
		if(stSlvEventBuf[ui].EventStatus)
		{
			//SlvEventReq((stSysEven_Def*)&stSlvEventBuf[ui]);
			//break;//每次只发送1次事件
		}		
	}
}

void AddTripEvent(UINT8 CH)
{
	stUserSlvEventReq_Def SlvEvent;

	if(CH>2)
		CH =0;
	
	SlvEvent.EventType.bit.uiEventType_Trip = 1;
	SlvEvent.Data1.TempChData1 = CH;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.SampTemp[CH] ;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.SampArc;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddOldTripEvent(void)
{
	stUserSlvEventReq_Def SlvEvent;

	SlvEvent.EventType.bit.uiEventType_Trip = 1;
	SlvEvent.Data1.TempChData1 = stSysInfo.stSysVar.TripTempCh ;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.TripTempVal;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.TripArcVal;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddTripFailedEvent(void)
{
	stUserSlvEventReq_Def SlvEvent;

	SlvEvent.EventType.bit.uiEventType_Record = 1;
	SlvEvent.Data1.RecordData1.bit.uiEvent_Record_TripFailed = 1;
	SlvEvent.Data2.TempData2 = 0;
	SlvEvent.Data3.ArcData3= 0;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddFireEvent(UINT8 CH)
{
	stUserSlvEventReq_Def SlvEvent;

	if(CH>2)
		CH =0;
	
	SlvEvent.EventType.bit.uiEventType_Fire = 1;
	SlvEvent.Data1.TempChData1 = CH;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.SampTemp[CH] ;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.SampArc;
	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}


//定时器250ms 调用1 次
//逻辑功能
void LogicCheck(void)
{
	UINT8 ArcOver = 0;
	UINT8 TempOver =0;
	UINT8 InputOver =0;
	UINT8 TripRes =0;
	UINT8 FireRes =0;
	UINT8 TempCh =0;

	//弧光判断
	if(stSysInfo.stSysVar.SampArc > stSysInfo.stSetting.ArcLimit)
	{
		stSysInfo.stSysVar.TripArcTimeCnt ++;
		if(stSysInfo.stSysVar.TripArcTimeCnt > stSysInfo.stSetting.ArcDelay*4)
		{
			ArcOver = 1;
			stSysInfo.stSysVar.TripArcTimeCnt = stSysInfo.stSetting.ArcDelay*4;
		}
	}
	else
	{
		stSysInfo.stSysVar.TripArcTimeCnt =0;
	}

	//温度判断
	if(stSysInfo.stSetting.TripType == LOGIC_TEMPAND)//温度与
	{
		if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
		&&(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
		&&(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
		{
			stSysInfo.stSysVar.TripTempTimeCnt ++;
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*4)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*4;
				TempCh =0;
			}
		}
		else
		{
			stSysInfo.stSysVar.TripTempTimeCnt =0;
		}
	}
	else//温度或
	{
		if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
		||(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
		||(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
		{
			stSysInfo.stSysVar.TripTempTimeCnt ++;
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*4)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*4;
				if(stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
					TempCh =0;
				else if(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
					TempCh =1;
				else
					TempCh =2;
					
			}
		}
		else
		{
			stSysInfo.stSysVar.TripTempTimeCnt =0;
		}
	}
	//开入判断
	if(stSysInfo.stSetting.InputBit & stSysInfo.stSysVar.SampInput)
	{
		InputOver = 1;
	}

	//跳闸逻辑
	switch( stSysInfo.stSetting.TripType)
	{
		case LOGIC_ARC_TEMPOR : //弧光 + 温度                
			if((ArcOver)&&(TempOver))
				TripRes = 1;				
		break; 
		case LOGIC_TEMPOR : //单温度 或关系
			if(TempOver)
				TripRes = 1;	
		break; 
		case LOGIC_ARCONLY : // 单弧光     
			if(ArcOver)
				TripRes = 1;	
		break; 
		case LOGIC_ARC_TEMP_INPUT : //温度+弧光+开入
			if((ArcOver)&&(TempOver)&& (InputOver))
				TripRes = 1;
		break; 
		case LOGIC_TEMPAND : //多温度 与关系
			if(TempOver)
				TripRes = 1;	
		break; 
		default :  break;
	}

	// 灭火器逻辑
	switch( stSysInfo.stSetting.OutFireType)
	{
		case LOGIC_ARC_TEMPOR : //弧光 + 温度                
			if((ArcOver)&&(TempOver))
				FireRes = 1;				
		break; 
		case LOGIC_TEMPOR : //单温度 或关系
			if(TempOver)
				FireRes = 1;	
		break; 
		case LOGIC_ARCONLY : // 单弧光     
			if(ArcOver)
				FireRes = 1;	
		break; 
		case LOGIC_ARC_TEMP_INPUT : //温度+弧光+开入
			if((ArcOver)&&(TempOver)&& (InputOver))
				FireRes = 1;
		break; 
		case LOGIC_TEMPAND : //多温度 与关系
			if(TempOver)
				FireRes = 1;	
		break; 
		default :  break;
	}

	if((TripRes)&&(stSysInfo.stSysVar.TripFlag == 0))//发送跳闸
	{
		stSysInfo.stSysVar.TripFlag = 1;
		SlvTripReq();		
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 1; //跳闸状态
		//AddTripEvent(TempCh);//等待开入监测电源失去 上送报告
		stSysInfo.stSysVar.TripTempCh = TempCh;
		stSysInfo.stSysVar.TripTempVal = stSysInfo.stSysVar.SampTemp[TempCh] ;
		stSysInfo.stSysVar.TripArcVal = 	stSysInfo.stSysVar.SampArc;
		stSysInfo.stSysVar.TripFaileTimeCnt =0;//
	}
	if((FireRes)&&(stSysInfo.stSysVar.FireFlag == 0))//开启灭火器
	{
		stSysInfo.stSysVar.FireFlag = 1;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FIRE_WARN = 1;//灭火启动状态
		//	add 灭火继电器动作函数
		DO1_ON();
		DO2_ON();

		stSysInfo.stSysVar.FireDelay = FIRE_RELAY_DELAYCNT;
		stSysInfo.stSysVar.FireStatus = 0x03;
		AddFireEvent(TempCh);
	}	
}
	

//定时器1s 调用1 次
//越限功能
void OverLimitCheck(void)
{
	UINT8 ArcOver = 0;
	UINT8 TempOver =0;
	UINT8 InputOver =0;
	UINT8 TempCh =0;

	UINT16 AlrmArcVal,AlrmTempVal;

	AlrmArcVal = stSysInfo.stSetting.ArcLimit * stSysInfo.stSetting.AlrmArcPrec /100;
	AlrmTempVal = stSysInfo.stSetting.TempLimit *  stSysInfo.stSetting.AlrmTempPrec /100;

	//弧光判断
	if(stSysInfo.stSysVar.SampArc > AlrmArcVal)
	{
		stSysInfo.stSysVar.AlmArcTimeCnt ++;
		if(stSysInfo.stSysVar.AlmArcTimeCnt > stSysInfo.stSetting.ArcDelay)
		{
			ArcOver = 1;
			stSysInfo.stSysVar.AlmArcTimeCnt = stSysInfo.stSetting.ArcDelay;
		}
	}
	else
	{
		stSysInfo.stSysVar.AlmArcTimeCnt =0;
	}

	//温度判断
	if((stSysInfo.stSysVar.SampTemp[0] > AlrmTempVal)
	||(stSysInfo.stSysVar.SampTemp[1] > AlrmTempVal)
	||(stSysInfo.stSysVar.SampTemp[2] > AlrmTempVal))
	{
		stSysInfo.stSysVar.AlmTempTimeCnt ++;
		if(stSysInfo.stSysVar.AlmTempTimeCnt > stSysInfo.stSetting.TempDelay)
		{
			TempOver = 1;
			stSysInfo.stSysVar.AlmTempTimeCnt = stSysInfo.stSetting.TempDelay;
			if(stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
				TempCh =0;
			else if(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
				TempCh =1;
			else
				TempCh =2;				
		}
	}
	else
	{
		stSysInfo.stSysVar.AlmTempTimeCnt =0;
	}
	
	//开入判断
	if( stSysInfo.stSysVar.SampInput)
	{
		InputOver = 1;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_INPUT_WARN = 1;//开入变位状态
		//stSysInfo.stSysVar.TripFaileTimeCnt =0;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_INPUT_WARN = 0;//开入变位状态
	}

	if(ArcOver)//弧光越限  管理单元记录
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN = 1;//弧光越限状态
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN = 0;//弧光越限状态
	}
	if(TempOver)//温度越限  管理单元记录
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN = 1;//弧光越限状态
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN = 0;//弧光越限状态
	}
	
	//发送跳闸记录
	if((InputOver ==1)&&(stSysInfo.stSysVar.TripFlag == 1))
	{
		
		stSysInfo.stSysVar.TripFaileTimeCnt++;
		if(stSysInfo.stSysVar.TripFaileTimeCnt == 1)//只记录一次
		{
			AddOldTripEvent();
		}
		if(stSysInfo.stSysVar.TripFaileTimeCnt == TRIP_FAILE_TIMEOUT)
		{
			stSysInfo.stSysVar.TripFaileTimeCnt = TRIP_FAILE_TIMEOUT;//只记录一次
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIPFAILED_WARN = 1;// 跳闸失败状态
			AddTripFailedEvent();//跳闸失败记录 没有开入				
		}
	}
	else	if((InputOver ==0)&&(stSysInfo.stSysVar.TripFlag))	//跳闸成功 
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIPFAILED_WARN = 0;
		stSysInfo.stSysVar.TripFaileTimeCnt = 0;
	}

	//t跳闸复归
	if(stSysInfo.stSysVar.TripFlag == 1)
	{
		stSysInfo.stSysVar.ResetTripTimeCnt ++;
		if((stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_RESET_TIMEOUT)
		&&(stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_FAILE_TIMEOUT))	
		{
			stSysInfo.stSysVar.ResetTripTimeCnt =0;
			stSysInfo.stSysVar.TripFaileTimeCnt = 0; //复归跳闸
			stSysInfo.stSysVar.TripFlag = 2;	
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 0; //跳闸状态
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIPFAILED_WARN = 0;// 跳闸失败状态
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR = 0;// 跳闸通信错误
		}
		
	}
        if(stSysInfo.stSysVar.TripFlag == 2)
		{
			stSysInfo.stSysVar.ResetTripFlagTimeCnt++;
			if(stSysInfo.stSysVar.ResetTripFlagTimeCnt >= TRIP_RESET_FLAG_TIMEOUT)
			{
				stSysInfo.stSysVar.ResetTripFlagTimeCnt = 0;
				stSysInfo.stSysVar.TripFlag = 0; //开启重新动作
			}
		}
	//t灭火复归
	if(stSysInfo.stSysVar.FireFlag == 1)
	{
		stSysInfo.stSysVar.ResetFireTimeCnt ++;
		if(stSysInfo.stSysVar.ResetFireTimeCnt>=FIRE_RESET_TIMEOUT)
		{
			stSysInfo.stSysVar.ResetFireTimeCnt =0;
			stSysInfo.stSysVar.FireFlag = 2;	
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FIRE_WARN = 0;//灭火启动状态
		}
		
		//继电器返回
		if(stSysInfo.stSysVar.FireDelay > 0)
		{
			stSysInfo.stSysVar.FireDelay --;
			if(stSysInfo.stSysVar.FireDelay == 0)
			{
				DO1_OFF();
				DO2_OFF();
				//释放继电器
				stSysInfo.stSysVar.FireStatus = 0;
			}
		}
	}
        if(stSysInfo.stSysVar.FireFlag == 2)
		{
			stSysInfo.stSysVar.ResetFireFlagTimeCnt++;
			if(stSysInfo.stSysVar.ResetFireFlagTimeCnt >= FIRE_RESET_FLAG_TIMEOUT)
			{
				stSysInfo.stSysVar.ResetFireFlagTimeCnt = 0;
				stSysInfo.stSysVar.FireFlag = 0; //开启重新动作
			}
		}
}



void FlashWrite(void)
{
	FATFS fs;
	FIL fsrc;
	FRESULT res; 
	UINT32 br, bw;         // 文件读/写字节计数 

	// 打开驱动器 0 上的源文件 
	res = f_open(&fsrc, "0:/srcfile.dat",  FA_OPEN_ALWAYS | FA_WRITE);// FA_OPEN_ALWAYS  
	if (res) 
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	br =  sizeof(stSetting_def);
	res = f_write(&fsrc, (UINT8*)&stSysInfo.stSetting,br, &bw);
	if (res || bw < br) 
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		f_close(&fsrc);
		return;
	}

	//关闭打开的文件
	f_close(&fsrc);
}

void FlashRead(void)
{
	FATFS fs;
	FIL fsrc;
	FRESULT res; 
	UINT32 br, bw;         // 文件读/写字节计数 

	// 打开驱动器 0 上的源文件 
	res = f_open(&fsrc, "0:/srcfile.dat",  FA_OPEN_ALWAYS | FA_READ);// FA_OPEN_ALWAYS  
	if (res) 
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	bw = sizeof(stSetting_def);
	res = f_read(&fsrc, (UINT8*)&stSysInfo.stSetting,bw , &br);  
	if (res || br < bw) 
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		f_close(&fsrc);
		return;
	}

	//关闭打开的文件
	f_close(&fsrc);

}

void SaveFlashClearVar(void)
{
	stSysInfo.stSysVar.SaveFlash =0;
	stSysInfo.stSysVar.ReadSettingCnt =0;
	stSysInfo.stSysVar.ReadSettingFlag =0;
	stSysInfo.stSysVar.ReadSettingTime =0;
	stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR = 0;
}

//检查写FLASH 任务
//每1s调用
void SCheckWriteFlash(void)
{
	if(stSysInfo.stSysVar.SaveFlash)
	{
		stSysInfo.stSysVar.SaveFlash = 0;

		SaveFlashClearVar();		

		//初始化comapi
		CanInit_Api(0,stSysInfo.stSetting.SampAddr,stSysInfo.stSetting.MasterAddr);
                
                //初始化CAN
                CAN_Mask_Init(2,stSysInfo.stSetting.MasterAddr,stSysInfo.stSetting.SampAddr);


		stSysInfo.stSetting.SysCheck1 = 0x5341;
		stSysInfo.stSetting.SysCheck2 = 0x5341;

		stSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stSysInfo.stSetting, sizeof(stSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();		
	}
}
#define SAMP_POINT_MAX (24)
typedef struct
{
	SINT16 Temp[3];
	UINT16 Arc;
}stSampVal_Def;
stSampVal_Def stSampVal[SAMP_POINT_MAX];
SINT32 TempCount[3];
SINT32 ArcCount;
SINT16 TempOld[3];
UINT16 ArcOld;
UINT8 SampVal_Index =0;


//每250ms调用
void GetSampVal(void)
{
	UINT16 ArcVal;
	SINT16 Temp[3];
	float fArc;
	float pwr[5];
	float pwrmin,pwrmax;
	UINT8 ui;

	TempOld[0] = stSampVal[SampVal_Index].Temp[0];
	TempOld[1] = stSampVal[SampVal_Index].Temp[1];
	TempOld[2] = stSampVal[SampVal_Index].Temp[2];
	ArcOld        = stSampVal[SampVal_Index].Arc;

	Temp[0] = ReadTempCh(1);
	Temp[1] = ReadTempCh(2);
	Temp[2]= ReadTempCh(3);
	ArcVal = GetArcAdcVal();//弧光
	get_pwr_val((float *)pwr);
	for(ui= 0; ui<3;ui++)	//温度错误判断
	{
		if((Temp[ui] >= S_TEMP_ERR_MAX )||(Temp[ui] <= S_TEMP_ERR_MIN))
		{
			Temp[ui] = 0;
			if(stSysInfo.stSysVar.TempErrCnt[ui] <= S_SENSOR_ERR_CNT)
				stSysInfo.stSysVar.TempErrCnt[ui] ++;
		}
		else
		{
			stSysInfo.stSysVar.TempErrCnt[ui] = 0;
		}
	}
	//弧光错误判断
	if(ArcVal <= S_ARC_ERR_MIN)
	{
		//ArcVal = 0;
		if(stSysInfo.stSysVar.ArcErrCnt <= S_SENSOR_ERR_CNT)
			stSysInfo.stSysVar.ArcErrCnt ++;
	}
	else
	{
		stSysInfo.stSysVar.ArcErrCnt = 0;
	}

	//pwr
	for(ui=0;ui<5;ui++)
	{
		switch( ui )
		{
		case 0 :         
			pwrmin = S_PWIN_ERR_MIN;
			pwrmax = S_PWIN_ERR_MAX;
		break; 
		case 1 :      
			pwrmin = S_PW50_ERR_MIN;
			pwrmax = S_PW50_ERR_MAX;
		break; 
		case 2 :      
			pwrmin = S_PW90_ERR_MIN;
			pwrmax = S_PW90_ERR_MAX;
		break; 
		case 3 :      
			pwrmin = S_PW240_ERR_MIN;
			pwrmax = S_PW240_ERR_MAX;
		break;
		case 4 :      
			pwrmin = S_PW33_ERR_MIN;
			pwrmax = S_PW33_ERR_MAX;
		break;	
		default :  break;
		}
		if((pwr[ui] > pwrmax )||(pwr[ui] < pwrmin))
		{
			if(stSysInfo.stSysVar.PwrErrCnt[ui] <= S_PWR_ERR_CNT)
				stSysInfo.stSysVar.PwrErrCnt[ui] ++;
		}
		else
		{
			stSysInfo.stSysVar.PwrErrCnt[ui] = 0;
		}
	}


	if((stSysInfo.stSysVar.TempErrCnt[0] >= S_SENSOR_ERR_CNT)//温度
		||(stSysInfo.stSysVar.TempErrCnt[1] >= S_SENSOR_ERR_CNT)
		||(stSysInfo.stSysVar.TempErrCnt[2] >= S_SENSOR_ERR_CNT))
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMP_ERROR =1;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMP_ERROR =0;
	}
	if(stSysInfo.stSysVar.ArcErrCnt >= S_SENSOR_ERR_CNT)//弧光
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARC_ERROR =1;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARC_ERROR =0;
	}
	if((stSysInfo.stSysVar.PwrErrCnt[0]>= S_PWR_ERR_CNT)//pwr
	||(stSysInfo.stSysVar.PwrErrCnt[1]>= S_PWR_ERR_CNT)
	||(stSysInfo.stSysVar.PwrErrCnt[2]>= S_PWR_ERR_CNT)
	||(stSysInfo.stSysVar.PwrErrCnt[3]>= S_PWR_ERR_CNT)
	||(stSysInfo.stSysVar.PwrErrCnt[4]>= S_PWR_ERR_CNT))
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =1;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =0;
	}
	
	

	stSampVal[SampVal_Index].Temp[0] = Temp[0]+stSysInfo.stSetting.TempRadio[0];
	stSampVal[SampVal_Index].Temp[1] = Temp[1]+stSysInfo.stSetting.TempRadio[1];
	stSampVal[SampVal_Index].Temp[2] = Temp[2]+stSysInfo.stSetting.TempRadio[2];
	
	stSampVal[SampVal_Index].Arc = (ArcVal < stSysInfo.stSetting.ArcZero)?0:(ArcVal - stSysInfo.stSetting.ArcZero);

	TempCount[0] +=  stSampVal[SampVal_Index].Temp[0] - TempOld[0];
	TempCount[1] +=  stSampVal[SampVal_Index].Temp[1] - TempOld[1];
	TempCount[2] +=  stSampVal[SampVal_Index].Temp[2] - TempOld[2];
	ArcCount        +=   stSampVal[SampVal_Index].Arc  - ArcOld;
	if(ArcCount < 0)
		ArcCount = 0;

	SampVal_Index ++;
	if(SampVal_Index >= SAMP_POINT_MAX)
		SampVal_Index = 0;

	//温度
	stSysInfo.stSysVar.SampTemp[0] = TempCount[0] / SAMP_POINT_MAX;
	stSysInfo.stSysVar.SampTemp[1] = TempCount[1] / SAMP_POINT_MAX;
	stSysInfo.stSysVar.SampTemp[2] = TempCount[2] / SAMP_POINT_MAX;
	//弧光
	fArc =  (ArcCount  / SAMP_POINT_MAX)  *  (stSysInfo.stSetting.ArcRadio /1000.0);  //10000.0;//放大10倍
	stSysInfo.stSysVar.SampArc = (UINT16)( fArc+ 0.5);
	
	stSysInfo.stSysVar.SampInput = GetDI(0);//开入
	stSysInfo.stSysVar.SampOutput = stSysInfo.stSysVar.FireStatus; //开出
	stSysInfo.stSysVar.SampIO = (stSysInfo.stSysVar.SampInput <<4)|(stSysInfo.stSysVar.SampOutput & 0x0f);
}

// 250ms 调用
void SampLedStatus(void)
{
	//led_config
	if(stSysInfo.stSysVar.SysStatus.uiAll & 0x07f)//告警
		led_config(LED_CH_ALAM, 1);
	else
		led_config(LED_CH_ALAM, 0);

	if(stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN|stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN)
		led_config(LED_CH_OVER, 1);
	else
		led_config(LED_CH_OVER, 0);
}

// 1s 调用
void CheckSettingFlag(void)
{
	if(stSysInfo.stSysVar.ReadSettingFlag)
	{
		stSysInfo.stSysVar.ReadSettingTime++;
		if(stSysInfo.stSysVar.ReadSettingTime > 60)
		{
			stSysInfo.stSysVar.ReadSettingTime=0;
			stSysInfo.stSysVar.ReadSettingFlag = 0;//重发
			stSysInfo.stSysVar.ReadSettingCnt ++;
			if(stSysInfo.stSysVar.ReadSettingCnt > 3)
			{
				stSysInfo.stSysVar.ReadSettingCnt = 3;// 读取后 ，保存flash 处清零
				stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;//3次读取后都未读到 
			}
		}
	}
}


//主函数调用
//初始化采集单元
void SysInitSampUnit(void)
{
	UINT8 u8crc = 0;
	//UINT8 ui;
	
	//设置变量为0 
	memset((UINT8 *)&stSysInfo.stSysVar,0,sizeof(stSysInfo.stSysVar));
	
	//读flash 获取配置stMSysInfo
	FlashRead();
        
        //读FLASH后必须加延时才能初始化CAN，否则CAN异常
        delay_ms(100);  

	if((stSysInfo.stSetting.SysCheck1 != 0x5341)||(stSysInfo.stSetting.SysCheck2 != 0x5341))
	{
		stSysInfo.stSetting.SysCheck1 = 0x5341;
		stSysInfo.stSetting.SysCheck2 = 0x5341;

		stSysInfo.stSetting.SampAddr = S_SAMP_ADDR;
		stSysInfo.stSetting.MasterAddr = S_MASTER_ADDR;

		stSysInfo.stSetting.AlrmTempPrec = S_ALRM_PREC;
		stSysInfo.stSetting.AlrmArcPrec = S_ALRM_PREC;

		stSysInfo.stSetting.CfgVersion = 0;

		stSysInfo.stSetting.TempRadio[0] =0;
		stSysInfo.stSetting.TempRadio[1] =0;
		stSysInfo.stSetting.TempRadio[2] =0;

		stSysInfo.stSetting.ArcZero =S_ARC_ZERO;
		stSysInfo.stSetting.ArcRadio =S_ARC_RADIO;
		stSysInfo.stSetting.InputBit =0;
		stSysInfo.stSetting.TempLimit = TEMP_OVER_SETTING_DEF;
		stSysInfo.stSetting.ArcLimit = ARC_OVER_SETTING_DEF;
		stSysInfo.stSetting.TempDelay = TEMP_DELAY_TIMESEC_MIN;
		stSysInfo.stSetting.ArcDelay = ARC_DELAY_TIMESEC_MIN;
		stSysInfo.stSetting.OutFireType =0;
		stSysInfo.stSetting.TripType =0;

		stSysInfo.stSetting.TripAddr = 0;
		stSysInfo.stSetting.TripBO = 0;

		stSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stSysInfo.stSetting, sizeof(stSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
	}
	else
	{
		u8crc = CRC8((UINT8 *)&stSysInfo.stSetting, sizeof(stSysInfo.stSetting)-1);
		if(u8crc != stSysInfo.stSetting.uiCrc8)
		{
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if((stSysInfo.stSetting.SampAddr == 0 )||(stSysInfo.stSetting.SampAddr == 0XFF))
		{
			stSysInfo.stSetting.SampAddr = S_SAMP_ADDR;
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if((stSysInfo.stSetting.MasterAddr == 0 )||(stSysInfo.stSetting.MasterAddr == 0XFF))
		{
			stSysInfo.stSetting.MasterAddr = S_SAMP_ADDR;
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if((stSysInfo.stSetting.TempLimit < TEMP_OVER_SETTING_MIN)||(stSysInfo.stSetting.ArcLimit < ARC_OVER_SETTING_MIN))
		{
			stSysInfo.stSetting.TempLimit  = TEMP_OVER_SETTING_DEF;
			stSysInfo.stSetting.ArcLimit = ARC_OVER_SETTING_DEF;
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}

		if((stSysInfo.stSetting.TempDelay < TEMP_DELAY_TIMESEC_MIN)||(stSysInfo.stSetting.ArcDelay < ARC_DELAY_TIMESEC_MIN))
		{
			stSysInfo.stSetting.TempDelay = TEMP_DELAY_TIMESEC_MIN;
			stSysInfo.stSetting.ArcDelay = ARC_DELAY_TIMESEC_MIN;
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}
		if(stSysInfo.stSetting.ArcRadio > S_ARC_RADIOMAX)
		{
			stSysInfo.stSetting.ArcZero =S_ARC_ZERO;
			stSysInfo.stSetting.ArcRadio =S_ARC_RADIO;
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}
	}

	stSysInfo.stSysVar.SysWorkMod = 3;
	stSysInfo.stSysVar.Version = SAMP_UINT_VERSION;

	//初始化CAN
	CAN_Mask_Init(2,stSysInfo.stSetting.MasterAddr,stSysInfo.stSetting.SampAddr);

	//初始化comapi
	CanInit_Api(0,stSysInfo.stSetting.SampAddr,stSysInfo.stSetting.MasterAddr);
}
UINT16 SampTimer600smCnt = 0;//10分钟
UINT8 SampTimer2sCnt = 0;
UINT8 SampTimer1sCnt = 0;
//主函数调用250ms
void SampTimerTask(void)
{
	SampTimer1sCnt ++;
	SampTimer2sCnt ++;
	if(SampTimer2sCnt >=8)//2s
	{
		SampTimer2sCnt=0;
		SCheckWriteFlash();//FLASH
		SendSampling();//心跳
	}
	if(SampTimer1sCnt >=4)//1s
	{
		SampTimer1sCnt=0;
		SampTimer600smCnt ++;
		if(SampTimer600smCnt >= 600)// 600s
		{
			SampTimer600smCnt = 0;
			adc_adjust();//校准adc
		}
		stSysInfo.stSysVar.SysTime ++;
		OverLimitCheck();//越限判断	
		CheckSettingFlag();//读取配置判断
	}

	//250ms
	CheckResend(); //重发
	LogicCheck();//逻辑判断
	GetSampVal();//采样
	SampLedStatus();//指示灯
}

void sResetSystem(void)
{
		stSysInfo.stSetting.SysCheck1 = 0x5341;
		stSysInfo.stSetting.SysCheck2 = 0x5341;

		stSysInfo.stSetting.SampAddr = S_SAMP_ADDR;
		stSysInfo.stSetting.MasterAddr = S_MASTER_ADDR;

		stSysInfo.stSetting.AlrmTempPrec = S_ALRM_PREC;
		stSysInfo.stSetting.AlrmArcPrec = S_ALRM_PREC;

		stSysInfo.stSetting.CfgVersion = 0;

		stSysInfo.stSetting.TempRadio[0] =0;
		stSysInfo.stSetting.TempRadio[1] =0;
		stSysInfo.stSetting.TempRadio[2] =0;

		stSysInfo.stSetting.ArcZero =S_ARC_ZERO;
		stSysInfo.stSetting.ArcRadio =S_ARC_RADIO;
		stSysInfo.stSetting.InputBit =0;
		stSysInfo.stSetting.TempLimit = TEMP_OVER_SETTING_DEF;
		stSysInfo.stSetting.ArcLimit = ARC_OVER_SETTING_DEF;
		stSysInfo.stSetting.TempDelay = TEMP_DELAY_TIMESEC_MIN;
		stSysInfo.stSetting.ArcDelay = ARC_DELAY_TIMESEC_MIN;
		stSysInfo.stSetting.OutFireType =0;
		stSysInfo.stSetting.TripType =0;

		stSysInfo.stSetting.TripAddr = 0;
		stSysInfo.stSetting.TripBO = 0;

		stSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stSysInfo.stSetting, sizeof(stSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
		delay_ms(100);

		//设置变量为0 
		memset((UINT8 *)&stSysInfo.stSysVar,0,sizeof(stSysInfo.stSysVar));

		stSysInfo.stSysVar.SysWorkMod = 3;
		stSysInfo.stSysVar.Version = SAMP_UINT_VERSION;

		//初始化CAN
		CAN_Mask_Init(2,stSysInfo.stSetting.MasterAddr,stSysInfo.stSetting.SampAddr);

		//初始化comapi
		CanInit_Api(0,stSysInfo.stSetting.SampAddr,stSysInfo.stSetting.MasterAddr);
}

#endif

