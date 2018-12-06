/*
@file     docom_can.c
@brief    CAN  通信协议解析 (采集器)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/

#include<string.h>
#include "type_slv.h"
#include "commapi.h"
#include "setting_samp.h"

#ifdef BOARD_SAMP

//////////////////////////////////////////////////////////////////////
//外部函数
///////////////////////////////////////////////////////////////////////
//发送温度值
void SlvSampTempReq(void)
{
	stUserSlvSampTemp_Def SlvSampTemp;
	SlvSampTemp.UserSlvCmd = SLV_SAMP_TEMP_CMD;
	SlvSampTemp.Version = stSysInfo.stSysVar.Version;
	SlvSampTemp.SampTemp[0] = stSysInfo.stSysVar.SampTemp[0];
	SlvSampTemp.SampTemp[1] = stSysInfo.stSysVar.SampTemp[1];
	SlvSampTemp.SampTemp[2] = stSysInfo.stSysVar.SampTemp[2];
	CanApi_Snd(0,(UINT8 *)&SlvSampTemp,sizeof(SlvSampTemp));
}
//发送弧光值
void SlvSampArcReq(void)
{
	UINT16 Res;
	char buf[12];
	stUserSlvSampArc_Def SlvSampArc;
	SlvSampArc.UserSlvCmd = SLV_SAMP_ARC_CMD;
	SlvSampArc.CfgVersion = stSysInfo.stSetting.CfgVersion;
	SlvSampArc.SlvStatus.uiAll = stSysInfo.stSysVar.SysStatus.uiAll;
	SlvSampArc.SampArc = stSysInfo.stSysVar.SampArc;
	SlvSampArc.SampIO = stSysInfo.stSysVar.SampIO;
	Res = CanApi_Snd(0,(UINT8 *)&SlvSampArc,sizeof(SlvSampArc));

	if(Res)
	{
		if(Res== 1)
			sprintf(buf,"Timeout");
		else if(Res == 0x10)
			sprintf(buf,"BitErr");
		else if(Res == 0x20)
			sprintf(buf,"FormErr");
		else if(Res == 0x30)
			sprintf(buf,"AckErr");
		else if(Res == 0x40)
			sprintf(buf,"HideErr");
		else if(Res == 0x50)
			sprintf(buf,"ShowErr");
		else if(Res == 0x60)
			sprintf(buf,"CrcErr");
		else if(Res == 0x70)
			sprintf(buf,"SetErr");
		else
			sprintf(buf,"OtherErr");
		sprintf(&stSDebug.Buf[0],"CanErr:%s\r\n",buf);
		stSDebug.Len = strlen(stSDebug.Buf);
	}
	else
	{
		stSDebug.Len =0;
	}
}

// 发送跳闸事件
void SlvTripReq(void)
{
	stUserSlvTripReq_Def SlvTripReq;//跳闸	

	stSysInfo.stSysVar.TripSendCnt ++;//收到回复清零
	stSysInfo.stSysVar.TripEvent  = 1;//定时器查询，调用本函数
	if(stSysInfo.stSysVar.TripSendCnt > 3)
	{
		stUserSlvEventReq_Def SlvEvent;//发送事件
		stSysInfo.stSysVar.TripEvent =0;
		stSysInfo.stSysVar.TripSendCnt =0;
		//记录发送跳闸信号失败
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR = 1;
		
		SlvEvent.EventType.bit.uiEventType_Record = 1;
		SlvEvent.Data1.RecordData1.bit.uiEvent_Record_SENDTRIPERR =1;
		SlvEvent.Data2.Data2 = 0;
		SlvEvent.Data3.Data3 =0;
		AddEvent((stUserSlvEventReq_Def *)&SlvEvent);//添加发送事件
		return;
	}

	SlvTripReq.UserSlvCmd = SLV_SEND_TRIP_CMD;
	SlvTripReq.TripUnitAddr = stSysInfo.stSetting.TripAddr;
	SlvTripReq.TripBO = stSysInfo.stSetting.TripBO;

	stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 1;

	CanApi_Snd(0,(UINT8 *)&SlvTripReq,sizeof(SlvTripReq));	

}

// 发送记录事件
void SlvEventReq(stSysEven_Def *SysEven)
{
	stUserSlvEventReq_Def SlvEventReq;

	SysEven->EventStatus =2;//发送中
	SysEven->SendCnt ++;//收到回复清零
	if(SysEven->SendCnt  > 1)//重发次数到后清零
	{
		SysEven->EventStatus =0;
		SysEven->SendCnt =0;
		memset((UINT8*)&SysEven->stSlvEvent,0,sizeof(SysEven->stSlvEvent));
		return;
	}

	SlvEventReq.UserSlvCmd = SLV_SEND_EVENT_CMD;
	SlvEventReq.EventType.uiAll = SysEven->stSlvEvent.EventType.uiAll;
	SlvEventReq.Data1.Data1= SysEven->stSlvEvent.Data1.Data1;
	SlvEventReq.Data2.Data2= SysEven->stSlvEvent.Data2.Data2;
	SlvEventReq.Data3.Data3= SysEven->stSlvEvent.Data3.Data3;
//MJC DELETE
//	CanApi_Snd(0,(UINT8 *)&SlvEventReq,sizeof(stUserSlvEventReq_Def));
}

void SlvSendCfgVersion(void)
{
	stUserSlvMasterIdResp_Def SlvMasterIdResp;

	SlvMasterIdResp.UserSlvCmd = SLV_MASTER_SET_CMD |SLV_RESP_CMD_MASK;
	SlvMasterIdResp.SettingReq = 1;
	stSysInfo.stSysVar.ReadSettingFlag = 1;

	CanApi_Snd(0,(UINT8 *)&SlvMasterIdResp,sizeof(SlvMasterIdResp));	
	
}	

//////////////////////////////////////////////////////////////////////
//DOCOM 内部函数
///////////////////////////////////////////////////////////////////////
UINT8 SlvSampTempResp(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvSampResp_Def *SlvSampResp = (stUserSlvSampResp_Def *)pRcvBuf;
		
	stSysInfo.stSysVar.SysTime = (UINT32 ) &pRcvBuf[1];
	stSysInfo.stSysVar.ConfigVersion = SlvSampResp->ConfigVersion;
	stSysInfo.stSysVar.SettingVersion = SlvSampResp->SettingVersion;

	if(((stSysInfo.stSysVar.ConfigVersion + stSysInfo.stSysVar.SettingVersion ) != stSysInfo.stSetting.CfgVersion)
		||(stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN))
	{
		SlvSendCfgVersion();
	}	
	stSysInfo.stSysVar.HeartCnt =0;
	return 0;	
}
UINT8 SlvSampArcResp(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvSampResp_Def *SlvSampResp = (stUserSlvSampResp_Def *)pRcvBuf;
		
	stSysInfo.stSysVar.SysTime = (UINT32 ) &pRcvBuf[1];
	stSysInfo.stSysVar.ConfigVersion = SlvSampResp->ConfigVersion;
	stSysInfo.stSysVar.SettingVersion = SlvSampResp->SettingVersion;

	if(((stSysInfo.stSysVar.ConfigVersion + stSysInfo.stSysVar.SettingVersion ) != stSysInfo.stSetting.CfgVersion)
		||(stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN))
	{
		SlvSendCfgVersion();
	}
	stSysInfo.stSysVar.HeartCnt =0;
	return 0;	
}

UINT8 SlvTripResp(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stSysInfo.stSysVar.TripEvent =0;
	stSysInfo.stSysVar.TripSendCnt =0;
	stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR = 0;
	return 0;	
}

UINT8 SlvEventResp(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
//重发送需判断
/*	UINT8 ui;
	for(ui=0;ui<EVENT_BUF_CNT;ui++)
	{
		if(stSlvEventBuf[ui].EventStatus ==2)
		{
			stSlvEventBuf[ui].EventStatus = 0;//发送完成
			memset((UINT8*)&stSlvEventBuf[ui].stSlvEvent,0,sizeof(stUserSlvEventReq_Def));
		}	
		break;
	}
*/	
	return 0;
}

