/*
@file     setting_master.c
@brief    配置及逻辑管理(管理单元)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/

//单元管理器工作流程
//1.上电初始化
//2.心跳到台区
//3.检查配置
//4.写配置到采集器
//5.进入正常工作


#include <string.h>
#include "setting.h"
#include "type_slv.h"
#include "type_com.h"
#include "delay_stm32.h"
#include "setting_master.h"

#include "lora.h"
#include "board_io.h"
#include "cand.h"
#include "ff.h"



stMSysInfo_Def stMSysInfo;
stMasterEvent_Def stMasterEvent[EVENT_CNT_MAX];
stMDebug_Def stMDebug;
stActiveReq_Def stMSendTrip;

#ifdef BOARD_MASTER

//获取单元信息
stUnit_Def *GetUnitByAddr(UINT16 Addr)
{
	UINT8 i;
	for(i=0;i<stMSysInfo.stSysVar.AllUnitCnt;i++)
	{
		if(Addr == stMSysInfo.stSysVar.stUnit[i].UnitID)
			return (stUnit_Def *)&stMSysInfo.stSysVar.stUnit[i];
	}
	return NULL;
}
//获取单元信息
stUnit_Def *GetUnitByIndex(UINT8 Index)
{
	if(Index >= stMSysInfo.stSysVar.AllUnitCnt)
		return NULL;

	return (stUnit_Def *)&stMSysInfo.stSysVar.stUnit[Index];
}

//获取配置信息
stUnitConfig_Def *GetConfigByAddr(UINT16 Addr)
{
	UINT8 i;
	for(i=0;i<stMSysInfo.stSetting.SampCnt;i++)
	{
		if(Addr == stMSysInfo.stSetting.stSampUnit[i].SampleID)
			return (stUnitConfig_Def *)&stMSysInfo.stSetting.stSampUnit[i];
	}
	return NULL;
}

//获取配置信息
stUnitConfig_Def *GetConfigByIndex(UINT8 Index)
{
	if(Index>= stMSysInfo.stSetting.SampCnt)
		return NULL;
	
	return (stUnitConfig_Def *)&stMSysInfo.stSetting.stSampUnit[Index];
}
//获取单元变量
stUnitVar_def *GetVarByAddr(UINT16 Addr)
{
	UINT8 i;
	for(i=0;i<stMSysInfo.stSysVar.AllUnitCnt;i++)
	{
		if(Addr == stMSysInfo.stSysVar.stUnitVar[i].UnitID)
			return (stUnitVar_def *)&stMSysInfo.stSysVar.stUnitVar[i];
	}
	return NULL;
}
//获取单元信息
stUnitVar_def *GetVarByIndex(UINT8 Index)
{
	if(Index >= stMSysInfo.stSysVar.AllUnitCnt)
		return NULL;

	return (stUnitVar_def *)&stMSysInfo.stSysVar.stUnitVar[Index];
}

/////////////////////////////////////////////////////////////////////
//管理器工作
/////////////////////////////////////////////////////////////////////

//设置地址
void WriteUnitAddr(stUnitSetNetAddr_Def *addr)
{
  //MJC DEBUG
        stMSysInfo.stSetting.U8MasterAddr = (addr->MasterID)>>8;
	stMSysInfo.stSetting.U16MasterAddr = addr->MasterID&0xff00;
	stMSysInfo.stSetting.stConfig.MasterID = addr->MasterID&0xff00;
	stMSysInfo.stSetting.stConfig.ConsoleID= addr->ConsoleID&0x00ff;
	stMSysInfo.stSetting.NET1ID = addr->NET1ID;
	stMSysInfo.stSetting.NET2ID = addr->NET2ID;
	stMSysInfo.stSetting.NET1FR = addr->NET1FR;
	stMSysInfo.stSetting.NET2FR = addr->NET2FR;
	
	stMSysInfo.stSetting.ArcZero = addr->ArcZero;
	stMSysInfo.stSetting.ArcRadio = addr->ArcRadio;
	stMSysInfo.stSetting.TempRadio[0]=addr->TempRadio[0];
	stMSysInfo.stSetting.TempRadio[1]=addr->TempRadio[1];
	stMSysInfo.stSetting.TempRadio[2]=addr->TempRadio[2];

	stMSysInfo.stSysVar.SaveFalshCfg = 1;

	stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 0;

	//调用写LORA 配置

	//初始化lora 配置
	lora_set_config(UART_PORT_LORA1,(stMSysInfo.stSetting.NET1FR*1000000),stMSysInfo.stSetting.NET1ID,stMSysInfo.stSetting.U16MasterAddr);
	lora_set_config(UART_PORT_LORA2,(stMSysInfo.stSetting.NET2FR*1000000),stMSysInfo.stSetting.NET2ID,stMSysInfo.stSetting.U16MasterAddr);

	//初始化CAN
	CAN_Mask_Init(1,stMSysInfo.stSetting.U8MasterAddr,0);
	
}

//读地址
void ReadUnitAddr(stUnitSetNetAddr_Def *addr)
{
	addr->ConsoleID = stMSysInfo.stSetting.stConfig.ConsoleID;
	addr->MasterID = stMSysInfo.stSetting.U16MasterAddr;
	addr->UnitID = stMSysInfo.stSetting.U16MasterAddr;
	addr->NET1ID = stMSysInfo.stSetting.NET1ID;
	addr->NET2ID = stMSysInfo.stSetting.NET2ID;
	addr->NET1FR = stMSysInfo.stSetting.NET1FR;
	addr->NET2FR = stMSysInfo.stSetting.NET2FR;
	
	addr->ArcZero = stMSysInfo.stSetting.ArcZero ;
	addr->ArcRadio = stMSysInfo.stSetting.ArcRadio;
	addr->TempRadio[0] = stMSysInfo.stSetting.TempRadio[0];
	addr->TempRadio[1] = stMSysInfo.stSetting.TempRadio[1];
	addr->TempRadio[2] = stMSysInfo.stSetting.TempRadio[2];
}
//读DBG
void ReadUnitDbg(stUnitReadDbg_Def *pDbg)
{
	if(pDbg ==NULL)
		return;

	pDbg->UnitID = stMSysInfo.stSetting.U16MasterAddr;

	pDbg->UnitType = 2;
	pDbg->Status.uiAll = stMSysInfo.stSysVar.SysStatus.uiAll;
	pDbg->Temp1 = 0;
	pDbg->Temp2 = 0;
	pDbg->Temp3 = 0;
	pDbg->ArcOrDO = 0;
	pDbg->IO = stMSysInfo.stSysVar.SampIO;
	pDbg->Version = stMSysInfo.stSysVar.Version;

	if(stMDebug.Len + 15 < DEBUG_BUF_LEN)
		sprintf(&stMDebug.Buf[stMDebug.Len],"CfgVer:%d,%d\r\n",stMSysInfo.stSetting.stConfig.ConfigVersion,stMSysInfo.stSetting.stLimit.SettingVersion);
	
	strncpy(pDbg->buf,stMDebug.Buf,DEBUG_BUF_LEN);
	//sprintf(pDbg->buf,"ConfigVersion:%d,%d \r\n",stMSysInfo.stSetting.stConfig.ConfigVersion,stMSysInfo.stSetting.stLimit.SettingVersion);
}

//形成单元管理器的心跳数据
void CopyMasterHeartToBuf(void)
{
	stUnit_Def *pUnit;

	pUnit = GetUnitByAddr(stMSysInfo.stSetting.U16MasterAddr);
	if(pUnit==NULL)
		return;
	pUnit->Status.uiAll = stMSysInfo.stSysVar.SysStatus.uiAll;
	pUnit->Temp1 = stMSysInfo.stSysVar.Temp1;
	pUnit->Temp2 = stMSysInfo.stSysVar.Temp2;
	pUnit->Temp3 = stMSysInfo.stSysVar.Temp3;
	pUnit->ArcOrDO = stMSysInfo.stSysVar.Arc;
	pUnit->IO = stMSysInfo.stSysVar.SampIO;
	pUnit->Version = stMSysInfo.stSysVar.Version;
}

//跳闸单元 和采集单元连接断开判断
void UnitAddrLinkLost(UINT16 Addr)
{
	stUnitVar_def * UnitVar;
	stUnit_Def * UnitHeart;
	
	UnitVar = GetVarByAddr(Addr);
	UnitHeart = GetUnitByAddr(Addr);
	if((UnitVar ==NULL)||(UnitHeart==NULL))
		return;
	
	if(UnitVar->HeartLostCnt <= 10)
		UnitVar->HeartLostCnt ++;//心跳回复清零
	if(UnitVar->HeartLostCnt >= 6)
	{
		UnitHeart->Status.bit.uiSTATUS_LINKLOST_WARN = 1;
	}
	else
	{
		UnitHeart->Status.bit.uiSTATUS_LINKLOST_WARN = 0;
		//UnitVar->HeartLostCnt = 0;
	}
}

//判断采集单元 连接状态
void SampUnitHeartLost(void)
{
	UINT8 ui;

	for(ui =0;ui<stMSysInfo.stSysVar.AllUnitCnt;ui++)
	{
		if(stMSysInfo.stSysVar.stUnit[ui].UnitType == 3)//采集器
		{
			//stMSysInfo.stSysVar.stUnitVar[ui].HeartLostCnt ++;
			//收到采集器心跳清零
			UnitAddrLinkLost(stMSysInfo.stSysVar.stUnit[ui].UnitID);
		}
	}
}

//每1s调用
void MasterGetInput(void)
{
	float pwr[5];
	float pwrmin,pwrmax;
	UINT8 ui;

	get_pwr_val((float *)pwr);

	//pwr
	for(ui=0;ui<5;ui++)
	{
		switch( ui )
		{
		case 0 :         
			pwrmin = M_PWIN_ERR_MIN;
			pwrmax = M_PWIN_ERR_MAX;
		break; 
		case 1 :      
			pwrmin = M_PW50_ERR_MIN;
			pwrmax = M_PW50_ERR_MAX;
		break; 
		case 2 :      
			pwrmin = M_PW90_ERR_MIN;
			pwrmax = M_PW90_ERR_MAX;
		break; 
		case 3 :      
			pwrmin = M_PW240_ERR_MIN;
			pwrmax = M_PW240_ERR_MAX;
		break;
		case 4 :      
			pwrmin = M_PW33_ERR_MIN;
			pwrmax = M_PW33_ERR_MAX;
		break;	
		default :  break;
		}
		if((pwr[ui] > pwrmax )||(pwr[ui] < pwrmin))
		{
			if(stMSysInfo.stSysVar.PwrErrCnt[ui] <= M_PWR_ERR_CNT)
				stMSysInfo.stSysVar.PwrErrCnt[ui] ++;
		}
		else
		{
			stMSysInfo.stSysVar.PwrErrCnt[ui] = 0;
		}
	}

	if((stMSysInfo.stSysVar.PwrErrCnt[0]>= M_PWR_ERR_CNT)//pwr
	||(stMSysInfo.stSysVar.PwrErrCnt[1]>= M_PWR_ERR_CNT)
	||(stMSysInfo.stSysVar.PwrErrCnt[2]>= M_PWR_ERR_CNT)
	||(stMSysInfo.stSysVar.PwrErrCnt[3]>= M_PWR_ERR_CNT)
	||(stMSysInfo.stSysVar.PwrErrCnt[4]>= M_PWR_ERR_CNT))
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =1;
	}
	else
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =0;
	}
	
	stMSysInfo.stSysVar.SampIO = (GetDI(0) <<4)&0xf0;//开入
}

// 250ms 调用
void MasterLedStatus(void)
{
	//led_config
	if(stMSysInfo.stSysVar.SysStatus.uiAll & 0x07f)//告警
		led_config(LED_CH_ALAM, 1);
	else
		led_config(LED_CH_ALAM, 0);

	if(stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN|stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN)
		led_config(LED_CH_OVER, 1);
	else
		led_config(LED_CH_OVER, 0);
}

//心跳事件 发送到台区
//每 2S 调用
void SendUnitSampling(void)
{
	if(stMSysInfo.stSysVar.SysWorkMod != 4)
		return;
	
	CopyMasterHeartToBuf();//单元管理器的心跳数据
		
	UnitSamplingReq();//心跳到台区
        
        UnitSendHeartTripUnit();//心跳到跳闸单元
	
	//单元管理器的连接判断
	stMSysInfo.stSysVar.HeartCnt ++;
	if(stMSysInfo.stSysVar.HeartCnt > 6)
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
		stMSysInfo.stSysVar.HeartCnt =6;
	}
	else
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
	}
	//采集器的连接判断
	SampUnitHeartLost();
}

//检查重发
//每1s调用
void MasterCheckResend(void)
{
	if(stMSysInfo.stSysVar.TripEvent)//检查发送跳闸
	{
		ReSendUnitTripReq(&stMSendTrip);
	}
	if(stMSysInfo.stSysVar.TripRestCnt)//清除发送跳闸通信错误标志
	{
		stMSysInfo.stSysVar.TripRestCnt --;
		if(stMSysInfo.stSysVar.TripRestCnt == 0)
		{
			//stMSysInfo.stSysVar.TripSendCnt =0;			
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR =0;
			//stMSysInfo.stSysVar.TripEvent = 0;//停止发送
			memset((UINT8 *)&stMSendTrip,0,sizeof(stActiveReq_Def)); 
		}
	}
}

//UINT8 ReadConfigIndex = 0;
//每 2S 调用
void MasterCheckLogic(void)
{
	UINT8 ui;

	//UnitSendReadSetting();
	//读取越限
	if(stMSysInfo.stSysVar.SettingVersion != stMSysInfo.stSetting.stLimit.SettingVersion)
	{
		UnitSendReadSetting();
	}
	//读取配置
	else if(stMSysInfo.stSysVar.ConfigVersion != stMSysInfo.stSetting.stConfig.ConfigVersion)
	{
		UnitSendReadConfig(0,3);//回复报文修改 ConfigVersion
		stMSysInfo.stSysVar.ReadConfigIndex = 0;
		stMSysInfo.stSysVar.ReadConfigFlag = 1;//停心跳
	}
	//读取配置循环读取所有配置
	else if(stMSysInfo.stSysVar.ReadConfigIndex)
	{
		if(stMSysInfo.stSysVar.ReadConfigIndex <  stMSysInfo.stSetting.stConfig.UnitConfigMax)
		{			
			UnitSendReadConfig(stMSysInfo.stSysVar.ReadConfigIndex,3);
                        //stMSysInfo.stSysVar.ReadConfigIndex += 1; //收到后加1
		}
		else
		{
			stMSysInfo.stSysVar.ReadConfigIndex = 0;
			stMSysInfo.stSysVar.ReadConfigFlag = 0;
		}
	}
	else
	{
		if(stMSysInfo.stSysVar.SysWorkMod == 1)
			stMSysInfo.stSysVar.SysWorkMod =2;
	}
}

//添加事件到缓冲区
void AddEventToBuf(stEventReq_Def *pEvent)
{
	UINT8 ui;
	for(ui=0;ui<EVENT_CNT_MAX;ui++)
	{
		if(stMasterEvent[ui].EventStatus ==0)
		{
			break;
		}
	}	

	if(ui>=EVENT_CNT_MAX)
		ui = 0;	

	memcpy((UINT8 *)&stMasterEvent[ui].stEvent,(UINT8 *)pEvent,sizeof(stEventReq_Def));
	stMasterEvent[ui].EventSendCnt =0;//再次清零			
	stMasterEvent[ui].EventStatus =1;//值标志
	return;
}
//type 1-温度越限 2-弧光越限 3.跳闸4.灭火5.跳闸失败
void AddSlvEvent(UINT16 Addr,UINT8 Type)
{
	stEventReq_Def stEventReq;
	stUnit_Def *pUnit;
	stUnitConfig_Def *pConfig;

	memset((UINT8*)&stEventReq,0,sizeof(stEventReq_Def));

	stEventReq.Time = stMSysInfo.stSysVar.SysTime;
	stEventReq.UnitType = 3;//采集器
	stEventReq.ADDR = Addr;
	stEventReq.EventType.uiAll = 0;
	if(Type == 1)
		stEventReq.EventType.bit.uiSTATUS_TEMPOVER_WARN = 1;
	else if(Type == 2)
		stEventReq.EventType.bit.uiSTATUS_ARCOVER_WARN = 1;	
	else if(Type == 3)
		stEventReq.EventType.bit.uiSTATUS_TRIP_WARN = 1;	
	else if(Type == 4)
		stEventReq.EventType.bit.uiSTATUS_FIRE_WARN = 1;
	else if(Type == 5)
		stEventReq.EventType.bit.uiSTATUS_TRIPFAILED_WARN = 1;	
	else if(Type == 6)
		stEventReq.EventType.bit.uiSTATUS_SENDTRIP_ERROR = 1;	
	else return;

	pUnit = GetUnitByAddr(Addr);
	if(pUnit==NULL)
		return;

	if(pUnit->UnitType !=3)
		return;

	stEventReq.Temp1 = pUnit->Temp1;
	stEventReq.Temp2 = pUnit->Temp2;
	stEventReq.Temp3 = pUnit->Temp3;
	stEventReq.Arc1 = pUnit->ArcOrDO;
	stEventReq.SampIO = pUnit->IO;

	if((Type ==3)||(Type >=4))//跳闸
	{
		pConfig = GetConfigByAddr(Addr);
		if(pConfig==NULL)
			return;
		stEventReq.TripUnitID = pConfig->TripUnitID;
		stEventReq.TripDOBit = pConfig->TripDOBit;
	}
	else 
	{
		stEventReq.TripUnitID =0;
		stEventReq.TripDOBit =0;
	}
	AddEventToBuf(&stEventReq);	
}
//type 1-温度越限 2-弧光越限 3.跳闸4.灭火5.跳闸失败6.跳闸发送失败
void AddMasterEvent(UINT16 Addr,UINT8 Type)
{
	stEventReq_Def stEventReq;
	stUnit_Def *pUnit;
	stUnitConfig_Def *pConfig;

	memset((UINT8*)&stEventReq,0,sizeof(stEventReq_Def));

	stEventReq.Time = stMSysInfo.stSysVar.SysTime;
	stEventReq.UnitType = 2;//单元管理器
	stEventReq.ADDR = Addr;
	stEventReq.EventType.uiAll = 0;
	if(Type == 1)
		stEventReq.EventType.bit.uiSTATUS_TEMPOVER_WARN = 1;
	else if(Type == 2)
		stEventReq.EventType.bit.uiSTATUS_ARCOVER_WARN = 1;	
	else if(Type == 3)
		stEventReq.EventType.bit.uiSTATUS_TRIP_WARN = 1;	
	else if(Type == 4)
		stEventReq.EventType.bit.uiSTATUS_FIRE_WARN = 1;
	else if(Type == 5)
		stEventReq.EventType.bit.uiSTATUS_TRIPFAILED_WARN = 1;	
	else if(Type == 6)
		stEventReq.EventType.bit.uiSTATUS_SENDTRIP_ERROR = 1;	
	else return;

	pUnit = GetUnitByAddr(Addr);
	if(pUnit==NULL)
		return;

	if(pUnit->UnitType !=2)
		return;
	if(Type == 6)
	{
		stEventReq.TripUnitID = stMSendTrip.TripUnitID;
		stEventReq.TripDOBit = stMSendTrip.TripDOBit;
	}

	AddEventToBuf(&stEventReq);	
}

//每 2S 调用
void MasterSendEvent(void)
{
	UINT8 ui;
	for(ui=0;ui<EVENT_CNT_MAX;ui++)
	{
		if(stMasterEvent[ui].EventStatus)
		{
			UnitEventReq(&stMasterEvent[ui].stEvent);
			stMasterEvent[ui].EventStatus = 0;//已发送
			break;//每次只发送1次事件
		}
	}
}

//每 250mS 调用
void MasterCheckEvent(void)
{
	stUnitVar_def * UnitVar;
	stUnit_Def * UnitHeart;
	UINT8 i;
	
	for(i=0;i<stMSysInfo.stSysVar.AllUnitCnt;i++)
	{	
		UnitVar = GetVarByIndex(i);
		UnitHeart = GetUnitByIndex(i);
		if((UnitVar == NULL)||(UnitHeart==NULL))
			return;

		if(UnitHeart->UnitType == 2)//单元管理
		{
			//跳闸发送失败
			if(UnitHeart->Status.bit.uiSTATUS_SENDTRIP_ERROR)
			{
				if(!UnitVar->TripSendErrFlag )
				{
					AddMasterEvent(UnitHeart->UnitID,6);
					UnitVar->TripSendErrFlag = 1;
					//return;//间隔发送
				}
			}
			else
			{
				UnitVar->TripSendErrFlag = 0;
			}
		}

		if(UnitHeart->UnitType != 3)//以下只发送采集器事件
			continue;
		
		//温度越限
		if(UnitHeart->Status.bit.uiSTATUS_TEMPOVER_WARN)
		{
			if(!UnitVar->TempOverEventFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,1);
				UnitVar->TempOverEventFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->TempOverEventFlag = 0;
		}
		//弧光越限
		if(UnitHeart->Status.bit.uiSTATUS_ARCOVER_WARN)
		{
			if(!UnitVar->ArcOverEventFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,2);
				UnitVar->ArcOverEventFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->ArcOverEventFlag = 0;
		}
		//跳闸
		if(UnitHeart->Status.bit.uiSTATUS_TRIP_WARN)
		{
			if(!UnitVar->TripEventFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,3);
				UnitVar->TripEventFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->TripEventFlag = 0;
		}
		//灭火
		if(UnitHeart->Status.bit.uiSTATUS_FIRE_WARN)
		{
			if(!UnitVar->FireEventFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,4);
				UnitVar->FireEventFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->FireEventFlag = 0;
		}
		//跳失败
		if(UnitHeart->Status.bit.uiSTATUS_TRIPFAILED_WARN)
		{
			if(!UnitVar->TripFailedFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,5);
				UnitVar->TripFailedFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->TripFailedFlag = 0;
		}
		//跳闸发送失败
		if(UnitHeart->Status.bit.uiSTATUS_SENDTRIP_ERROR)
		{
			if(!UnitVar->TripSendErrFlag )
			{
				AddSlvEvent(UnitHeart->UnitID,6);
				UnitVar->TripSendErrFlag = 1;
				//return;//间隔发送
			}
		}
		else
		{
			UnitVar->TripSendErrFlag = 0;
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
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	br =  sizeof(stMSetting_def);
	res = f_write(&fsrc, (UINT8*)&stMSysInfo.stSetting,br, &bw);
	if (res || bw < br) 
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
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
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	bw = sizeof(stMSetting_def);
	res = f_read(&fsrc, (UINT8*)&stMSysInfo.stSetting,bw , &br);  
	if (res || br < bw) 
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		f_close(&fsrc);
		return;
	}

	//关闭打开的文件
	f_close(&fsrc);

}

void MSaveFlashClearVar(void)
{
	stMSysInfo.stSysVar.SaveFalshCfg = 0;
	stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR = 0;
}

//检查写FLASH 任务
//每1s调用
void CheckWriteFlash(void)
{
	if(stMSysInfo.stSysVar.SaveFalshCfg)
	{
		stMSysInfo.stSysVar.SaveFalshCfg = 0;

		MSaveFlashClearVar();
		//关键变量赋值
		Init_SettingVar();

		//初始化comapi
		CanInit_Api(1,stMSysInfo.stSetting.U8MasterAddr,stMSysInfo.stSetting.U8MasterAddr);
		LoraInit_Api(0, stMSysInfo.stSetting.U16MasterAddr, stMSysInfo.stSetting.stConfig.ConsoleID);
		CAN_Mask_Init(1,stMSysInfo.stSetting.U8MasterAddr,0);

		stMSysInfo.stSetting.SysCheck1 = 0x5342;
		stMSysInfo.stSetting.SysCheck2 = 0x5342;

		stMSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stMSysInfo.stSetting, sizeof(stMSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
		
	}
}


/////////////////////////////////////////////////////////////////////
//初始化工作
/////////////////////////////////////////////////////////////////////


//上电初始化和写配置初始化
void Init_SettingVar( void )
{
        UINT8 ui;
	UINT8 Index;
	UINT8 TripCnt;
	UINT16 TripAddrBuf[SAMPLING_UNIT_CNT_MAX];

	stMSysInfo.stSysVar.Version =  MASTER_VERSION;

	//初始化配置版本
	stMSysInfo.stSysVar.ConfigVersion = stMSysInfo.stSetting.stConfig.ConfigVersion;
	stMSysInfo.stSysVar.SettingVersion = stMSysInfo.stSetting.stLimit.SettingVersion;

	//master
	stMSysInfo.stSysVar.AllUnitCnt = 1;
	stMSysInfo.stSysVar.stUnit[0].UnitType = 2;//单元管理器
	stMSysInfo.stSetting.U16MasterAddr = stMSysInfo.stSetting.stConfig.MasterID & 0xff00;
	stMSysInfo.stSetting.U8MasterAddr = (stMSysInfo.stSetting.stConfig.MasterID >> 8);
	stMSysInfo.stSetting.SampCnt  = stMSysInfo.stSetting.stConfig.UnitConfigMax;
	stMSysInfo.stSysVar.stUnit[0].UnitID = stMSysInfo.stSetting.U16MasterAddr ;
	stMSysInfo.stSysVar.stUnitVar[0].UnitID = stMSysInfo.stSetting.U16MasterAddr ;
	Index = 1;
	stMSysInfo.stSysVar.AllUnitCnt += stMSysInfo.stSetting.SampCnt;
	for(ui=0;ui<stMSysInfo.stSetting.SampCnt;ui++)
	{
		stMSysInfo.stSysVar.stUnit[Index+ui].UnitType = 3;//采集器
		stMSysInfo.stSysVar.stUnit[Index+ui].UnitID = stMSysInfo.stSetting.stSampUnit[ui].SampleID;
		stMSysInfo.stSysVar.stUnitVar[Index+ui].UnitID = stMSysInfo.stSetting.stSampUnit[ui].SampleID;

		TripAddrBuf[ui] =  stMSysInfo.stSetting.stSampUnit[ui].TripUnitID;
	}

	//删除重复单元地址
	TripCnt = removemultip(TripAddrBuf, stMSysInfo.stSetting.SampCnt);
	Index +=  stMSysInfo.stSetting.SampCnt;
	stMSysInfo.stSysVar.AllUnitCnt += TripCnt;//单元总数
	stMSysInfo.stSysVar.TripUnitCnt = TripCnt;
	for(ui=0;ui<TripCnt;ui++)
	{
		stMSysInfo.stSysVar.TripUnitAddrBuf[ui] = TripAddrBuf[ui];
		stMSysInfo.stSysVar.stUnit[Index+ui].UnitType = 4;//跳闸单元
		stMSysInfo.stSysVar.stUnit[Index+ui].UnitID = TripAddrBuf[ui];
		stMSysInfo.stSysVar.stUnitVar[Index+ui].UnitID = TripAddrBuf[ui];
	}

	memset((UINT8*)&stMasterEvent,0,sizeof(stMasterEvent)); //清楚发送事件
	
}

//主函数调用
//初始化MASTER
void SysInitMaster(void)
{
	UINT8 u8crc = 0;
	//UINT8 ui;
	
	//设置变量为0 
	memset((UINT8 *)&stMSysInfo.stSysVar,0,sizeof(stMSysInfo.stSysVar));
	
	//读flash 获取配置stMSysInfo
	FlashRead();
        
        //读FLASH后必须加延时才能初始化CAN，否则CAN异常
        delay_ms(100);        

	if((stMSysInfo.stSetting.SysCheck1 != 0x5342)||(stMSysInfo.stSetting.SysCheck2 != 0x5342))
	{
		stMSysInfo.stSetting.SysCheck1 = 0x5342;
		stMSysInfo.stSetting.SysCheck2 = 0x5342;
		stMSysInfo.stSetting.U8MasterAddr = MASTER_ADDRU8_DEF;
		stMSysInfo.stSetting.U16MasterAddr = MASTER_ADDRU16_DEF;
		stMSysInfo.stSetting.NET1ID = LORA_NET1ID_DEF;
		stMSysInfo.stSetting.NET2ID = LORA_NET2ID_DEF;	
		stMSysInfo.stSetting.NET1FR = LORA_NET1FR_DEF;
		stMSysInfo.stSetting.NET2FR = LORA_NET2FR_DEF;	
		stMSysInfo.stSetting.SampCnt = 0;
		memset((UINT8 *)&stMSysInfo.stSetting.stConfig,0,sizeof(stMSysInfo.stSetting.stConfig));
		memset((UINT8 *)&stMSysInfo.stSetting.stSampUnit,0,sizeof(stMSysInfo.stSetting.stSampUnit));
		memset((UINT8 *)&stMSysInfo.stSetting.stLimit,0,sizeof(stMSysInfo.stSetting.stLimit));

		stMSysInfo.stSetting.stConfig.ConsoleID = CONSOLE_ADDR_DEF;
		stMSysInfo.stSetting.stConfig.MasterID = MASTER_ADDRU16_DEF;

		stMSysInfo.stSetting.stLimit.TempLimit = TEMP_OVER_SETTING_DEF;
		stMSysInfo.stSetting.stLimit.ArcLimit = ARC_OVER_SETTING_DEF;

		stMSysInfo.stSetting.stLimit.TempLimit = TEMP_OVER_SETTING_DEF;
		stMSysInfo.stSetting.stLimit.ArcLimit = ARC_OVER_SETTING_DEF;

		stMSysInfo.stSetting.stLimit.AlrmTempPrec = 100;
		stMSysInfo.stSetting.stLimit.AlrmArcPrec = 100;

		stMSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stMSysInfo.stSetting, sizeof(stMSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
	}
	else
	{
		u8crc = CRC8((UINT8 *)&stMSysInfo.stSetting, sizeof(stMSysInfo.stSetting)-1);
		if(u8crc != stMSysInfo.stSetting.uiCrc8)
		{
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if((stMSysInfo.stSetting.U16MasterAddr < 0x0100)||((stMSysInfo.stSetting.U16MasterAddr & 0x0ff) !=0 ))
		{
			stMSysInfo.stSetting.U8MasterAddr = MASTER_ADDRU8_DEF;
			stMSysInfo.stSetting.U16MasterAddr = MASTER_ADDRU16_DEF;
			stMSysInfo.stSetting.stConfig.MasterID = MASTER_ADDRU16_DEF;
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if((stMSysInfo.stSetting.stConfig.ConsoleID ==0)||(stMSysInfo.stSetting.stConfig.ConsoleID > 0x00ff))
		{
			stMSysInfo.stSetting.stConfig.ConsoleID = CONSOLE_ADDR_DEF;
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}
		if((stMSysInfo.stSetting.stLimit.TempLimit < TEMP_OVER_SETTING_MIN)||(stMSysInfo.stSetting.stLimit.ArcLimit < ARC_OVER_SETTING_MIN))
		{
			stMSysInfo.stSetting.stLimit.TempLimit = TEMP_OVER_SETTING_DEF;
			stMSysInfo.stSetting.stLimit.ArcLimit = ARC_OVER_SETTING_DEF;
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}

		if((stMSysInfo.stSetting.stLimit.TempDelay < TEMP_DELAY_TIMESEC_MIN)||(stMSysInfo.stSetting.stLimit.ArcDelay < ARC_DELAY_TIMESEC_MIN))
		{
			stMSysInfo.stSetting.stLimit.TempDelay = TEMP_DELAY_TIMESEC_MIN;
			stMSysInfo.stSetting.stLimit.ArcDelay = ARC_DELAY_TIMESEC_MIN;
			stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		}
	}

	//初始化变量数据
	Init_SettingVar();

	//初始化lora 配置   改到写地址后初始化
//	lora_set_config(UART_PORT_LORA1,(stMSysInfo.stSetting.NET1FR*1000000),stMSysInfo.stSetting.NET1ID,stMSysInfo.stSetting.U16MasterAddr);
//	lora_set_config(UART_PORT_LORA2,(stMSysInfo.stSetting.NET2FR*1000000),stMSysInfo.stSetting.NET2ID,stMSysInfo.stSetting.U16MasterAddr);

	//初始化CAN
	CAN_Mask_Init(1,stMSysInfo.stSetting.U8MasterAddr,0);


	//初始化comapi
	CanInit_Api(1,stMSysInfo.stSetting.U8MasterAddr,stMSysInfo.stSetting.U8MasterAddr);
	LoraInit_Api(0, stMSysInfo.stSetting.U16MasterAddr, stMSysInfo.stSetting.stConfig.ConsoleID);
	
	stMSysInfo.stSysVar.SysWorkMod =4;//进入正常工作状态
}

//主函数调用
void MasterPowerOn(void)
{
	//1.上电初始化
	//2.心跳到台区
	//3.检查配置
	//4.写配置到采集器
	//5.进入正常工作
	
	SysInitMaster();	
	stMSysInfo.stSysVar.SysWorkMod =0;
	/*
	do
	{
		SendUnitSampling();
		delay_ms(1000);
	}while(stMSysInfo.stSysVar.SysWorkMod != 1);
	do
	{
		MasterCheckLogic();
		delay_ms(1000);
	}while(stMSysInfo.stSysVar.SysWorkMod != 2);
	*/
	stMSysInfo.stSysVar.SysWorkMod =4;//进入正常工作状态
}


