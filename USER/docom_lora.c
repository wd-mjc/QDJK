/*
@file     docom_lora.c
@brief    LORA  ͨ��Э�����( ������)
@author   mjc
@version  1.00
@create date     2018-11-05 10:00
@note
*/

#include <string.h>
#include "type_slv.h"
#include "type_com.h"
#include "commapi.h"
#include "setting_master.h"

#ifdef BOARD_MASTER
//////////////////////////////////////////////////////////////////////
//�ڲ�����
///////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
//�ⲿ����
///////////////////////////////////////////////////////////////////////

//���Ͷ�����
//�������ò�ͬ��ʱ2S ����
void UnitSendReadConfig(UINT8 Start,UINT8 Cnt)
{
	stUnitConfigRead_Def Config;
	Config.UnitStartNo =Start;
	Config.UnitCnt =Cnt;
	
	LoraApi_Snd(0,UNIT_CONFIGUNIT_READ_CMD,(UINT8 *)&Config,sizeof(stUnitConfigRead_Def));
}

//���Ͷ�Խ��ֵ
//�������ò�ͬ��ʱ2S ����
void UnitSendReadSetting(void)
{
	UINT8 BUF;
	
	LoraApi_Snd(0,UNIT_SETTING_READ_CMD,(UINT8 *)&BUF,0);
}


//������������բ��Ԫ
void UnitSendHeartTripUnit(void)
{
	stSampleResp_Def TripHeart;
	UINT16 TripAddr;
	
	TripHeart.Time = stMSysInfo.stSysVar.SysTime;	

	TripAddr = stMSysInfo.stSysVar.TripUnitAddrBuf[stMSysInfo.stSysVar.TripHeartSn];
	if(TripAddr & 0xff00)
		LoraApi_Snd(TripAddr,UNIT_SAMPLING_CMD,(UINT8 *)&TripHeart,sizeof(stSampleResp_Def));

	stMSysInfo.stSysVar.TripHeartSn ++;	//	ѭ������
	if(stMSysInfo.stSysVar.TripHeartSn >= stMSysInfo.stSysVar.TripUnitCnt)
		stMSysInfo.stSysVar.TripHeartSn = 0;

	//��բ��Ԫ�����ж�
	UnitAddrLinkLost(TripAddr);
}

//���Ͳ���ֵ
void UnitSamplingReq(void)
{
	UINT8 SendBuf[USER_DATA_LEN_MAX];
	stSampleReq_Def *pSampleReq = (stSampleReq_Def *)SendBuf;
	stUnit_Def *pUnit = &pSampleReq->pUnit;
	UINT8 cnt;
	UINT8 Len;
	
	cnt=stMSysInfo.stSysVar.AllUnitCnt  - stMSysInfo.stSysVar.SendUnitSn;
	cnt = (cnt > 1)?1:(cnt);
		

	pSampleReq->UnitCnt = cnt;
	Len = sizeof(stUnit_Def)* cnt;
	memcpy((UINT8*)pUnit,(UINT8*)&stMSysInfo.stSysVar.stUnit[stMSysInfo.stSysVar.SendUnitSn],Len);

	LoraApi_Snd(0,UNIT_SAMPLING_CMD,SendBuf,Len+1);

	//ѭ�����͵�Ԫ����������
	stMSysInfo.stSysVar.SendUnitSn += cnt;
	if(stMSysInfo.stSysVar.SendUnitSn >= stMSysInfo.stSysVar.AllUnitCnt)
		stMSysInfo.stSysVar.SendUnitSn = 0;
}

