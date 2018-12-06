#ifndef TYPE_SLV_H_
#define TYPE_SLV_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
/////////////Э��///////////
//ʹ��CAN2.0BЭ�����չ֡��ʽ
//1   11  1   1  18   1   1  1  4  0~64  16  2  7
//SOF ID SRR IDE EID RTR  1  0 DLC DATA CRC ACK EOF
//DEST ADDR ʹ��ID�ĺ�8λ
//SOUR ADDR ʹ��EID�ĺ�8λ
//
//��Ԫ������ADDR 16λ  ��8λΪ��Ԫ��ţ���8λΪ0
//�ɼ���ADDR 16λ ��8λ���������8λ��ͬ����8λ��1��255���
//��Ԫ�������������� DEST ʹ�òɼ�����ţ�SOUR ʹ�ø�8λ ����ģʽ���� ֻ���ո�8λ��ͬ����֡
//�ɼ����������� DEST  ʹ�ø�8λ��SOUR ʹ�òɼ������    ����ģʽ�������ú��޸�Ϊ��ʶ���б�ģʽ
*/

#include "common.h"
#include "type_com.h"

//������궨��

//�¶Ȳ���ֵ����
#define SLV_SAMP_TEMP_CMD 0X01
//�������ֵ����
#define SLV_SAMP_ARC_CMD 0X02
//��բ����
#define SLV_SEND_TRIP_CMD 0X03
//�¼�����
#define SLV_SEND_EVENT_CMD 0X04
//�߼�����
#define SLV_LOGIC_SET_CMD 0X05
//��ֵ����
#define SLV_SETTING_SET_CMD 0X06
//������������
#define SLV_ZERO_SET_CMD 0X07
//��������������
#define SLV_MASTER_SET_CMD 0X08

/////////////////////////////////////////////////////////////////////////////
//���б��ĵ���ȷ�ظ���ԭ���������UNIT_RESP_CMD_MASK
//���б��ĵĴ���ظ���ԭ���������UNIT_ERR_CMD_MASK
/////////////////////////////////////////////////////////////////////////////
#define SLV_RESP_CMD_MASK 0X80
#define SLV_ERR_CMD_MASK 0X40
#define SLV_CMD_MASK 0x1F



#define ARC_RADIO_MAX (1000) // 0.1 * 10000
#define TEMP_SETTING_MIN (TEMP_OVER_SETTING_MIN) //�Ŵ�10��
#define ARC_SETTING_MIN (ARC_OVER_SETTING_MIN) //�Ŵ�10��

#define LOGIC_ARC_TEMPOR 0 //���� + �¶�
#define LOGIC_TEMPOR 1 //���¶Ȼ��ϵ
#define LOGIC_ARCONLY 2 // ������
#define LOGIC_ARC_TEMP_INPUT 3 //�¶�+����+����
#define LOGIC_TEMPAND 4//���¶����ϵ

#define TRIP_FAILE_TIMEOUT (20) //��λ��
#define TRIP_RESET_TIMEOUT (60) //��λ��
#define FIRE_RESET_TIMEOUT (60) //��λ��
#define TRIP_RESET_FLAG_TIMEOUT (60) //��λ��
#define FIRE_RESET_FLAG_TIMEOUT (60) //��λ��

#define FIRE_RELAY_DELAYCNT (2)	// 2s��̵�������


/*
/////////////�ɼ�����������///////////
//��FLASH,��ȡ������master��ַ�����ޣ��״Σ��ȴ����������á�
//���У��ж϶�ֵ�Ƿ���ȷ����ȷ��������������ģʽ�����󣬽�ȥ����ģʽ�����Ͷ�ֵ������Ϣ���ȴ��������á�
/////////////���ù�������///////////
//1.��Ԫ���������͹����������� cmd = 0x07
//2.�ɼ����ظ��汾  cmd = 0x87
//3.��Ԫ�����������߼� cmd = 0x05  ����3�� ���1s �޻ظ���1
//4.�ɼ����ظ� cmd = 0x85
//5.��Ԫ���������Ͷ�ֵ cmd = 0x06  ����3�� ���1s �޻ظ���1
//6.�ɼ����ظ� cmd = 0x86
//7.��Ԫ��������������==>��ʱ���Ͳ�������
*/

