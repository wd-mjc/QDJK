/*
@file     commapi.c
@brief    CAN LORA ͨ��API
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
//CAN ͨ��
//////////////////////////////////////////////////////////////////////////////////////

//����ID ��EID ��������
extern UINT8 CanLowlevelSend(UINT8 DestAddr,UINT8 SourAddr,UINT8 *Buf, UINT8 Len);
//����BUF����
extern UINT8 CanDocmd(UINT8 SourAddr,UINT8 *pRcvBuf, UINT8 uiRcvLen);
	
UINT8 uiSampAddr ;
UINT8 uiMasterAddr;
UINT8 uiMasterFlag;

//uiUnitAttr = 1 Ϊ��Ԫ������
//uiUnitAttr = 0 Ϊ�ɼ���
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

//�ɼ���DestAddr ��д0 
//������DestAddr ��д�ɼ������
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
		if(pRcvBuf[0] == SLV_MASTER_SET_CMD)//���ù�������ַ
			uiMasterAddr = 	pRcvBuf[1]; 	//���޸ı�ʶ���б�ŵ�docom�д���
		return CanDocmd(SourAddr,pRcvBuf, uiRcvLen);
	}
}
#endif

#if defined (BOARD_MASTER)||defined (BOARD_TRIP)
//////////////////////////////////////////////////////////////////////////////////////
//LORA ͨ��
//////////////////////////////////////////////////////////////////////////////////////

//LORA��������
extern UINT8 LoraLowlevelSend(UINT8 LoraCh,UINT8 *Buf, UINT8 Len);
//����BUF����
extern UINT8 LoraDocmd(UINT16 SourAddr,UINT8 CMD,UINT8 *pRcvBuf, UINT8 uiRcvLen);


UINT16 U16ConsoleAddr ;//̨����ַ( �̶�) 0x00XX
UINT16 U16MasterAddr;//��Ԫ��������ַ0xXX00
UINT8 uiConsoleFlag;

stLoraStruct_Def stLoraFrame;

//uiUnitAttr = 0 ��Ԫ������uiUnitAddr=����Ԫ��ַuiConsoleAddr=̨����ַ
//uiUnitAttr = 1 ̨��                uiUnitAddrδ��                   uiConsoleAddr=̨����ַ
//uiUnitAttr = 2 ��բ��Ԫ     uiUnitAddr=����Ԫ��ַuiConsoleAddrδ��  
void LoraInit_Api(UINT8 uiUnitAttr,UINT16 uiUnitAddr,UINT16 uiConsoleAddr)
{
	if(uiUnitAttr ==1)//̨��
	{		
		U16MasterAddr = 0;// �������ļ�
		U16ConsoleAddr =uiConsoleAddr;
		uiConsoleFlag = 1;
	}
	else if(uiUnitAttr ==2)//��բ��Ԫ
	{
		U16MasterAddr = uiUnitAddr;
		U16ConsoleAddr =0;
		uiConsoleFlag = 0;
	}
	else// ��Ԫ������
	{
		U16MasterAddr = uiUnitAddr;
		U16ConsoleAddr =uiConsoleAddr;//�����������л�ȡ
		uiConsoleFlag = 0;
	}
}

//��Ԫ������DestAddr ��д0 
//̨��DestAddr ��д��Ӧ��Ԫ��������ַ
//����0 �ɹ� ,����ʧ��
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
	stLoraFrame.LoraAck = 1;//��ҪӦ����1
	stLoraFrame.LoraRadius = 0x07;
	stLoraFrame.LoraRoute = 0x01;//·�ɷ�ʽ 01 �Զ�·��
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

//����0 �ɹ� ,����ʧ��
UINT8 LoraApi_Rcv(UINT8 * pRcvBuf, UINT8 uiRcvLen)
{
	stLoraRxStruct_Def *pLoraRecv = (stLoraRxStruct_Def *)pRcvBuf;
	UINT8 CMD;
	UINT8 Len;
	UINT16 SourAddr; 

	if(uiRcvLen > LORA_FRAME_LEN_MAX)//��󳤶�
		return 1;
		
	if(pLoraRecv->LoraType !=0x05)//��������
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
//�����Ե�ͨ��
//////////////////////////////////////////////////////////////////////////////////////

//����
extern UINT8 RednoteLowlevelSend(UINT8 *Buf, UINT8 Len);
//����
extern UINT8 RednoteDocmd(UINT8 Cmd, UINT8 *Buf, UINT8 Len);

UINT8 RedNote_Snd(UINT8 Cmd,UINT8 *Buf, UINT8 Len)
{
	stRedNote_Def RedNote;

	if(Len > REDNOTE_DATA_LENMAX)
		return 1;
	
	RedNote.StartFlg = REDNOTE_FRAM_START;
	RedNote.Type = 0;//0x00-����(�豸->����) 0x01 -����(����->�豸)
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
	if(RedNote->Type != 1)//0x00-����(�豸->����) 0x01 -����(����->�豸)
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
