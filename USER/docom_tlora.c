/*
@file     docom_tlora.c
@brief    CAN  ͨ��Э����� (��բ��Ԫ)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/

#include <string.h>
#include "type_slv.h"
#include "type_com.h"
#include "commapi.h"
#include "setting_trip.h"

#ifdef BOARD_TRIP

//////////////////////////////////////////////////////////////////////
//DOCOM �ڲ�����
///////////////////////////////////////////////////////////////////////

UINT8 tHeartReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stSampleResp_Def *pHeart = (stSampleResp_Def *)pRcvBuf;
	UINT8 SendBuf[USER_DATA_LEN_MAX];
	stSampleReq_Def *pHeartResp; 
	stUnit_Def *pUnit;
	UINT8 Len;

	stTSysInfo.stSysVar.Time = pHeart->Time;

	AddMasterAddr(SourAddr);

	pHeartResp = (stSampleReq_Def *)SendBuf;
	
	pHeartResp->UnitCnt =1;
	pUnit = &pHeartResp->pUnit;

	pUnit->UnitType = 4;
	pUnit->UnitID = stTSysInfo.stSetting.TripAddr;
	pUnit->Status.uiAll =  stTSysInfo.stSysVar.SysStatus.uiAll;
	pUnit->ArcOrDO = stTSysInfo.stSysVar.TripDo;
	pUnit->Version = stTSysInfo.stSysVar.Version;
	Len= sizeof(stUnit_Def)+1;
	
	return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,SendBuf,Len);
}


UINT8 tUpdataFileReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,pRcvBuf,0);
}

UINT8 tUnitTripReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stActiveReq_Def *pTrip = (stActiveReq_Def *)pRcvBuf;

	if(uiRcvLen!=sizeof(stActiveReq_Def))
		return 1;

	if(pTrip->TripUnitID != stTSysInfo.stSetting.TripAddr)
		return 1;

	if(!CheckMasterAddr(pTrip->MasterID))//�����������ӵĵ�ַ
		return 1;
	//������բ 
	SetTripChDelay(pTrip->TripDOBit);

	//�ظ�
	LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,pRcvBuf,0);
	

	//stTSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 1;
	//stTSysInfo.stSysVar.TripDo |= pTrip->TripDOBit;	
	
	return 0;
}


// ��բ��Ԫdocom
UINT8 LoraDocmd(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	UINT8 Res = 0;
	//if(SourAddr != stTSysInfo.stSetting.SampAddr)
	//	return 1;
	if(CMD& UNIT_ERR_CMD_MASK)//�Ǵ�����
	{
		return 1;
	}
	if(CMD & UNIT_RESP_CMD_MASK)//��Ӧ����
	{  
		switch( CMD & UNIT_CMD_MASK )
		{
		case UNIT_SAMPLING_CMD : //����  
		break; 

		default :  break;
		}
	}
	else
	{
		switch( CMD)//����Ԫ��������
		{
		case UNIT_SAMPLING_CMD : //����             
			Res=tHeartReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_SENDTRIP_CMD : //��բ����                 
			Res=tUnitTripReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_UPDATA_CMD : //����                 
			Res=tUpdataFileReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
	
		default :  break;
		}
	}	
	return Res;
}

#endif


 