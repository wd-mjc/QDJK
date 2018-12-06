#include <string.h>
#include "type_com.h"
#include "commapi.h"

#include "setting_master.h"
#include "setting_trip.h"
#include "setting_samp.h"



//////////////////////////////////////////////////////////////////////
//DOCOM 内部函数
///////////////////////////////////////////////////////////////////////
UINT8 UnitWriteAddrReq(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stUnitSetNetAddr_Def *SetNetAddr = (stUnitSetNetAddr_Def *)Buf;
	
	if(sizeof(stUnitSetNetAddr_Def)!= Len)
		return 1;

	//设置地址
	WriteUnitAddr(SetNetAddr);

	return RedNote_Snd(Cmd|UNIT_RESP_CMD_MASK,Buf,0);
	
}
UINT8 UnitReadAddrReq(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stUnitSetNetAddr_Def NetAddr;

	ReadUnitAddr((stUnitSetNetAddr_Def *)&NetAddr);

	return RedNote_Snd(Cmd|UNIT_RESP_CMD_MASK,(UINT8 *)&NetAddr,sizeof(stUnitSetNetAddr_Def));
}

UINT8 UnitReadSettingReq(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stUnitSettingCfg_Def SettingCfg;

	ReadSettingCfg((stUnitSettingCfg_Def *)&SettingCfg);

	return RedNote_Snd(Cmd|UNIT_RESP_CMD_MASK,(UINT8 *)&SettingCfg,sizeof(stUnitSetNetAddr_Def));
}

UINT8 UnitReadDbgReq(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stUnitReadDbg_Def stDbg;

	ReadUnitDbg((stUnitReadDbg_Def *)&stDbg);

	return RedNote_Snd(Cmd|UNIT_RESP_CMD_MASK,(UINT8 *)&stDbg,sizeof(stUnitReadDbg_Def));
}


// 红外docom
UINT8 RednoteDocmd(UINT8 Cmd, UINT8 *Buf, UINT8 Len)
{
	UINT8 Res = 0;

	if(Cmd & UNIT_ERR_CMD_MASK)
		return 1;

	if(Cmd & UNIT_RESP_CMD_MASK)//是应答报文
	{  
		return 1;
	}
	else
	{
		switch( Cmd )//管理单元写写设置
		{
		case UNIT_ADDR_WRITE_CMD : //写               
			Res=UnitWriteAddrReq(Cmd,Buf,Len);
		break; 
		case UNIT_ADDR_READ_CMD : //读      
			Res=UnitReadAddrReq(Cmd,Buf,Len);
		break; 
#ifdef BOARD_SAMP		
		case UNIT_SETTING_CFG_CMD : //读  配置    
			Res=UnitReadSettingReq(Cmd,Buf,Len);
		break; 
#endif		
		
		case UNIT_READ_DBG_CMD : //读dbg      
			Res=UnitReadDbgReq(Cmd,Buf,Len);
		break; 
		default :  break;
		}
	}	
	return Res;
}




 