// ������բ
void UnitTripReq(stUserSlvTripReq_Def *pSlvTripReq,UINT16 SampAddr)
{
	stActiveReq_Def	stActiveReq;
	stUnit_Def *pUnit;

	pUnit = GetUnitByAddr(pSlvTripReq->TripUnitAddr);
	if(pUnit ==NULL)
		return ;

	stActiveReq.MasterID = stMSysInfo.stSetting.U16MasterAddr;
	stActiveReq.SampleID = SampAddr;
	stActiveReq.TripUnitID = pSlvTripReq ->TripUnitAddr;
	stActiveReq.TripDOBit =  pSlvTripReq ->TripBO;

	LoraApi_Snd(stActiveReq.TripUnitID,UNIT_SENDTRIP_CMD,(UINT8 *)&stActiveReq,sizeof(stActiveReq_Def));

	memcpy((UINT8 *)&stMSendTrip,(UINT8 *)&stActiveReq,sizeof(stActiveReq_Def));
	 stMSysInfo.stSysVar.TripSendCnt ++;
	 stMSysInfo.stSysVar.TripEvent =1;//��ѭ����ѯ

}
// ������բ�ط�
void ReSendUnitTripReq(stActiveReq_Def *pstActiveReq)
{
	stActiveReq_Def	stActiveReq;

	memcpy((UINT8 *)&stActiveReq,(UINT8 *)pstActiveReq, sizeof(stActiveReq_Def));

	LoraApi_Snd(stActiveReq.TripUnitID,UNIT_SENDTRIP_CMD,(UINT8 *)&stActiveReq,sizeof(stActiveReq_Def));

	 stMSysInfo.stSysVar.TripSendCnt ++;
	 if(stMSysInfo.stSysVar.TripSendCnt >= 3)
	 {
	 	stMSysInfo.stSysVar.TripSendCnt =0;			
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR =1;//�յ��ظ�����
		stMSysInfo.stSysVar.TripEvent = 0;//ֹͣ����
		stMSysInfo.stSysVar.TripRestCnt = TRIP_RESET_TIMEOUT;// 	��ʱ������������־
		//memset((UINT8 *)&stMSendTrip,0,sizeof(stActiveReq_Def)); //������ �¼���¼����ʹ��
	 }
}

// ����Խ���¼�
//type 1-�¶�Խ�� 2-����Խ�� 3.��բ4.���
/*void UnitEventReq(UINT16 Addr,UINT8 Type)
{
	stEventReq_Def stEventReq;
	stUnit_Def *pUnit;
	stUnitConfig_Def *pConfig;

	stEventReq.Time = stMSysInfo.stSysVar.SysTime;
	stEventReq.UnitType = 3;
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

	if(Type ==3)//��բ
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
	LoraApi_Snd(0,UNIT_SENDEVENT_CMD,(UINT8 *)&stEventReq,sizeof(stEventReq_Def));	
	
}*/

void UnitEventReq(stEventReq_Def *pEvent)
{
	stEventReq_Def stEventReq;
	memcpy((UINT8*)&stEventReq,pEvent,sizeof(stEventReq_Def));

	LoraApi_Snd(0,UNIT_SENDEVENT_CMD,(UINT8 *)&stEventReq,sizeof(stEventReq_Def));	
	
}

//////////////////////////////////////////////////////////////////////
//DOCOM �ڲ�����
///////////////////////////////////////////////////////////////////////

//�����ظ�
UINT8 UnitSamplingResp(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	if((SourAddr & 0xff00) == 0)//̨��
	{
		stSampleResp_Def *pSampleResp = (stSampleResp_Def *)pRcvBuf;
		stMSysInfo.stSysVar.SysTime = pSampleResp->Time;

		stMSysInfo.stSysVar.ConfigVersion = pSampleResp->ConfigVersion;
		stMSysInfo.stSysVar.SettingVersion = pSampleResp->SettingVersion;

		stMSysInfo.stSysVar.HeartCnt =0;
		if(stMSysInfo.stSysVar.SysWorkMod ==0)
			stMSysInfo.stSysVar.SysWorkMod = 1;
		return 0;
	}
	else//��բ��Ԫ
	{
		stUnit_Def *pUnit;	
		stUnitVar_def * UnitVar;
		stSampleReq_Def *HeartResp = (stSampleReq_Def *)pRcvBuf;
		stUnit_Def *pUnitResp = &HeartResp->pUnit;
		
		if(uiRcvLen != (sizeof(stUnit_Def)+1))
			return 1;

		pUnit = GetUnitByAddr(SourAddr);
		UnitVar = GetVarByAddr(SourAddr);
		if(pUnit ==NULL)
			return 1;
		if(UnitVar ==NULL)
			return 1;

		if(pUnitResp->UnitType!=4)//��բ��Ԫ
			return 1;

		pUnit->Status.uiAll = pUnitResp->Status.uiAll;
		pUnit->ArcOrDO = pUnitResp->ArcOrDO;
		pUnit->Version = pUnit->Version; 
		UnitVar->HeartLostCnt = 0; //���������ʧ����
	}
        return 0;
}
//��բ�ظ�
UINT8 UnitTripResp(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stMSysInfo.stSysVar.TripEvent =0;//���ѭ������
	stMSysInfo.stSysVar.TripSendCnt =0;
	stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SENDTRIP_ERROR =0;
	return 0;
}
//�¼��ظ�
UINT8 UnitEventResp(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////

UINT8 UnitUpdataFileReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,pRcvBuf,0);
}