//////////////////////////////////////////////////////////////////////////
void mCanTest(UINT16 SampAddr)
{
	stUserSlvZeroReq_Def ZeroReq;
	UINT8 Addr = SampAddr & 0x00ff;
	stUnitConfig_Def *pConfig;

	ZeroReq.UserSlvCmd = SLV_ZERO_SET_CMD;
	ZeroReq.TempRadio[0] = 1;
	ZeroReq.TempRadio[1] = 2;
	ZeroReq.TempRadio[2] = 3;
	ZeroReq.ArcZero = 0x456;
	ZeroReq.ArcRadio = 0x789;
	
	CanApi_Snd(Addr,(UINT8 *)&ZeroReq,sizeof(ZeroReq));
}

UINT8 SetAddrIndex =1;//跳过本单元
//采集器设置
//每1s 调用
void MasterSetSlvConfig(void)
{
//	mCanTest(55);

	if(!stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].SetMasterIDFlag)//设置ID
	{
		//mSlvSetMasterReq(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].UnitID);
		//delay_ms(10);
	}
	if(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].SettingReq)//发送定值
	{
		stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].RespMask = 0;//准备接收
		mSlvSetMasterReq(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].UnitID);
		delay_ms(50);
		mSlvSetZeroReq(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].UnitID);
		delay_ms(50);
		mSlvSetLogicReq(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].UnitID);
		delay_ms(50);
		mSlvSetSettingReq(stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].UnitID);

		//stMSysInfo.stSysVar.stUnitVar[SetAddrIndex].SettingReq = 0;//收到回复清零		
	}

	SetAddrIndex ++;//循环检查所有单元
	if(SetAddrIndex> stMSysInfo.stSetting.stConfig.UnitConfigMax)
		SetAddrIndex = 1;//跳过本单元
}