UINT8 SlvSetZeroReq(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvZeroReq_Def *SlvZeroReq =(stUserSlvZeroReq_Def*)pRcvBuf;
	stUserSlvZeroResp_Def SlvZeroResp;
	UINT8 Res =0;
	if(sizeof(stUserSlvZeroReq_Def)!= uiRcvLen)
	{
		Res =1;		
	}	
	//处理错误
	//if(SlvZeroReq->MasterAddr ==0)
	//	Res =1;	
	if(SlvZeroReq->ArcRadio > ARC_RADIO_MAX)
		Res =1;	

	if(Res == 1)
	{
		//stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		SlvZeroResp.UserSlvCmd = SLV_ZERO_SET_CMD|SLV_ERR_CMD_MASK;
	}
	else
	{
		//stSysInfo.stSetting.MasterAddr = SlvZeroReq->MasterAddr;
		stSysInfo.stSetting.TempRadio[0] = SlvZeroReq->TempRadio[0];
		stSysInfo.stSetting.TempRadio[1] = SlvZeroReq->TempRadio[1];
		stSysInfo.stSetting.TempRadio[2] = SlvZeroReq->TempRadio[2];
		stSysInfo.stSetting.ArcZero = SlvZeroReq->ArcZero;
		stSysInfo.stSetting.ArcRadio = SlvZeroReq->ArcRadio;	

		/*if(stSysInfo.stSetting.SettingSn != SlvZeroReq->SettingSn)
		{
			stSysInfo.stSysVar.SettingSnTmp = SlvZeroReq->SettingSn;
			SlvZeroResp.SettingReq = 1;
		}
		else
		{
			SlvZeroResp.SettingReq = 0;
		}*/
		
		//stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN =0;
		SlvZeroResp.UserSlvCmd = SLV_ZERO_SET_CMD|SLV_RESP_CMD_MASK;
	}

	CanApi_Snd(0,(UINT8 *)&SlvZeroResp,sizeof(SlvZeroResp));	
	return 0;	
}

