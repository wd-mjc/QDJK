/*
@file     docom_mcan.c
@brief    CAN  通信协议解析 (管理器)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#include <string.h>
#include "type_slv.h"
#include "commapi.h"
#include "setting_master.h"

#ifdef BOARD_MASTER

//////////////////////////////////////////////////////////////////////
//外部函数
///////////////////////////////////////////////////////////////////////

//管理器上电调用
//循环判断是否发送到采集器
void mSlvSetMasterReq(UINT16 SampAddr)
{
	stUserSlvMasterIdReq_Def MasterIdReq;
	UINT8 Addr = SampAddr & 0x00ff;
	
	MasterIdReq.UserSlvCmd = SLV_MASTER_SET_CMD;
	MasterIdReq.MasterAddr = stMSysInfo.stSetting.U8MasterAddr;
	MasterIdReq.AlrmTempPrec = stMSysInfo.stSetting.stLimit.AlrmTempPrec;
	MasterIdReq.AlrmArcPrec = stMSysInfo.stSetting.stLimit.AlrmArcPrec;
	MasterIdReq.CfgVersion =  stMSysInfo.stSetting.stLimit.SettingVersion+stMSysInfo.stSetting.stConfig.ConfigVersion;

	CanApi_Snd(Addr,(UINT8 *)&MasterIdReq,sizeof(MasterIdReq));
}
void mSlvSetZeroReq(UINT16 SampAddr)
{
	stUserSlvZeroReq_Def ZeroReq;
	UINT8 Addr = SampAddr & 0x00ff;
	stUnitConfig_Def *pConfig;

	pConfig = GetConfigByAddr(SampAddr);
	if(pConfig ==NULL)
		return ;

	ZeroReq.UserSlvCmd = SLV_ZERO_SET_CMD;
	ZeroReq.TempRadio[0] = pConfig->TempRadio[0];
	ZeroReq.TempRadio[1] = pConfig->TempRadio[1];
	ZeroReq.TempRadio[2] = pConfig->TempRadio[2];
	ZeroReq.ArcZero = pConfig->ArcZero;
	ZeroReq.ArcRadio = pConfig->ArcRadio;
	
	CanApi_Snd(Addr,(UINT8 *)&ZeroReq,sizeof(ZeroReq));
}

void mSlvSetLogicReq(UINT16 SampAddr)
{
	stUserSlvLogicReq_Def LogicReq;
	UINT8 Addr = SampAddr & 0x00ff;

	stUnitConfig_Def *pConfig;

	pConfig = GetConfigByAddr(SampAddr);
	if(pConfig ==NULL)
		return ;
	
	LogicReq.UserSlvCmd = SLV_LOGIC_SET_CMD;
	LogicReq.OutFireType = stMSysInfo.stSetting.stLimit.FireType;
	LogicReq.TripType = stMSysInfo.stSetting.stLimit.TripType;
	LogicReq.LocalTripType =0;
	LogicReq.TripAddr =  pConfig->TripUnitID;	
	LogicReq.TripBO =  pConfig->TripDOBit;		

	CanApi_Snd(Addr,(UINT8 *)&LogicReq,sizeof(LogicReq));
}
void mSlvSetSettingReq(UINT16 SampAddr)
{
	stUserSlvSettingReq_Def SettingReq;
	UINT8 Addr = SampAddr & 0x00ff;

	SettingReq.UserSlvCmd = SLV_SETTING_SET_CMD;
	SettingReq.InputBit = 0;
	SettingReq.TempLimit = stMSysInfo.stSetting.stLimit.TempLimit;
	SettingReq.ArcLimit = stMSysInfo.stSetting.stLimit.ArcLimit;
	SettingReq.TempDelay= stMSysInfo.stSetting.stLimit.TempDelay;
	SettingReq.ArcDelay= stMSysInfo.stSetting.stLimit.ArcDelay;

	CanApi_Snd(Addr,(UINT8 *)&SettingReq,sizeof(SettingReq));
}


//////////////////////////////////////////////////////////////////////
//DOCOM 内部函数
///////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//采集器回复
UINT8 mSlvSetMasterResp(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvMasterIdResp_Def *pMasterIdResp = (stUserSlvMasterIdResp_Def *)pRcvBuf;
	stUnitVar_def *pUnitVar; 
	UINT16 Addr;

	Addr = stMSysInfo.stSetting.U16MasterAddr | SourAddr;
	pUnitVar = GetVarByAddr(Addr);
	if(pUnitVar ==NULL)
		return 1;

	pUnitVar->HeartLostCnt = 0;//清除连接断开计数

	pUnitVar->SetMasterIDFlag = 1;//设置成功
	
	if(pMasterIdResp->SettingReq)
	{
		pUnitVar ->SettingReq = 1;//需要更新定值
	}		
}
UINT8 mSlvSetZeroResq(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUnitVar_def *pUnitVar; 
	UINT16 Addr;

	Addr = stMSysInfo.stSetting.U16MasterAddr | SourAddr;
	pUnitVar = GetVarByAddr(Addr);
	if(pUnitVar ==NULL)
		return 1;
	pUnitVar->RespMask |=0x01;
	if(pUnitVar->RespMask == 0x07)	//	3三个报文都回复
		pUnitVar ->SettingReq = 0;//需要更新定值清除
	
	return 0;
}
UINT8 mSlvSetLogicResp(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUnitVar_def *pUnitVar; 
	UINT16 Addr;

	Addr = stMSysInfo.stSetting.U16MasterAddr | SourAddr;
	pUnitVar = GetVarByAddr(Addr);
	if(pUnitVar ==NULL)
		return 1;
	pUnitVar->RespMask |=0x02;
	if(pUnitVar->RespMask == 0x07)	//	3三个报文都回复
		pUnitVar ->SettingReq = 0;//需要更新定值清除
	
	return 0;
}
UINT8 mSlvSetSettingResp(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUnitVar_def *pUnitVar; 
	UINT16 Addr;

	Addr = stMSysInfo.stSetting.U16MasterAddr | SourAddr;
	pUnitVar = GetVarByAddr(Addr);
	if(pUnitVar ==NULL)
		return 1;
	pUnitVar->RespMask |=0x04;
	if(pUnitVar->RespMask == 0x07)	//	3三个报文都回复
		pUnitVar ->SettingReq = 0;//需要更新定值清除
	
	return 0;
}

//////////////////////////////////////////////////////
//采集器上送
UINT8 mSlvSampTempReq(UINT8 SourAddr, UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvSampTemp_Def *pslvTemp =(stUserSlvSampTemp_Def *)pRcvBuf;
	stUnit_Def *pUnit;	
	stUserSlvSampResp_Def slvTempResp; 
	UINT16 Addr;
	UINT8 Res;
	char buf[12];
	
	if(uiRcvLen!= sizeof(stUserSlvSampTemp_Def))
		return 1;

	Addr = stMSysInfo.stSetting.U16MasterAddr |SourAddr;
	pUnit = GetUnitByAddr(Addr);
	if(pUnit ==NULL)
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		return 1;
	}	
	
	pUnit->Temp1 = pslvTemp->SampTemp[0];
	pUnit->Temp2 = pslvTemp->SampTemp[1];
	pUnit->Temp3 = pslvTemp->SampTemp[2];
	pUnit->Version = pslvTemp->Version;

	slvTempResp.UserSlvCmd = SLV_SAMP_TEMP_CMD|SLV_RESP_CMD_MASK;
	slvTempResp.Time = stMSysInfo.stSysVar.SysTime;
	slvTempResp.ConfigVersion = stMSysInfo.stSetting.stConfig.ConfigVersion;
	slvTempResp.SettingVersion = stMSysInfo.stSetting.stLimit.SettingVersion;

	Res = CanApi_Snd(SourAddr,(UINT8 *)&slvTempResp,sizeof(slvTempResp));
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
		sprintf(&stMDebug.Buf[0],"CanErr:%s\r\n",buf);
		stMDebug.Len = strlen(stMDebug.Buf);
	}
	else
	{
		stMDebug.Len =0;
	}
	return Res;
	
}

UINT8 mSlvSampArcReq(UINT8 SourAddr, UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvSampArc_Def *pslvArc =(stUserSlvSampArc_Def *)pRcvBuf;
	stUnit_Def *pUnit;
	stUnitVar_def *pUnitVar; 
	stUserSlvSampResp_Def slvTempResp; 
	UINT16 Addr;
	
	if(uiRcvLen!= sizeof(stUserSlvSampArc_Def))
		return 1;

	Addr = stMSysInfo.stSetting.U16MasterAddr |SourAddr;
	pUnit = GetUnitByAddr(Addr);
	if(pUnit ==NULL)
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		return 1;
	}
	
	pUnit->ArcOrDO= pslvArc->SampArc;
	pUnit->IO = pslvArc->SampIO;
	pUnit->Status.uiAll = pslvArc->SlvStatus.uiAll;

	if(pslvArc->CfgVersion != (stMSysInfo.stSetting.stConfig.ConfigVersion + stMSysInfo.stSetting.stLimit.SettingVersion))//判断设置定值是否修改
	{
		pUnitVar= GetVarByAddr(Addr);
		if(pUnit ==NULL)
			return 1;
		pUnitVar->SettingReq = 1;
	}
	
	slvTempResp.UserSlvCmd = SLV_SAMP_ARC_CMD|SLV_RESP_CMD_MASK;
	slvTempResp.Time = stMSysInfo.stSysVar.SysTime;
	slvTempResp.ConfigVersion = stMSysInfo.stSetting.stConfig.ConfigVersion;
	slvTempResp.SettingVersion = stMSysInfo.stSetting.stLimit.SettingVersion;

	return CanApi_Snd(SourAddr,(UINT8 *)&slvTempResp,sizeof(slvTempResp));
	
}

UINT8 mSlvTripReq(UINT8 SourAddr, UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvTripReq_Def * pTripReq =(stUserSlvTripReq_Def *)pRcvBuf;
	stUserSlvTripResp_Def TripResp;
	stUnit_Def *pUnit;
	UINT16 Addr;
	

	if(uiRcvLen!= sizeof(stUserSlvTripReq_Def))
		return 1;

	TripResp.UserSlvCmd = SLV_SEND_TRIP_CMD|SLV_RESP_CMD_MASK;
	CanApi_Snd(SourAddr,(UINT8 *)&TripResp,sizeof(TripResp));

	Addr = stMSysInfo.stSetting.U16MasterAddr |SourAddr;
	//pUnit = GetUnitByAddr(Addr);//判断采集器地址
	//if(pUnit ==NULL)
	//	return 1;
	UnitTripReq(pTripReq,Addr);	
        return 0;
}

UINT8 mSlvEventReq(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUserSlvEventResp_Def EventResp;
	EventResp.UserSlvCmd = SLV_SEND_EVENT_CMD|SLV_RESP_CMD_MASK;
	return CanApi_Snd(SourAddr,(UINT8 *)&EventResp,sizeof(EventResp));
}



// 单元管理器docom
UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	UINT8 Res = 0;
	//if(SourAddr != stMSysInfo.stSetting.SampAddr)
	//	return 1;
	if(pRcvBuf[0] & SLV_ERR_CMD_MASK)//是错误报文
	{
		return 1;
	}
	if(pRcvBuf[0] & SLV_RESP_CMD_MASK)//是应答报文
	{  
		switch( pRcvBuf[0] & SLV_CMD_MASK )
		{
		case SLV_MASTER_SET_CMD : //主控设置                    
			Res=mSlvSetMasterResp(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_ZERO_SET_CMD : //参数设置                    
			Res=mSlvSetZeroResq(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_LOGIC_SET_CMD : //逻辑设置                     
			Res=mSlvSetLogicResp(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_SETTING_SET_CMD : //定值设置                    
			Res=mSlvSetSettingResp(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		
		default :  break;
		}
	}
	else
	{
		switch( pRcvBuf[0] )//采集单元上送
		{
		case SLV_SAMP_TEMP_CMD : //温度采样                      
			Res=mSlvSampTempReq(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_SAMP_ARC_CMD : //弧光采样                            
			Res=mSlvSampArcReq(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_SEND_TRIP_CMD : //跳闸                   
			Res=mSlvTripReq(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		case SLV_SEND_EVENT_CMD : //事件                   
			Res=mSlvEventReq(SourAddr,pRcvBuf,uiRcvLen);
		break; 
		default :  break;
		}
	}	
	return Res;
}


#endif

 