UINT16 Timer600smCnt = 0;//10分钟
UINT8 Timer1sCnt=0;
UINT8 Timer2sCnt=0;
UINT8 Timer5sCnt=0;
//主函数250 MS调用
void MasterTimerTask(void)
{
	Timer1sCnt ++;
	Timer2sCnt ++;
	Timer5sCnt ++;
	if(Timer5sCnt >=20)//5s
	{
		Timer5sCnt =0;
		if(!stMSysInfo.stSysVar.ReadConfigFlag)//配置期间停止心跳
			SendUnitSampling();//心跳发送
	}
	if(Timer2sCnt >=8)//2S
	{
		Timer2sCnt =0;
		MasterCheckLogic();//检查配置		
		MasterSendEvent();//发送事件
	}	
	if(Timer1sCnt >=4)//1S
	{
		Timer1sCnt =0;
		stMSysInfo.stSysVar.SysTime ++;
		MasterCheckResend();//跳闸从发
		CheckWriteFlash();	//写FLASH
		MasterGetInput(); //开入
		if(!stMSysInfo.stSysVar.ReadConfigFlag)//配置期间停止心跳
			MasterSetSlvConfig(); //下发配置到采集单元

		Timer600smCnt ++;
		if(Timer600smCnt >600)
		{
			Timer600smCnt = 0;
			adc_adjust();//校准adc
		}
	}	
	//250MS
	MasterLedStatus();//LED
	MasterCheckEvent();//检测事件
	
}