#pragma pack(1)



//////////////////////////////////////////////////////////////////
////////////////////////�¼�����/////////////////////////////
//////////////////////////////////////////////////////////////////
//��¼�¼�����
typedef union
{
	UINT8 uiAll;
	struct
		{
		/*bit  0*/ UINT8 uiEventType_Record:1; //�޲���
		/*bit  1*/ UINT8 uiEventType_Over:1; //Խ�޼�¼
		/*bit  2*/ UINT8 uiEventType_Input:1; //�����λ
		/*bit  3*/ UINT8 uiEventType_Trip:1; //������¼
		/*bit  4*/ UINT8 uiEventType_Fire:1; //����¼
		/*bit  5*/ UINT8 uiEventType_Res:3; //
		/*bit  6*/ 
	}bit;
}unEventType;

//�޲�������data1���� data2 =0
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiEvent_Record_FLASHERR:1; //FALSH �����¼
		/*bit  1*/ UINT16 uiEvent_Record_ADCERR:1; //ADC�����¼
		/*bit  2*/ UINT16 uiEvent_Record_COMLINK:1; //ͨ������
		/*bit  3*/ UINT16 uiEvent_Record_COMLOST:1; //ͨ�ŶϿ�
		/*bit  4*/ UINT16 uiEvent_Record_SETERR:1; //���ô���
		/*bit  5*/ UINT16 uiEvent_Record_SENDTRIPERR:1;//������բͨ�Ŵ���
		/*bit  6*/ UINT16 uiEvent_Record_SENDEVENTERR:1;//�����¼�ͨ�Ŵ���
		/*bit  7*/ UINT16 uiEvent_Record_TripFailed:1;//��բʧ�� û�м�⵽�����ź�
		/*bit  7*/ UINT16 uiEvent_Record_Res:8; //
	}bit;
}unEventRecordData1;

//Խ�޼�¼����data1 = ͨ�� data2 = ����ֵ
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiEvent_Over_TEMP1:1; //�¶�ͨ��1
		/*bit  1*/ UINT16 uiEvent_Over_TEMP2:1; //�¶�ͨ��2
		/*bit  2*/ UINT16 uiEvent_Over_TEMP3:1; //�¶�ͨ��3
		/*bit  3*/ UINT16 uiEvent_Over_ARC:1; //����
		/*bit  4*/ UINT16 uiEvent_Over_Res:12; //
	}bit;
}unEventOverData1;
//�����λ��¼����data1=����ֵ

//��բ��¼����data1=�¶�ͨ�� data2 = �¶�ֵ data3 =����ֵ

//����¼����data1=�¶�ͨ�� data2 = �¶�ֵ data3 =����ֵ

typedef union
{
	UINT16 Data1;
	unEventRecordData1 RecordData1;
	unEventOverData1 OverData1;
	UINT16 InputData1;
	UINT16 TempChData1;
}unEventData1;

typedef union
{
	UINT16 Data2;
	UINT16 TempData2;
	UINT16 ArcData2;
}unEventData2;

typedef union
{
	UINT16 Data3;
	UINT16 ArcData3;
}unEventData3;

/////////////////////////////////////////////////////////////////////////////////////




//�����¶�
typedef struct
{
	UINT8 UserSlvCmd; // ������
	UINT8   Version; // ��4λ����4λ
	SINT16  SampTemp[3]; // �¶ȣ�  ���ݷŴ�10��������ͨ��˳����
}stUserSlvSampTemp_Def;

