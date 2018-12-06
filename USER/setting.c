#include<string.h>
#include "setting.h"
#include "type_slv.h"


stSysInfo_Def stSysInfo;

stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


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
			memset((UINT8*)&stSlvEventBuf[ui].stSlvEvent,0,sizeof(stUserSlvEventReq_Def));
			stSlvEventBuf[ui].EventStatus =1;//值标志
			break;
		}
	}	
}

//心跳事件 发送采样值
//每 1  S 调用
void SendSampling(void)
{
	if(stSysInfo.stSysVar.SysWorkMod !=3 )
		return;
	
	SlvSampTempReq();
	SlvSampTempReq();

	stSysInfo.stSysVar.HeartCnt ++;
	if(stSysInfo.stSysVar.HeartCnt > 5)
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
		stSysInfo.stSysVar.HeartCnt =5;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
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
		//	SlvEventReq((stSysEven_Def*)&stSlvEventBuf[ui]);
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


//定时器100ms 调用1 次
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
		if(stSysInfo.stSysVar.TripArcTimeCnt > stSysInfo.stSetting.ArcDelay*10)
		{
			ArcOver = 1;
			stSysInfo.stSysVar.TripArcTimeCnt = stSysInfo.stSetting.ArcDelay*10;
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
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*10)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*10;
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
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*10)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*10;
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

	//弧光判断
	if(stSysInfo.stSysVar.SampArc > stSysInfo.stSetting.ArcLimit)
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
	if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
	||(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
	||(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
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
		stSysInfo.stSysVar.TripFaileTimeCnt =0;
	}

	if(ArcOver)//弧光越限  管理单元记录
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN = 1;//弧光越限状态
	}
	if(TempOver)//温度越限  管理单元记录
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN = 1;//弧光越限状态
	}
	
	//发送跳闸记录
	if((InputOver ==0)&&(stSysInfo.stSysVar.TripFlag))
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

	//t跳闸复归
	if(stSysInfo.stSysVar.TripFlag)
	{
		stSysInfo.stSysVar.ResetTripTimeCnt ++;
		if((stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_RESET_TIMEOUT)
		&&(stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_FAILE_TIMEOUT))	
		{
			stSysInfo.stSysVar.ResetTripTimeCnt =0;
			stSysInfo.stSysVar.TripFaileTimeCnt = 0; //复归跳闸
			stSysInfo.stSysVar.TripFlag = 0;		
		}
	}
	//t灭火复归
	if(stSysInfo.stSysVar.FireFlag)
	{
		stSysInfo.stSysVar.ResetFireTimeCnt ++;
		if(stSysInfo.stSysVar.ResetFireTimeCnt>=FIRE_RESET_TIMEOUT)
		{
			stSysInfo.stSysVar.ResetFireTimeCnt =0;
			stSysInfo.stSysVar.FireFlag = 0;		
		}
	}
}



