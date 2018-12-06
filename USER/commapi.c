/*
@file     commapi.c
@brief    CAN LORA 通信API
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/
#include<string.h>
#include "type_com.h"
#include "type_slv.h"
#include "commapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (BOARD_MASTER)||defined (BOARD_SAMP)

//////////////////////////////////////////////////////////////////////////////////////
//CAN 通信
//////////////////////////////////////////////////////////////////////////////////////

//设置ID 和EID 发送数据
extern UINT8 CanLowlevelSend(UINT8 DestAddr,UINT8 SourAddr,UINT8 *Buf, UINT8 Len);
//解析BUF数据
extern UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen);
	
UINT8 uiSampAddr ;
UINT8 uiMasterAddr;
UINT8 uiMasterFlag;

//uiUnitAttr = 1 为单元管理器
//uiUnitAttr = 0 为采集器
void CanInit_Api(UINT8 uiUnitAttr,UINT8 uiUnitAddr,UINT8 uiMaAddr)
{
	if(uiUnitAttr)
	{
		uiMasterAddr = uiUnitAddr;
		uiSampAddr =0;
		uiMasterFlag = 1;
	}
	else
	{
		uiSampAddr = uiUnitAddr;
		uiMasterAddr =uiMaAddr;
		uiMasterFlag =0;
	}
}

//采集器DestAddr 填写0 
//管理器DestAddr 填写采集器编号
UINT8 CanApi_Snd(UINT8 DestAddr,UINT8 *Buf, UINT8 Len)
{
	if(uiMasterFlag)
	{
		return CanLowlevelSend(DestAddr, uiMasterAddr,Buf,Len);
	}
	else
	{
		return CanLowlevelSend(uiMasterAddr, uiSampAddr,Buf,Len);
	}
}

UINT8 CanApi_Rcv(UINT8 SourAddr,UINT8 * pRcvBuf, UINT8 uiRcvLen)
{
	if(uiMasterFlag)
	{
		return CanDocmd(SourAddr,pRcvBuf, uiRcvLen);
	}
	else
	{
		if(pRcvBuf[0] == SLV_MASTER_SET_CMD)//设置管理器地址
			uiMasterAddr = 	pRcvBuf[1]; 	//将修改标识符列表放到docom中处理
		return CanDocmd(SourAddr,pRcvBuf, uiRcvLen);
	}
}
#endif

#if defined (BOARD_MASTER)||defined (BOARD_TRIP)
//////////////////////////////////////////////////////////////////////////////////////
//LORA 通信
//////////////////////////////////////////////////////////////////////////////////////

//LORA发送数据
extern UINT8 LoraLowlevelSend(UINT8 LoraCh,UINT8 *Buf, UINT8 Len);
//解析BUF数据
extern UINT8 LoraDocmd(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen);


UINT16 U16ConsoleAddr ;//台区地址( 固定) 0x00XX
UINT16 U16MasterAddr;//单元管理器地址0xXX00
UINT8 uiConsoleFlag;

stLoraStruct_Def stLoraFrame;

//uiUnitAttr = 0 单元管理器uiUnitAddr=本单元地址uiConsoleAddr=台区地址
//uiUnitAttr = 1 台区                uiUnitAddr未用                   uiConsoleAddr=台区地址
//uiUnitAttr = 2 跳闸单元     uiUnitAddr=本单元地址uiConsoleAddr未用  
void LoraInit_Api(UINT8 uiUnitAttr,UINT16 uiUnitAddr,UINT16 uiConsoleAddr)
{
	if(uiUnitAttr ==1)//台区
	{		
		U16MasterAddr = 0;// 从配置文件
		U16ConsoleAddr =uiConsoleAddr;
		uiConsoleFlag = 1;
	}
	else if(uiUnitAttr ==2)//跳闸单元
	{
		U16MasterAddr = uiUnitAddr;
		U16ConsoleAddr =0;
		uiConsoleFlag = 0;
	}
	else// 单元管理器
	{
		U16MasterAddr = uiUnitAddr;
		U16ConsoleAddr =uiConsoleAddr;//从配置命令中获取
		uiConsoleFlag = 0;
	}
}

//单元管理器DestAddr 填写0 
//台区DestAddr 填写对应单元管理器地址
//返回0 成功 ,其他失败
UINT8 LoraApi_Snd(UINT16 DestAddr,UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	UINT8 LoraCh;
	
	if(Len > USER_DATA_LEN_MAX)
		return 1;

	if(DestAddr >= 0xff00)
		LoraCh = 2;
	else
		LoraCh = 1;

//	DestAddr = 0x0205;//DBG
//	U16ConsoleAddr = 0X0205;
		
	stLoraFrame.LoraType = 0x05;
	stLoraFrame.LoraSn =0;
	stLoraFrame.LoraCmd =0X01;
	stLoraFrame.LoraLen = (Len + 6 + 5);
	if(DestAddr != 0)
		stLoraFrame.LoraDestAddr = Tranverse16(DestAddr);
	else if(uiConsoleFlag)
		stLoraFrame.LoraDestAddr =Tranverse16( DestAddr);
	else	
		stLoraFrame.LoraDestAddr = Tranverse16(U16ConsoleAddr);
	stLoraFrame.LoraAck = 1;//需要应答填1
	stLoraFrame.LoraRadius = 0x07;
	stLoraFrame.LoraRoute = 0x01;//路由方式 01 自动路由
	stLoraFrame.LoraDateLen = (Len + 5);

	stLoraFrame.UserPacket.UserCmd = Cmd;
	if(uiConsoleFlag)
		stLoraFrame.UserPacket.UserSoursAddr = U16ConsoleAddr;
	else
		stLoraFrame.UserPacket.UserSoursAddr = U16MasterAddr;
	
	stLoraFrame.UserPacket.DataLen = Len;
	memcpy((UINT8 *)stLoraFrame.UserPacket.UserData,Buf,Len);
	
	stLoraFrame.UserPacket.UserData[Len] = CRC8((UINT8 *)&stLoraFrame.UserPacket.UserCmd,Len+4); //CRC
	stLoraFrame.UserPacket.UserData[Len +1 ] = CalcXor((UINT8*)&stLoraFrame.LoraType,(Len + 6+5+4));

	return LoraLowlevelSend(LoraCh,(UINT8*)&stLoraFrame,(Len+6+5+5));
}

//返回0 成功 ,其他失败
UINT8 LoraApi_Rcv(UINT8 * pRcvBuf, UINT8 uiRcvLen)
{
	stLoraRxStruct_Def *pLoraRecv = (stLoraRxStruct_Def *)pRcvBuf;
	UINT8 CMD;
	UINT8 Len;
	UINT16 SourAddr; 

	if(uiRcvLen > LORA_FRAME_LEN_MAX)//最大长度
		return 1;
		
	if(pLoraRecv->LoraType !=0x05)//数据类型
		return 1;

	if(pLoraRecv->LoraCmd !=0x82)
		return 1;
	
	if(pLoraRecv->LoraLen > LORA_FRAME_LEN_MAX)
		return 1;
/*	if(uiConsoleFlag)
	{
		if(Tranverse16(pLoraRecv->LoraDestAddr) != U16ConsoleAddr)
			return 1;
	}	
	else
	{
		if(Tranverse16(pLoraRecv->LoraDestAddr) != U16MasterAddr)
			return 1;
	}
*/	
	if(pLoraRecv->LoraDateLen > LORA_DATA_LEN_MAX)
		return 1;

	if(pLoraRecv->UserPacket.DataLen > USER_DATA_LEN_MAX)
		return 1;

	CMD = pLoraRecv->UserPacket.UserCmd;
	Len = pLoraRecv->UserPacket.DataLen;
	SourAddr = pLoraRecv->UserPacket.UserSoursAddr;

	if(pLoraRecv->UserPacket.UserData[Len ] != CRC8((UINT8 *)&pLoraRecv->UserPacket.UserCmd, Len+4))
		return 2;