void mResetSystem(void)
{
	stMSysInfo.stSetting.SysCheck1 = 0x5342;
		stMSysInfo.stSetting.SysCheck2 = 0x5342;
		stMSysInfo.stSetting.U8MasterAddr = MASTER_ADDRU8_DEF;
		stMSysInfo.stSetting.U16MasterAddr = MASTER_ADDRU16_DEF;
		stMSysInfo.stSetting.NET1ID = LORA_NET1ID_DEF;
		stMSysInfo.stSetting.NET2ID = LORA_NET2ID_DEF;	
		stMSysInfo.stSetting.NET1FR = LORA_NET1FR_DEF;
		stMSysInfo.stSetting.NET2FR = LORA_NET2FR_DEF;	
		stMSysInfo.stSetting.SampCnt = 0;
		memset((UINT8 *)&stMSysInfo.stSetting.stConfig,0,sizeof(stMSysInfo.stSetting.stConfig));
		memset((UINT8 *)&stMSysInfo.stSetting.stSampUnit,0,sizeof(stMSysInfo.stSetting.stSampUnit));
		memset((UINT8 *)&stMSysInfo.stSetting.stLimit,0,sizeof(stMSysInfo.stSetting.stLimit));

		stMSysInfo.stSetting.stConfig.ConsoleID = CONSOLE_ADDR_DEF;
		stMSysInfo.stSetting.stConfig.MasterID = MASTER_ADDRU16_DEF;

		stMSysInfo.stSetting.stLimit.TempLimit = TEMP_OVER_SETTING_DEF;
		stMSysInfo.stSetting.stLimit.ArcLimit = ARC_OVER_SETTING_DEF;

		stMSysInfo.stSetting.stLimit.TempLimit = TEMP_OVER_SETTING_DEF;
		stMSysInfo.stSetting.stLimit.ArcLimit = ARC_OVER_SETTING_DEF;

		stMSysInfo.stSetting.stLimit.AlrmTempPrec = 100;
		stMSysInfo.stSetting.stLimit.AlrmArcPrec = 100;

		stMSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stMSysInfo.stSetting, sizeof(stMSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
		delay_ms(100);

		memset((UINT8 *)&stMSysInfo.stSysVar,0,sizeof(stMSysInfo.stSysVar));

		//初始化变量数据
		Init_SettingVar();

		//初始化lora 配置   改到写地址后初始化
		lora_set_config(UART_PORT_LORA1,(stMSysInfo.stSetting.NET1FR*1000000),stMSysInfo.stSetting.NET1ID,stMSysInfo.stSetting.U16MasterAddr);
		lora_set_config(UART_PORT_LORA2,(stMSysInfo.stSetting.NET2FR*1000000),stMSysInfo.stSetting.NET2ID,stMSysInfo.stSetting.U16MasterAddr);

		//初始化CAN
		CAN_Mask_Init(1,stMSysInfo.stSetting.U8MasterAddr,0);


		//初始化comapi
		CanInit_Api(1,stMSysInfo.stSetting.U8MasterAddr,stMSysInfo.stSetting.U8MasterAddr);
		LoraInit_Api(0, stMSysInfo.stSetting.U16MasterAddr, stMSysInfo.stSetting.stConfig.ConsoleID);
		
		stMSysInfo.stSysVar.SysWorkMod =4;//进入正常工作状态

		
}



#endif