UINT8 SlvSetMasterReq(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvMasterIdReq_Def *SlvMasterIdReq =(stUserSlvMasterIdReq_Def*)pRcvBuf;
	stUserSlvMasterIdResp_Def SlvMasterIdResp;

	if(stSysInfo.stSetting.MasterAddr != SlvMasterIdReq->MasterAddr)
	{
		stSysInfo.stSetting.MasterAddr = SlvMasterIdReq->MasterAddr;
		stSysInfo.stSysVar.SaveFlash = 1;
	}
	stSysInfo.stSetting.AlrmTempPrec = SlvMasterIdReq->AlrmTempPrec;
	stSysInfo.stSetting.AlrmArcPrec = SlvMasterIdReq->AlrmArcPrec;
	//stSysInfo.stSetting.CfgVersion = SlvMasterIdReq->CfgVersion;

	if(stSysInfo.stSetting.CfgVersion != SlvMasterIdReq->CfgVersion)
	{
		stSysInfo.stSysVar.CfgVersionTmp = SlvMasterIdReq->CfgVersion;
		SlvMasterIdResp.SettingReq = 1;
		stSysInfo.stSysVar.ReadSettingFlag = 1;
	}
	else
	{
		SlvMasterIdResp.SettingReq = 0;
	}

	SlvMasterIdResp.UserSlvCmd = SLV_MASTER_SET_CMD |SLV_RESP_CMD_MASK;

	return CanApi_Snd(0,(UINT8 *)&SlvMasterIdResp,sizeof(SlvMasterIdResp));	
	
}	

