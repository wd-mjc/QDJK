/*
@file     setting_trip.c
@brief    配置及逻辑管理(跳闸单元)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#include<string.h>
#include "setting.h"
#include "type_slv.h"
#include "setting_trip.h"

#include "lora.h"
#include "board_io.h"
#include "ff.h"
#include "hc595.h"


stTSysInfo_Def stTSysInfo;

#ifdef BOARD_TRIP


//设置地址
void WriteUnitAddr(stUnitSetNetAddr_Def *addr)
{

	stTSysInfo.stSetting.TripAddr = addr->UnitID;
	//stTSysInfo.stSetting.NET1ID = addr->NET1ID;
	stTSysInfo.stSetting.NET2ID = addr->NET2ID;
	stTSysInfo.stSetting.NET2FR = addr->NET2FR;

	stTSysInfo.stSysVar.SaveFalshCfg = 1;
	//配置lora模块
	//配置LORA
	lora_set_config(UART_PORT_LORA1,(stTSysInfo.stSetting.NET2FR*1000000),stTSysInfo.stSetting.NET2ID,stTSysInfo.stSetting.TripAddr);

}
//读地址
void ReadUnitAddr(stUnitSetNetAddr_Def *addr)
{
	addr->ConsoleID =0;
	addr->MasterID =0;
	addr->UnitID = stTSysInfo.stSetting.TripAddr;
	addr->NET1ID = 0 ;
	addr->NET2ID = stTSysInfo.stSetting.NET2ID ;
	addr->NET1FR = 0; 
	addr->NET2FR = stTSysInfo.stSetting.NET2FR;
}
//读DBG
void ReadUnitDbg(stUnitReadDbg_Def *pDbg)
{
	UINT8 ui;
	char buf[30];
	if(pDbg ==NULL)
		return;

	pDbg->UnitID = stTSysInfo.stSetting.TripAddr;

	pDbg->UnitType = 4;
	pDbg->Status.uiAll = stTSysInfo.stSysVar.SysStatus.uiAll;
	pDbg->Temp1 = 0;
	pDbg->Temp2 = 0;
	pDbg->Temp3 = 0;
	pDbg->ArcOrDO = stTSysInfo.stSysVar.TripDo;
	pDbg->IO = 0;
	pDbg->Version = stTSysInfo.stSysVar.Version;

	sprintf(pDbg->buf,"MasterCnt:%d addr:",stTSysInfo.stSysVar.MasterAddrCnt);
	for(ui=0;ui<stTSysInfo.stSysVar.MasterAddrCnt;ui++)
	{
		sprintf(buf,"0x%x ",stTSysInfo.stSysVar.stMasterAddr[0].MasterAddr);
		if(strlen(pDbg->buf)+strlen(buf) < DEBUG_BUF_LEN)
			strcat(pDbg->buf,buf);
	}
}


//添加MasterAddr到缓冲区
UINT8 AddrIndex =0;
void AddMasterAddr(UINT16 Addr)
{
	UINT8 ui;
	for(ui =0;ui< MASTER_ADDR_CNTMAX;ui++)
	{
		if(stTSysInfo.stSysVar.stMasterAddr[ui].MasterAddr ==  0)
		{
			stTSysInfo.stSysVar.stMasterAddr[ui].MasterAddr = Addr;
			stTSysInfo.stSysVar.MasterAddrCnt = ui +1;
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt = 0;//清除心跳计数
			break;
		}
		else if(stTSysInfo.stSysVar.stMasterAddr[ui].MasterAddr == Addr)
		{
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt = 0;
			break;
		}
	}
	if(ui >= MASTER_ADDR_CNTMAX)
	{
		stTSysInfo.stSysVar.stMasterAddr[AddrIndex].MasterAddr = Addr;
		stTSysInfo.stSysVar.stMasterAddr[AddrIndex].HeartLostCnt = 0;
		stTSysInfo.stSysVar.MasterAddrCnt = MASTER_ADDR_CNTMAX;
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		AddrIndex ++;
		if(AddrIndex >= MASTER_ADDR_CNTMAX)
			AddrIndex = 0;
	}
}
//判断地址是否为缓冲区地址
UINT8 CheckMasterAddr(UINT16 Addr)
{
	UINT8 ui;
	for(ui =0;ui< MASTER_ADDR_CNTMAX;ui++)
	{
		if(stTSysInfo.stSysVar.stMasterAddr[ui].MasterAddr == Addr)
		{
			return 1;
		}
	}
	return 0;
}

//设置继电器闭合时间
void SetTripChDelay(UINT16 Mask)
{
	UINT8 ui;
	for(ui=0;ui<TRIP_UNIT_TRIPCH;ui++)
	{
		if(Mask&(0x01<<ui))
		{
			//闭合继电器
			DO_config(ui,1);// 1 -闭合 0- 释放
			stTSysInfo.stSysVar.TripChDelay[ui]= TRIP_UNIT_RESET_DEF;
			stTSysInfo.stSysVar.TripResetDelay = TRIP_RESET_TIMEOUT;

			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 1;
			stTSysInfo.stSysVar.TripDo |= Mask;	
		}	
	}
}

//检查继电器恢复
//每1s调用
void TCheckTripCHReset(void)
{
	UINT8 ui;

	if(stTSysInfo.stSysVar.TripResetDelay )
	{
		stTSysInfo.stSysVar.TripResetDelay--;
		if(stTSysInfo.stSysVar.TripResetDelay==0)
		{
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 0;
		}
	}
	
	for(ui=0;ui<TRIP_UNIT_TRIPCH;ui++)
	{
		
		if(stTSysInfo.stSysVar.TripChDelay[ui] > 0)
		{
			stTSysInfo.stSysVar.TripChDelay[ui] -- ;
			if(stTSysInfo.stSysVar.TripChDelay[ui]  == 0)
			{
				stTSysInfo.stSysVar.TripDo &= (~(0x0001<<ui));
				//	释放继电器
				DO_config(ui,0);// 1 -闭合 0- 释放
			}
		}
	}
}

//1s 调用
void TCheckMasterLinkLost(void)
{
	UINT8 ui = 0,res =0;
	if(stTSysInfo.stSysVar.MasterAddrCnt == 0)
	{
		stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt ++;
		if(stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt > HEART_OVER_TIMESEC_MAX)
		{
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostFlag = 1;
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt = HEART_OVER_TIMESEC_MAX;
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
		}
		else
		{	
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostFlag = 0;
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
		}
	}

	for(ui=0;ui<stTSysInfo.stSysVar.MasterAddrCnt;ui++)
	{
		stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt ++;
		if(stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt > HEART_OVER_TIMESEC_MAX)
		{
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostFlag = 1;
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostCnt = HEART_OVER_TIMESEC_MAX;
			res = 1;
		}
		else
		{	
			stTSysInfo.stSysVar.stMasterAddr[ui].HeartLostFlag = 0;
		}

		if(res)
		{
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
		}
		else
		{
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
		}
	}
}


// 250ms 调用
void TripLedStatus(void)
{
	//led_config
	if(stTSysInfo.stSysVar.SysStatus.uiAll & 0x07f)//告警
		led_config(LED_CH_ALAM, 1);
	else
		led_config(LED_CH_ALAM, 0);
}

//每1s调用
void TripGetPwr(void)
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
			pwrmin = T_PWIN_ERR_MIN;
			pwrmax = T_PWIN_ERR_MAX;
		break; 
		case 1 :      
			pwrmin = T_PW50_ERR_MIN;
			pwrmax = T_PW50_ERR_MAX;
		break; 
		case 2 :      
			pwrmin = T_PW90_ERR_MIN;
			pwrmax = T_PW90_ERR_MAX;
		break; 
		case 3 :      
			pwrmin = T_PW240_ERR_MIN;
			pwrmax = T_PW240_ERR_MAX;
		break;
		case 4 :      
			pwrmin = T_PW33_ERR_MIN;
			pwrmax = T_PW33_ERR_MAX;
		break;	
		default :  break;
		}
		if((pwr[ui] > pwrmax )||(pwr[ui] < pwrmin))
		{
			if(stTSysInfo.stSysVar.PwrErrCnt[ui] <= T_PWR_ERR_CNT)
				stTSysInfo.stSysVar.PwrErrCnt[ui] ++;
		}
		else
		{
			stTSysInfo.stSysVar.PwrErrCnt[ui] = 0;
		}
	}

	if((stTSysInfo.stSysVar.PwrErrCnt[1]>= T_PWR_ERR_CNT)//pwr
	||(stTSysInfo.stSysVar.PwrErrCnt[4]>= T_PWR_ERR_CNT))
	{
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =1;
	}
	else
	{
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_PWR_ERROR =0;
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
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	br =  sizeof(stTSetting_def);
	res = f_write(&fsrc, (UINT8*)&stTSysInfo.stSetting,br, &bw);
	if (res || bw < br) 
	{
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
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
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		return;
	}	

	bw = sizeof(stTSetting_def);
	res = f_read(&fsrc, (UINT8*)&stTSysInfo.stSetting,bw , &br);  
	if (res || br < bw) 
	{
		stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FLASH_ERROR = 1;
		f_close(&fsrc);
		return;
	}

	//关闭打开的文件
	f_close(&fsrc);

}

//检查写FLASH 任务
//每1s调用
void TCheckWriteFlash(void)
{
	if(stTSysInfo.stSysVar.SaveFalshCfg)
	{
		stTSysInfo.stSysVar.SaveFalshCfg = 0;


		//初始化comapi
		LoraInit_Api(2, stTSysInfo.stSetting.TripAddr, 0);

		stTSysInfo.stSetting.SysCheck1 = TRIP_FLASH_SAVE_FLAG;
		stTSysInfo.stSetting.SysCheck2 = TRIP_FLASH_SAVE_FLAG;

		stTSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stTSysInfo.stSetting, sizeof(stTSysInfo.stSetting)-1);
		
		//写FLASH 
		FlashWrite();
	}
}

//主函数调用
//初始化跳闸单元
void SysInitTripUnit(void)
{
	UINT8 u8crc = 0;
	//UINT8 ui;
	
	//设置变量为0 
	memset((UINT8 *)&stTSysInfo.stSysVar,0,sizeof(stTSysInfo.stSysVar));
	
	//读flash 获取配置stMSysInfo
	FlashRead();
	
	if((stTSysInfo.stSetting.SysCheck1 != TRIP_FLASH_SAVE_FLAG)||(stTSysInfo.stSetting.SysCheck2 != TRIP_FLASH_SAVE_FLAG))
	{
		stTSysInfo.stSetting.SysCheck1 = TRIP_FLASH_SAVE_FLAG;
		stTSysInfo.stSetting.SysCheck2 = TRIP_FLASH_SAVE_FLAG;

		stTSysInfo.stSetting.TripAddr = TRIP_UNIT_ADDR_DEF;
		stTSysInfo.stSetting.NET2ID = LORA_NET2ID_DEF;
		stTSysInfo.stSetting.NET2FR = LORA_NET2FR_DEF;

		stTSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stTSysInfo.stSetting, sizeof(stTSysInfo.stSetting)-1);

		//写FLASH 
		FlashWrite();
	}
	else
	{
		u8crc = CRC8((UINT8 *)&stTSysInfo.stSetting, sizeof(stTSysInfo.stSetting)-1);
		if(u8crc != stTSysInfo.stSetting.uiCrc8)
		{
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
		if(stTSysInfo.stSetting.TripAddr &TRIP_UNIT_ADDR_MASK != TRIP_UNIT_ADDR_MASK )
		{
			stTSysInfo.stSetting.TripAddr = TRIP_UNIT_ADDR_DEF;
			stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		}
	}

	stTSysInfo.stSysVar.SysWorkMod = 1;
	stTSysInfo.stSysVar.Version = TRIP_UNIT_VERSION_DEF;

	//配置LORA
	//lora_set_config(UART_PORT_LORA1,(stTSysInfo.stSetting.NET2FR*1000000),stTSysInfo.stSetting.NET2ID,stTSysInfo.stSetting.TripAddr);

	//初始化comapi
	LoraInit_Api(2, stTSysInfo.stSetting.TripAddr, 0);
}

UINT16 TripTimer600smCnt = 0;//10分钟
UINT8 TripTime1sCnt =0;
//主函数调用250ms
void TripTimerTask(void)
{
	TripTime1sCnt ++;
	if(TripTime1sCnt >= 4)//1s
	{
		TripTime1sCnt =0;
		stTSysInfo.stSysVar.Time ++;
		TCheckWriteFlash();//写FLASH
		TCheckTripCHReset();//复归
		TCheckMasterLinkLost();//连接状态
		TripGetPwr();//电源状态
		TripTimer600smCnt ++;
		if(TripTimer600smCnt >600)
		{
			TripTimer600smCnt =0;
			adc_adjust();//校准adc
		}
	}
	//250MS
	TripLedStatus();//Led
}

void tResetSystem(void)
{
	stTSysInfo.stSetting.SysCheck1 = TRIP_FLASH_SAVE_FLAG;
	stTSysInfo.stSetting.SysCheck2 = TRIP_FLASH_SAVE_FLAG;

	stTSysInfo.stSetting.TripAddr = TRIP_UNIT_ADDR_DEF;
	stTSysInfo.stSetting.NET2ID = LORA_NET2ID_DEF;
	stTSysInfo.stSetting.NET2FR = LORA_NET2FR_DEF;

	stTSysInfo.stSetting.uiCrc8= CRC8((UINT8 *)&stTSysInfo.stSetting, sizeof(stTSysInfo.stSetting)-1);

	//写FLASH 
	FlashWrite();

		//设置变量为0 
	memset((UINT8 *)&stTSysInfo.stSysVar,0,sizeof(stTSysInfo.stSysVar));

	stTSysInfo.stSysVar.SysWorkMod = 1;
	stTSysInfo.stSysVar.Version = TRIP_UNIT_VERSION_DEF;

	//配置LORA
	lora_set_config(UART_PORT_LORA1,(stTSysInfo.stSetting.NET2FR*1000000),stTSysInfo.stSetting.NET2ID,stTSysInfo.stSetting.TripAddr);

	//初始化comapi
	LoraInit_Api(2, stTSysInfo.stSetting.TripAddr, 0);
		
}


#endif