UINT8 UnitSettingReadReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stLimitReq_Def stLimitReq;
	
	memcpy((UINT8 *)&stLimitReq,(UINT8 *)&stMSysInfo.stSetting.stLimit,sizeof(stLimitReq_Def));
	return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,(UINT8 *)&stLimitReq,sizeof(stLimitReq_Def));
}


//�޸�Ϊ������������ȡԽ��ֵ
UINT8 UnitSettingWriteReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stLimitReq_Def *stLimitReq = (stLimitReq_Def *)pRcvBuf;
	UINT8 res=0;

	if(stLimitReq->TempLimit < TEMP_OVER_SETTING_MIN)
		res =1;
	if(stLimitReq->ArcLimit < ARC_OVER_SETTING_MIN)
		res =1;
	
	if(res)
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 1;
		return 1;
		//return LoraApi_Snd(SourAddr,CMD|UNIT_ERR_CMD_MASK,(UINT8 *)stLimitReq,0);
	}
	else
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_SETTING_WARN = 0;
	}
	memcpy((UINT8 *)&stMSysInfo.stSetting.stLimit,(UINT8 *)stLimitReq,sizeof(stLimitReq_Def));
	stMSysInfo.stSysVar.SaveFalshCfg = 1;//��Ԫ�������
	return 0;
	//return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,(UINT8 *)stLimitReq,0);
}

UINT8 UnitConfigReadReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUnitConfigRead_Def *pConfigRead = (stUnitConfigRead_Def *)pRcvBuf;
	UINT8 SendBuf[USER_DATA_LEN_MAX];
	stUnitConfigReq_Def *pConfigReq = (stUnitConfigReq_Def  *)SendBuf;
	stUnitConfig_Def *pConfig =pConfigReq->pUint;
	UINT8 res=0;
	UINT8 Len =0;

	if(pConfigRead->UnitStartNo > stMSysInfo.stSetting.SampCnt)
		res =1;
	if(pConfigRead->UnitStartNo+ pConfigRead->UnitCnt > stMSysInfo.stSetting.SampCnt)
		res =1;
	if(pConfigRead->UnitCnt > 3)
		res =1;
	
	if(res)
		return LoraApi_Snd(SourAddr,CMD|UNIT_ERR_CMD_MASK,SendBuf,0);
	
	memcpy((UINT8 *)pConfigReq,(UINT8 *)&stMSysInfo.stSetting.stConfig,sizeof(stUnitConfigReq_Def));
	Len = sizeof(stUnitConfig_Def) * pConfigRead->UnitCnt;
	memcpy((UINT8 *)pConfig,(UINT8 *)&stMSysInfo.stSetting.stSampUnit[pConfigRead->UnitStartNo],Len);
	
	Len += sizeof(stUnitConfigReq_Def) - sizeof(stMSysInfo.stSetting.stConfig.pUint);
	return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,SendBuf,Len);
}

