#ifndef		_COMM_API_H
#define		_COMM_API_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//Z�ڱ�����ѡ����ѡ��     
//BOARD_SAMP //�ɼ���
//BOARD_TRIP //��բ��Ԫ
//BOARD_MASTER  //������ 
 
    
extern void CanInit_Api(UINT8 uiUnitAttr,UINT8 uiUnitAddr,UINT8 uiMaAddr);

extern UINT8 CanApi_Snd(UINT8 DestAddr,UINT8 *Buf, UINT8 Len);

extern UINT8 CanApi_Rcv(UINT8 SourAddr,UINT8 * pRcvBuf, UINT8 uiRcvLen);


extern void LoraInit_Api(UINT8 uiUnitAttr,UINT16 uiUnitAddr,UINT16 uiConsoleAddr);

extern UINT8 LoraApi_Snd(UINT16 DestAddr,UINT8 Cmd,UINT8 *Buf, UINT8 Len);

extern UINT8 LoraApi_Rcv(UINT8 * pRcvBuf, UINT8 uiRcvLen);


extern UINT8 RedNote_Snd(UINT8 Cmd,UINT8 *Buf, UINT8 Len);
extern UINT8 RedNote_Rcv(UINT8 * pRcvBuf, UINT8 uiRcvLen);


#ifdef __cplusplus
}
#endif

#endif