//	if(CalcXor(pRcvBuf, uiRcvLen)!=0)
//		return 2;

/*	if(!uiConsoleFlag)
	{
		if(CMD == UNIT_CONFIGUNIT_WRITE_CMD)
		{
			stUnitConfigReq_Def *pUnitConfigReq = (stUnitConfigReq_Def *)pLoraRecv->UserPacket.UserData;
			if((pUnitConfigReq->ConsoleID)&&(U16MasterAddr == pUnitConfigReq->MasterID))
			{
				U16ConsoleAddr = pUnitConfigReq->ConsoleID;
			}
		}
	}
*/	
	return LoraDocmd( SourAddr,  CMD, (UINT8 *)pLoraRecv->UserPacket.UserData, Len);
	
}

#endif

//////////////////////////////////////////////////////////////////////////////////////
//红外点对点通信
//////////////////////////////////////////////////////////////////////////////////////

//发送
extern UINT8 RednoteLowlevelSend(UINT8 *Buf, UINT8 Len);
//解析
extern UINT8 RednoteDocmd(UINT8 Cmd, UINT8 *Buf, UINT8 Len);

UINT8 RedNote_Snd(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stRedNote_Def RedNote;

	if(Len > REDNOTE_DATA_LENMAX)
		return 1;
	
	RedNote.StartFlg = REDNOTE_FRAM_START;
	RedNote.Type = 0;//0x00-上行(设备->主机) 0x01 -下行(主机->设备)
	RedNote.Cmd = Cmd;
	RedNote.Len = Len;
	memcpy (RedNote.Data,Buf,Len);
	RedNote.Data[Len] = CRC8((UINT8*)&RedNote.Type,(Len + 3));
	RedNote.Data[Len+1] = REDNOTE_FRAM_END;
	RednoteLowlevelSend((UINT8*)&RedNote,(Len+REDNOTE_FRAM_LENDEF));
	return 0;
}
UINT8 RedNote_Rcv(UINT8 * pRcvBuf, UINT8 uiRcvLen)
{
	stRedNote_Def *RedNote = (stRedNote_Def *)pRcvBuf;
	if(uiRcvLen < REDNOTE_FRAM_LENDEF)
	{
		return 1;
	}
	if(uiRcvLen> (REDNOTE_FRAM_LENDEF + REDNOTE_DATA_LENMAX))
	{
		return 1;
	}
	if(RedNote->StartFlg != REDNOTE_FRAM_START)
	{
		return 1;
	}
	if(RedNote->Type != 1)//0x00-上行(设备->主机) 0x01 -下行(主机->设备)
	{
		return 1;
	}
		
	if((RedNote->Len + REDNOTE_FRAM_LENDEF) != uiRcvLen)
	{
		return 2;
	}
	if(RedNote->Data[RedNote->Len] != CRC8((UINT8 *)&RedNote->Type, (RedNote->Len +3)))
	{
		return 3;
	}
	return RednoteDocmd(RedNote->Cmd ,RedNote->Data,RedNote->Len);
	
}


#ifdef __cplusplus
}
#endif