UINT8 SlvSetLogicReq(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvLogicReq_Def *SlvLogicReq = (stUserSlvLogicReq_Def *)pRcvBuf;
	UINT8 CMD;
	if(sizeof(stUserSlvLogicReq_Def)!= uiRcvLen)
	{
		CMD = 	SLV_LOGIC_SET_CMD|SLV_ERR_CMD_MASK;
	}
	else
	{
		stSysInfo.stSetting.OutFireType = SlvLogicReq->OutFireType;
		stSysInfo.stSetting.TripType = SlvLogicReq->TripType;
		stSysInfo.stSetting.LocalTripType = SlvLogicReq->LocalTripType;
		stSysInfo.stSetting.TripAddr = SlvLogicReq->TripAddr;
		stSysInfo.stSetting.TripBO = SlvLogicReq->TripBO;

		CMD = 	SLV_LOGIC_SET_CMD|SLV_RESP_CMD_MASK;
	}
	
	CanApi_Snd(0,(UINT8 *)&CMD ,1);	
	return 0;	
}
UINT8 SlvSetSettingReq(UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvSettingReq_Def *SlvSettingReq = (stUserSlvSettingReq_Def *)pRcvBuf;
	UINT8 Res =0;
	UINT8 CMD;
	
	if(sizeof(stUserSlvSettingReq_Def)!= uiRcvLen)
	{
		Res =1;
	}
	//处理错误
	if(SlvSettingReq->TempLimit < TEMP_SETTING_MIN)
		Res =1;
	if(SlvSettingReq->ArcLimit < ARC_SETTING_MIN)
		Res =1;
	if(Res)
	{
		CMD = 	SLV_SETTING_SET_CMD|SLV_ERR_CMD_MASK;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
	}
	else
	{
		stSysInfo.stSetting.InputBit = SlvSettingReq->InputBit;
		stSysInfo.stSetting.TempLimit = SlvSettingReq->TempLimit;
		stSysInfo.stSetting.ArcLimit = SlvSettingReq->ArcLimit;
		stSysInfo.stSetting.TempDelay = SlvSettingReq->TempDelay;
		stSysInfo.stSetting.ArcDelay = SlvSettingReq->ArcDelay;

		CMD = 	SLV_SETTING_SET_CMD|SLV_RESP_CMD_MASK;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 0;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN =0;

		stSysInfo.stSetting.CfgVersion = stSysInfo.stSysVar.CfgVersionTmp;//写入定值序号到FLASH
		stSysInfo.stSysVar.SaveFlash = 1;
		stSysInfo.stSysVar.ReadSettingFlag = 0;
	}
	CanApi_Snd(0,(UINT8 *)&CMD ,1);	
	return 0;	
}

// 采集器docom
UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	UINT8 Res = 0;
	
	if( pRcvBuf[0] != SLV_MASTER_SET_CMD)//不是设置地址命令
	if(SourAddr != stSysInfo.stSetting.MasterAddr)
	{
		return 1;
	}	
	if(pRcvBuf[0] & SLV_ERR_CMD_MASK)//是错误报文
	{
		return 1;
	}
	
	if(pRcvBuf[0] & SLV_RESP_CMD_MASK)//是应答报文
	{  
		switch( pRcvBuf[0] & SLV_CMD_MASK )
		{
		case SLV_SAMP_TEMP_CMD : //温度采样                      
			Res=SlvSampTempResp(pRcvBuf,uiRcvLen);
		break; 
		case SLV_SAMP_ARC_CMD : //弧光采样                            
			Res=SlvSampArcResp(pRcvBuf,uiRcvLen);
		break; 
		case SLV_SEND_TRIP_CMD : //动作                       
			Res=SlvTripResp(pRcvBuf,uiRcvLen);
		break; 
		case SLV_SEND_EVENT_CMD : //事件                   
			Res=SlvEventResp(pRcvBuf,uiRcvLen);
		break; 
		default :  break;
		}
	}
	else
	{
		switch( pRcvBuf[0] )//管理单元写写设置
		{		
		case SLV_MASTER_SET_CMD : //主控设置                    
			Res=SlvSetMasterReq(pRcvBuf,uiRcvLen);
		break; 
		case SLV_ZERO_SET_CMD : //参数设置                    
			Res=SlvSetZeroReq(pRcvBuf,uiRcvLen);
		break; 
		case SLV_LOGIC_SET_CMD : //逻辑设置                     
			Res=SlvSetLogicReq(pRcvBuf,uiRcvLen);
		break; 
		case SLV_SETTING_SET_CMD : //定值设置                    
			Res=SlvSetSettingReq(pRcvBuf,uiRcvLen);
		break; 
		default :  break;
		}
	}	
	return Res;
}


#endif

 