//���ͻ���
typedef struct
{
	UINT8 UserSlvCmd; // ������
	UINT8   CfgVersion; // ��4λ����4λ ��Ԫ�������ж��汾��һ�� (CONFIG + SETTING)
	unUnitStatus_Def SlvStatus; // ״̬
	UINT16  SampArc;	     // ���⣻   ���ݷŴ�10��
	UINT8   SampIO;       // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����
	
}stUserSlvSampArc_Def;

//�ظ�ʱ��
typedef struct
{
	UINT8 UserSlvCmd; // ������
	UINT32  Time;  // �ظ�ʱ�䣬ʵ�ֶ�ʱ�� 
	UINT8   ConfigVersion;// ��Ԫ�������ظ� ���ð汾 
	UINT8   SettingVersion;//��Ԫ�������ظ�Խ�ް汾  
}stUserSlvSampResp_Def;

//���Ͷ���
typedef struct
{
	UINT8 UserSlvCmd; // ������
	UINT16 TripUnitAddr;  // ��բ��������ַ
	UINT16 TripBO;      // ��բ��ԪIO BIT 
}stUserSlvTripReq_Def;

//�����ظ�
typedef struct
{
	UINT8 UserSlvCmd; // ������
}stUserSlvTripResp_Def;

//�¼�
typedef struct
{
	UINT8 UserSlvCmd; // ������
	unEventType EventType;//�������ݽ�����ʽ
	unEventData1 Data1;
	unEventData2 Data2;
	unEventData3 Data3;
}stUserSlvEventReq_Def;

//�¼��ظ�
typedef struct
{
	UINT8 UserSlvCmd; // ������
}stUserSlvEventResp_Def;



//�߼�����
typedef struct
{
	UINT8   UserSlvCmd; // ������ 05
	UINT8   OutFireType;    // ����������߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   TripType;    // ��բ�����߼��� 0���¶�+ ����  1���¶ȣ�2������ 3.�¶�+ ���� +������
	UINT8   LocalTripType;//
	UINT16  TripAddr;   //��բ��Ԫ��ַ
	UINT16   TripBO; // ��բ��ԪIO 1~16
}stUserSlvLogicReq_Def;
//��ֵ����
typedef struct
{
	UINT8 UserSlvCmd; // ������ 06
	UINT8   InputBit; //����λ��
	UINT16  TempLimit;  // �¶�Խ�޶�ֵ��	�Ŵ�10��
	UINT16  ArcLimit;   // ����Խ�޶�ֵ��         �Ŵ�10��
	UINT8   TempDelay; // �¶���ʱ����λS
	UINT8   ArcDelay; // ������ʱ����λS

}stUserSlvSettingReq_Def;

//�߼��ظ�
typedef struct
{
	UINT8 UserSlvCmd; // ������
}stUserSlvLogicResp_Def;

//��������
typedef struct
{
	UINT8 UserSlvCmd; // ������  07
	SINT8  TempRadio[3];//[3];  ��// �¶Ȳ������� �Ŵ�10��
	UINT16   ArcZero;       // ������Ư
	UINT16   ArcRadio;       // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
}stUserSlvZeroReq_Def;
//���ûظ�
typedef struct
{
	UINT8 UserSlvCmd; // ������

}stUserSlvZeroResp_Def;

typedef struct
{
	UINT8 UserSlvCmd; // ������  08
	UINT8  MasterAddr;// ��Ԫ��������ַ
	UINT8   AlrmTempPrec;// �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   AlrmArcPrec;  // �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   CfgVersion; // ��4λ����4λ ��ֵ�汾  (CONFIG + SETTING)
}stUserSlvMasterIdReq_Def;
//���ûظ�
typedef struct
{
	UINT8 UserSlvCmd; // ������
	UINT8 SettingReq;//�ɼ�������CfgVersion��һ�£��ظ�����ֵ
}stUserSlvMasterIdResp_Def;


#ifdef __cplusplus
}
#endif

#endif 