//�޸�Ϊ������������ȡ����
UINT8 UnitConfigWriteReq(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	stUnitConfigReq_Def *pConfigReq = (stUnitConfigReq_Def  *)pRcvBuf;
	stUnitConfig_Def *pConfig =(stUnitConfig_Def *)&pConfigReq->pUint;

	UINT8 res=0;
	UINT8 Len =0;

	if(pConfigReq->UnitConfigMax > SAMPLING_UNIT_CNT_MAX)
		res =1;
	if(pConfigReq->UnitStartNo + pConfigReq->UnitCnt > SAMPLING_UNIT_CNT_MAX)
		res = 1;
	if(pConfigReq->UnitCnt > 3)
		res =1;

	if(pConfigReq->MasterID != stMSysInfo.stSetting.U16MasterAddr)// ���Ǳ���Ԫ
		res =1;

	if(res)
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 1;
		return 1;
		//return LoraApi_Snd(SourAddr,CMD|UNIT_ERR_CMD_MASK,pRcvBuf,0);
	}
	else
	{
		stMSysInfo.stSysVar.SysStatus.bit.uiSTATUS_CONFIG_WARN = 0;
	}

	memcpy((UINT8 *)&stMSysInfo.stSetting.stConfig,(UINT8 *)pConfigReq,sizeof(stUnitConfigReq_Def));
	Len = sizeof(stUnitConfig_Def) * pConfigReq->UnitCnt;
	memcpy((UINT8 *)&stMSysInfo.stSetting.stSampUnit[pConfigReq->UnitStartNo],(UINT8 *)pConfig,Len);


	stMSysInfo.stSysVar.SampUnitCntTmp += pConfigReq->UnitCnt;
	stMSysInfo.stSysVar.ReadConfigIndex += pConfigReq->UnitCnt; //�յ����
	if(stMSysInfo.stSysVar.ReadConfigIndex >= stMSysInfo.stSetting.stConfig.UnitConfigMax)
	{
		stMSysInfo.stSysVar.ReadConfigIndex = 0;
		stMSysInfo.stSysVar.ReadConfigFlag = 0;//�ָ�����
		stMSysInfo.stSysVar.SaveFalshCfg = 1;//��Ԫ�������
	}
	if(stMSysInfo.stSysVar.SampUnitCntTmp >= stMSysInfo.stSetting.stConfig.UnitConfigMax)
	{
		stMSysInfo.stSysVar.SampUnitCntTmp = 0;
		stMSysInfo.stSysVar.SaveFalshCfg = 1;//��Ԫ�������
	}
	if( pConfigReq->UnitStartNo + pConfigReq->UnitCnt >= stMSysInfo.stSetting.stConfig.UnitConfigMax)
	{
		stMSysInfo.stSysVar.SampUnitCntTmp = 0;
		stMSysInfo.stSysVar.SaveFalshCfg = 1;//��Ԫ�������
	}
	return 0;
	//return LoraApi_Snd(SourAddr,CMD|UNIT_RESP_CMD_MASK,pRcvBuf,Len);
}



// ��Ԫ������docom LORAЭ��
UINT8 LoraDocmd(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen)
{
	UINT8 Res = 0;
	
	//stMSysInfo.stSetting.U16ConsoleAddr = SourAddr;
	if(CMD& UNIT_ERR_CMD_MASK)//�Ǵ�����
	{
		return 1;
	}
	
	if(CMD& UNIT_RESP_CMD_MASK)//��Ӧ����
	{  
		switch( CMD & UNIT_CMD_MASK )
		{
		case UNIT_SAMPLING_CMD : //����         
			Res=UnitSamplingResp(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_SENDTRIP_CMD : //��բ                        
			Res=UnitTripResp(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_SENDEVENT_CMD : //����                       
			Res=UnitEventResp(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_CONFIGUNIT_READ_CMD : //�����ûظ�       
			Res=UnitConfigWriteReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_SETTING_READ_CMD : //����ֵ�ظ�
			Res=UnitSettingWriteReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 

		default :  break;
		}
	}
	else
	{
		switch( CMD)//̨��д����
		{
		case UNIT_UPDATA_CMD : //����                
			Res=UnitUpdataFileReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		/*case UNIT_SETTING_READ_CMD : //����ֵ
			Res=UnitSettingReadReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_SETTING_WRITE_CMD : //д��ֵ
			Res=UnitSettingWriteReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_CONFIGUNIT_READ_CMD : //������       
			Res=UnitConfigReadReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		case UNIT_CONFIGUNIT_WRITE_CMD : //д����       
			Res=UnitConfigWriteReq(SourAddr,CMD,pRcvBuf,uiRcvLen);
		break; 
		*/
		default :  break;
		}
	}	
	return Res;
}


#endif

 