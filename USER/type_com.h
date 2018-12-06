#ifndef TYPE_COMMON_H_
#define TYPE_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
//������̨���͵�Ԫ��������ͨ��
//ͨ�ŷ�ʽLORA 
//�ļ���������������IR����ͨ��
//��բ����ܵ�Ԫ����������բ��Ԫ��LORA�򴮿�
//̨��������������ʹ����̨���ͷ�����
/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
//    YL-800MN-100MW   Ӧ������֡��ʽ
//    ֡��    ֡�� ����  ����  Ŀ�� ACK �� ���� ·��  ����  �û� У��
//    ��         ��       ����  ����  ��ַ  ��      �뾶 ��ʽ  ����  ����
//    1byte      1byte     1byte     1byte      2byte    1byte     1byte    1byte    1byte     N byte   1byte
//    ֡ͷ                                      |  ֡����                                                       | XOR
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
//̨���ϵ繤������

/////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"

//������궨��

//����ֵ����
#define UNIT_SAMPLING_CMD 0X01
//��բ����
#define UNIT_SENDTRIP_CMD 0X02
//�¼�����
#define UNIT_SENDEVENT_CMD 0X03
//������������
#define UNIT_UPDATA_CMD 0X04
//��Խ��ֵ����
#define UNIT_SETTING_READ_CMD 0X05
//дԽ��ֵ����
//#define UNIT_SETTING_WRITE_CMD 0X06
//����������
#define UNIT_CONFIGUNIT_READ_CMD 0X07
//д��������
//#define UNIT_CONFIGUNIT_WRITE_CMD 0X08

//#define UNIT_CONFIGEVENT_READ_CMD 0X09
//#define UNIT_CONFIGEVENT_WRITE_CMD 0X0A

//#define UNIT_NETWORK_RED_CMD 0X0B
//#define UNIT_NETWORK_WRITE_CMD 0X0C

//д��ַ
#define UNIT_ADDR_WRITE_CMD 0X0D
//����ַ
#define UNIT_ADDR_READ_CMD 0X0E
//���ɼ�������
#define UNIT_SETTING_CFG_CMD 0X0F

//��������Ϣ
#define UNIT_READ_DBG_CMD 0X11

/////////////////////////////////////////////////////////////////////////////
//���б��ĵ���ȷ�ظ���ԭ���������UNIT_RESP_CMD_MASK
//���б��ĵĴ���ظ���ԭ���������UNIT_ERR_CMD_MASK
/////////////////////////////////////////////////////////////////////////////
#define UNIT_RESP_CMD_MASK 0X80
#define UNIT_ERR_CMD_MASK 0X40
#define UNIT_CMD_MASK 0x1F

/////////////////////////////////////////////////////////////////////////////
//LORAģ�鷢��Ӧ�����ݳ��ȶ���Ϊ50�ֽ�
/////////////////////////////////////////////////////////////////////////////
#define USER_DATA_LEN_MAX (50)
#define LORA_DATA_LEN_MAX (USER_DATA_LEN_MAX +5)
#define LORA_FRAME_LEN_MAX (USER_DATA_LEN_MAX+16)
#define LORARX_FRAME_LEN_MAX (USER_DATA_LEN_MAX+29)//���ճ���

//̨��������500����Ԫ������
#define MASTER_UNIT_CNT_MAX (500)
//��Ԫ������������50���ɼ���Ԫ
#define SAMPLING_UNIT_CNT_MAX (50)

//����ʱ������λs
#define HEART_INTV_TIMESEC_MAX (10) 
//������ʱ����
#define HEART_OVER_TIMESEC_MAX (60) 
//�¶���ʱ��λs
#define TEMP_DELAY_TIMESEC_MAX (600) 
#define TEMP_DELAY_TIMESEC_MIN (10) 
//������ʱ��λs
#define ARC_DELAY_TIMESEC_MAX (240) 
#define ARC_DELAY_TIMESEC_MIN (10) 
//�¶�Խ��ֵ
#define TEMP_OVER_SETTING_MIN (100) //ֵ�ѷŴ�10��
#define TEMP_OVER_SETTING_DEF (100)
//����Խ��ֵ
#define ARC_OVER_SETTING_MIN (50) //ֵ�ѷŴ�10��
#define ARC_OVER_SETTING_DEF (150) //ֵ�ѷŴ�10��

#define LORA_NET1ID_DEF (1)//̨��������
#define LORA_NET2ID_DEF (2)//��բ������
#define LORA_NET1FR_DEF (434)//̨��Ƶ��
#define LORA_NET2FR_DEF (435)//��բƵ��


#pragma pack(1)

/*
typedef struct
{
	UINT8 SendFrameId;//0-����֡ ����-��֡���
	UINT8 SendDataLen;// ���ݳ���
	UINT8 SendData[6];
}stSendPacket_Def;
*/	
typedef struct
{
	UINT8 UserCmd; // ������
	UINT16 UserSoursAddr; // Դ��ַ
	UINT8 DataLen;//���ݳ���
	UINT8 UserData[USER_DATA_LEN_MAX + 2];// ���ݳ��� ���50�ֽ�  +2 �������
	UINT8 Crc8;//CRC8     ��������UserCmd �����ݽ���
}stUserPacket_Def;

typedef struct
{
	UINT8  LoraType;      // ֡���ͣ� 05 ��ʾ�����û�����
	UINT8  LoraSn;      // �̶�0
	UINT8  LoraCmd; //01 ��������
	UINT8  LoraLen;//���ɳ��ȣ���ָ�Ӹ��ɳ��Ⱥ�һ���ֽڵ�У��ǰһ���ֽڵĸ���
	UINT16 LoraDestAddr;//���͵�Ŀ���ַ (��˸�ʽ)
	UINT8 LoraAck;//00 ��ʾ���� ACK Ӧ��01 ��ʾ��Ҫ ACK Ӧ��
	UINT8 LoraRadius;//���Ͱ뾶��07 ��ʾ��� 7 ��
	UINT8 LoraRoute;//·�ɷ�ʽ��00 ��ֹ·�ɡ�01 �Զ�·�ɡ�02 ǿ��·��
	UINT8 LoraDateLen;//���ݳ��ȣ���ʾ�û��� �ֽڵ�������Ҫ����	
	stUserPacket_Def UserPacket; // ��������
	UINT8 LoraXor;//Ч�飺XOR Ч��ֵ 
	//��У��ֵΪ��֡ͷ��һ���ֽڣ�֡�����ֽڣ���ʼ��
	//У���ֽ�֮ǰ�����ֽ��������Ľ�������ʱȫ֡�����ֽ����������Ϊ 0 ��ΪУ��
	//��ȷ��
}stLoraStruct_Def;

typedef struct
{
	UINT8  LoraType;      // ֡���ͣ� 05 ��ʾ�����û�����
	UINT8  LoraSn;      // �̶�0
	UINT8  LoraCmd; //0x82 ����
	UINT8  LoraLen;//���ɳ��ȣ���ָ�Ӹ��ɳ��Ⱥ�һ���ֽڵ�У��ǰһ���ֽڵĸ���
	UINT16 LoraDestAddr;//Դ��ַ(��˸�ʽ)
	UINT8 LoraRss;//�ź�ǿ��
	//UINT8 LoraAck;//00 ��ʾ���� ACK Ӧ��01 ��ʾ��Ҫ ACK Ӧ��
	//UINT8 LoraRadius;//���Ͱ뾶��07 ��ʾ��� 7 ��
	//UINT8 LoraRoute;//·�ɷ�ʽ��00 ��ֹ·�ɡ�01 �Զ�·�ɡ�02 ǿ��·��
	UINT8 LoraDateLen;//���ݳ��ȣ���ʾ�û��� �ֽڵ�������Ҫ����	
	stUserPacket_Def UserPacket; // ��������
	UINT8 LoraRelayCnt; //�м�����
	UINT16 LoraRelay[7];//���7���м�
	UINT8 LoraXor;//Ч�飺XOR Ч��ֵ 
	//��У��ֵΪ��֡ͷ��һ���ֽڣ�֡�����ֽڣ���ʼ��
	//У���ֽ�֮ǰ�����ֽ��������Ľ�������ʱȫ֡�����ֽ����������Ϊ 0 ��ΪУ��
	//��ȷ��
}stLoraRxStruct_Def;

////////////////////////////////////////////////
//״̬����
typedef union
{
	UINT16 uiAll;
	struct
		{
		/*bit  0*/ UINT16 uiSTATUS_FLASH_ERROR:1; //FALSH �����¼
		/*bit  1*/ UINT16 uiSTATUS_PWR_ERROR:1; //��Դ��ѹ�쳣
		/*bit  2*/UINT16 uiSTATUS_TEMP_ERROR:1;//�¶ȴ������쳣
		/*bit  3*/UINT16 uiSTATUS_ARC_ERROR:1;//���⴫�����쳣
		/*bit  4*/ UINT16 uiSTATUS_LINKLOST_WARN:1; //���ӶϿ�
		/*bit  5*/ UINT16 uiSTATUS_SETTING_WARN:1; //��ֵ����
		/*bit  6*/ UINT16 uiSTATUS_CONFIG_WARN:1; //���ó���
		/*bit  7*/ UINT16 uiSTATUS_INPUT_WARN:1; //(�����λ)
		/*bit  8*/ UINT16 uiSTATUS_TEMPOVER_WARN:1;//�¶�Խ��
		/*bit  9*/ UINT16 uiSTATUS_ARCOVER_WARN:1;//����Խ��
		/*bit  10*/ UINT16 uiSTATUS_TRIP_WARN:1; //��Ԫ��բ
		/*bit  11*/ UINT16 uiSTATUS_FIRE_WARN:1; //��Ԫ���
		/*bit  12*/ UINT16 uiSTATUS_SENDTRIP_ERROR:1;//������բͨ�Ŵ���	
		/*bit  13*/ UINT16 uiSTATUS_TRIPFAILED_WARN:1;//��բʧ�� û�м�⵽�����ź�
		/*bit  14*/ UINT16 uiSTATUS_Res:2;
		
		/*bit  12*/ // UINT16 uiSTATUS_SETMODIFY_WARN:1;//�����޸ĸ澯(̨����¼)
		/*bit  13*/ // UINT16 uiSTATUS_NETWORK_WARN:1;//����(����4G)�쳣(̨����¼)		
	}bit;
}unUnitStatus_Def;


////////////////////////////////////////////////
//��������
//Ҳ����������
//��Ԫ���������͵�̨�� 
typedef struct 
{
	UINT8   UnitType;  //1��̨��������   2����Ԫ������  3���ɼ�������  4����բ������
	UINT16  UnitID;       // װ��ID
	unUnitStatus_Def  Status;    // װ���Լ�״̬    0x0����������                     
	SINT16  Temp1;//�¶�1  ; ���ݷŴ�10��                
	SINT16  Temp2 ;//�¶�2; ���ݷŴ�10��
	SINT16  Temp3 ;//�¶�3 ;���ݷŴ�10��
	UINT16  ArcOrDO;     // ����;   ���ݷŴ�10�� ; ��Ϊ��բ��Ԫʱ�����ֶα�ʾ���״̬ ��λ����
	UINT8   IO;      // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����
	UINT8   Version; // ��4λ����4λ
}stUnit_Def;
//����Ӧ�����ݣ�
typedef struct  
{
	UINT8  UnitCnt;        // ����stUnit�ĸ��� ���3��
	stUnit_Def  pUnit;         // stUnit_Def �ĸ���,
}stSampleReq_Def;
typedef struct  
{
	UINT32  Time;// �ظ�ʱ�䣬ʵ�ֶ�ʱ��  
	UINT8   ConfigVersion;// ̨���ظ� ���ð汾 //��Ԫ���������ֲ�ͬ��ȡ��������
	UINT8   SettingVersion;//̨���ظ�Խ�ް汾  //��Ԫ���������ֲ�ͬ��ȡԽ������
}stSampleResp_Def;


////////////////////////////////////////////////
//��������
typedef struct  
{
	UINT16   MasterID;         // ��Ԫ������ID����С��ȫ��Ψһ 0xHH00
	UINT16   SampleID;       // �ɼ�������ID����С��ȫ��Ψһ0x00HH
	UINT16   TripUnitID;  // ��բ������ID�� ��С��ȫ��Ψһ
	//UINT8    TripDO;      // ��բ��Ԫ�ϵĳ��ڱ��0 ~15
	UINT16   TripDOBit;   // ��բ��Ԫ�ϵĳ��� ��λ����BIT0��ӦDO1
}stActiveReq_Def;

//�ظ���


////////////////////////////////////////////////
//�¼�����
typedef struct  
{
	//UINT8  WarnLevel; // �澯����   1-��2-��3-��  (��̨��)
	UINT32  Time;// �¼��������¼�����ȷ���룬ͳһʹ��Unicodeʱ�䣻
	UINT8   UnitType;  //1��̨��������   2����Ԫ������  3���ɼ�������  4����բ������
	UINT16  ADDR;   //�豸��ַ
	unUnitStatus_Def   EventType;//�������ݽ�����ʽ   (ֻ����Խ�޺Ͷ����������¼�ͨ��������̨����¼)
	SINT16  Temp1;//�¶�1   ���ݷŴ�10��
	SINT16  Temp2 ;//�¶�2 ���ݷŴ�10��
	SINT16  Temp3 ;//�¶�3 ���ݷŴ�10��
	UINT16  Arc1;// ���� ���ݷŴ�10��
	UINT8  SampIO;//�ɼ���IO ��4λ��������4λ����
	UINT16  TripUnitID; // ��բ������ID
	//UINT8  TripDO;//��բ��Ԫ�ϵĳ��ڱ��
	UINT16   TripDOBit;   // ��բ��Ԫ�ϵĳ��� ��λ����BIT0��ӦDO1
}stEventReq_Def;
//�ظ���


////////////////////////////////////////////////
//��������
//˵�� ��Ԫ�������յ����������ֹͣ���й���
//            ��������ɺ�Ԫ��������������У���ж� ��ȷ���ټ���
//		  ���������֡����ʱ�䳬��5s����Ԫ�������ж�����ʧ�ܽ���ԭ�г�����������
//FileSize = �����ļ�ʵ�ʴ�С+ȫ�ĵ�����У����CRC16��2�ֽڣ�
//�����һ֡�ļ����ݵ�ĩβ+ȫ�ĵ�����У����ֵ��
typedef struct  
{
	UINT8   UnitType;  //1��̨��������   2����Ԫ������  3���ɼ�������  4����բ������
	UINT32  FileSize; // �ļ����ܳ��ȣ�  
	UINT16  CntMax;//�ܹ���CNT �η���
	UINT16  CntIndex;// ���α���λ���ļ��ĵؼ�������λ��
	UINT8   Len; // ���α��ĵĳ��ȣ�
	UINT8   *Data;  //  �ļ�����
}stUpdataFileReq_Def;

//�ظ���


////////////////////////////////////////////////
//Խ���������� ��ȡ
typedef struct  
{
	UINT8  SettingVersion;//��4λ ��4λ  //̨���ϵ��ȶ�ȡ��λ �� ��ǰʹ�ð汾�Ա� 
										//�������ͬ��д����������
	UINT8   TripType;  //��բ�߼��� 0���¶�+ ����  1���¶� 2������ 3.����+�¶�+ ���� 4. �¶���
	UINT8   FireType;    // ����������߼��� 0���¶�+ ����  1���¶� 2������ 3.����+�¶�+ ���� 4. �¶���
	SINT16  TempLimit;  // �¶�Խ�޶�ֵ��������ֵ 
	UINT16  ArcLimit;   // ����Խ�޶�ֵ��  ������ֵ
	UINT8   TempDelay; // �¶���ʱ����λS
	UINT8   ArcDelay; // ������ʱ����λS
	UINT8   AlrmTempPrec;// �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   AlrmArcPrec;  // �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
}stLimitReq_Def;
//��Ԫ��������ȡ ���ݿ�
//̨���ظ�stLimitReq_Def

/*
////////////////////////////////////////////////
//�¼��������� (д�� ��ȡ)
//��ʱ��ʹ��
typedef struct  
{
	unUnitStatus_Def  OperTypeMask; // �¼�����  ���� 
	//UINT8  WarnLevel;// �澯���� 1-��2-��3-�� (��̨��)
	UINT16  EnableMask;  // �������͹������� 1- ��ͣ��0-�ָ�
}stEventSet_Def;
//�ظ���
*/

/*
////////////////////////////////////////////////
//̨��������������(д�� ��ȡ)
typedef struct  
{
	UINT8  Mac[6];   // mac��ַ
	UINT8  ip[4];  // IP��ַ��  
	UINT8  mask[4];   // �����ַ��
	UINT8  gate[4];    // ���ص�ַ��
	UINT8  DNS[4];    // DNS��ַ��
	
	UINT8  ip1[4]; // web������ IP��ַ��  
	UINT8  mask1[4]; // web�����������ַ��
	UINT8  port1;    // web�������˿ںţ�
	UINT8  ip2[4];  // ���������IP��ַ2��  
	UINT8  mask2[4];   // ��������������ַ2��
	UINT8  port2;    // ����������˿ں�2��
}stNet_Def;
//�ظ���
*/

/*
////////////////////////////////////////////////
//̨���ƶ���������(д�� ��ȡ)
typedef struct  
{
	UINT8  ip1[4]; // web������ IP��ַ��  
	UINT8  mask1[4]; // web�����������ַ��
	UINT8  port1;    // web�������˿ںţ�
	UINT8  ip2[4];  // ���������IP��ַ2��  
	UINT8  mask2[4];   // ��������������ַ2��
	UINT8  port2;    // ����������˿ں�2��
}stCloud_Def;
//�ظ���
*/

////////////////////////////////////////////////
//�ɼ����������ݶ�ȡ
typedef struct  
{
	//UINT16   MasterID;   // ��Ԫ������ID����С��ȫ��Ψһ
	UINT16   SampleID;   // �ɼ�������ID����С��ȫ��Ψһ
	UINT16   TripUnitID;  // ��բ������ID�� ��С��ȫ��Ψһ
	//UINT8     TripDO;       // ��բ��Ԫ�ϵĳ��ڱ�ţ�
	UINT16   TripDOBit;   // ��բ��Ԫ�ϵĳ��� ��λ����BIT0��ӦDO1
        SINT8    TempRadio[3];//[3]; ��// �¶Ȳ������� �Ŵ�10��
	UINT16   ArcZero;       // ������Ư��
	UINT16   ArcRadio;       // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
}stUnitConfig_Def;
typedef struct  
{
	UINT16 ConsoleID;//̨����ַ 
	UINT16   MasterID;   // ��Ԫ������ID����С��ȫ��Ψһ
	UINT8  ConfigVersion;//��4λ ��4λ  //̨���ϵ��ȶ�ȡ��λ �� ��ǰʹ�ð汾�Ա� 
										//�������ͬ��д����������
	UINT8  UnitConfigMax;// �õ�Ԫ������ �ܹ��ɼ���Ԫ����  ���SAMPLING_UNIT_CNT_MAX
	UINT8  UnitStartNo;        // ���ο�ʼ��� ( ��Ŵ�0��UnitConfigMax-1)
	UINT8  UnitCnt;        // ���ε�Ԫ���� ,ÿ�����3��
	stUnitConfig_Def  *pUint;         // stUnit �ĸ���,
}stUnitConfigReq_Def;

//��Ԫ��������ȡ̨��
//̨���ظ�stUnitConfigReq_Def
typedef struct  
{
	UINT8  UnitStartNo;        // ���ο�ʼ��� ( ��Ŵ�0��UnitConfigMax-1)
	UINT8  UnitCnt;        // ���ε�Ԫ���� ,ÿ�����3��
}stUnitConfigRead_Def;


/*
////////////////////////////////////////////////
//�ɼ��������ĵ������д�롢�鿴��
typedef struct  
{
	UINT16   MasterID;        // ��Ԫ������ID����С��ȫ��Ψһ
	UINT16  SampleID;      // �ɼ�������ID����С��ȫ��Ψһ
	SINT8  TempRadio;  ��// �¶Ȳ������� �Ŵ�10��
	UINT16   ArcZero;       // ������Ư��
	UINT16   ArcRadio;       // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
}stUnitZeroReq_Def;
//�ظ���
*/

////////////////////////////////////////////////
//�����Ե�ͨ��
//��ʼ|����|����|����|����|У��|����
//0xCA      0x00      0x0D       0xLL      DATA    CRC8     0xCF

#define REDNOTE_FRAM_LENMAX (76)
#define REDNOTE_DATA_LENMAX (70)
#define REDNOTE_FRAM_LENDEF (6)
#define REDNOTE_FRAM_START (0xCA)
#define REDNOTE_FRAM_END (0xCF)
#define DEBUG_BUF_LEN (50)
typedef struct  
{
	UINT8 StartFlg;        // ��ʼ0xCA
	UINT8 Type;        // 0x00-����(�豸->����) 0x01 -����(����->�豸)
	UINT8 Cmd;         //������
	UINT8 Len;  //���ݳ���
	UINT8 Data[REDNOTE_DATA_LENMAX+2];//����
	UINT8 Crc8; //��TYPE��ʼ�����ݽ���
	UINT8 EndFlg;//���� 0xCF
}stRedNote_Def;


//�����д��ַ
typedef struct  
{
	UINT16  ConsoleID;    //̨����ַ     ��ʽ 0x00MM (��8λ�̶�0 MM = 1~254)
	UINT16  MasterID;    // ��Ԫ������ID       ��ʽ 0xMM00 (��8λ�̶�0 MM = 1~254)
	UINT16  UnitID;        // �ɼ���ID ��ʽ 0xYYMM (YY=��Ԫ��������8λMM = 1~254)
					    // ��բ��ԪID ��ʽ 0xFFMM(��8λ�̶�FF ,MM = 1~254)
	UINT8    NET1ID;     //̨��LORA ͨ������ID
	UINT8    NET2ID;	   //��բLORA ͨ������ID			
	UINT16 NET1FR;     //̨��LORA ͨ������Ƶ��
	UINT16 NET2FR;     //��բLORA ͨ������Ƶ��
	UINT16   ArcZero;       // ������Ư��
	UINT16   ArcRadio;       // �������ϵ�� �Ŵ�10000�� 0.025 -> 250	
	SINT8    TempRadio[3];//[3]; ��// �¶Ȳ������� �Ŵ�10��
	
}stUnitSetNetAddr_Def;
//�ظ���

//����������UNIT_SETTING_CFG_CMD
//�ظ�stUnitSettingCfg_Def
//������ɼ�������
typedef struct  
{
	UINT16  UnitID;        // �ɼ���ID ��ʽ 0xYYMM (YY=��Ԫ��������8λMM = 1~254)
					    // ��բ��ԪID ��ʽ 0xFFMM(��8λ�̶�FF ,MM = 1~254
		
	UINT8   TripType;  //��բ�߼��� 0���¶�+ ����  1���¶� 2������ 3.����+�¶�+ ���� 4. �¶���
	UINT8   FireType;    // ����������߼��� 0���¶�+ ����  1���¶� 2������ 3.����+�¶�+ ���� 4. �¶���
	SINT16  TempLimit;  // �¶�Խ�޶�ֵ��������ֵ 
	UINT16  ArcLimit;   // ����Խ�޶�ֵ��  ������ֵ
	UINT8   TempDelay; // �¶���ʱ����λS
	UINT8   ArcDelay; // ������ʱ����λS
	UINT8   AlrmTempPrec;// �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��
	UINT8   AlrmArcPrec;  // �¶� �澯��ֵ �Ƕ�����ֵ�İٷ�֮��

	UINT16   TripUnitID;  // ��բ������ID�� ��С��ȫ��Ψһ
	UINT16   TripDOBit;   // ��բ��Ԫ�ϵĳ��� ��λ����BIT0��ӦDO1	
}stUnitSettingCfg_Def;

//����������UNIT_READ_DBG_CMD
//�ظ�stUnitReadDbg_Def
//�����������Ϣ
typedef struct  
{
	UINT16  UnitID;        // �ɼ���ID ��ʽ 0xYYMM (YY=��Ԫ��������8λMM = 1~254)
					    // ��բ��ԪID ��ʽ 0xFFMM(��8λ�̶�FF ,MM = 1~254
		
	UINT8   UnitType;  //1��̨��������   2����Ԫ������  3���ɼ�������  4����բ������
	unUnitStatus_Def  Status;    // װ���Լ�״̬    0x0����������                     
	SINT16  Temp1;//�¶�1  ; ���ݷŴ�10��                
	SINT16  Temp2 ;//�¶�2; ���ݷŴ�10��
	SINT16  Temp3 ;//�¶�3 ;���ݷŴ�10��
	UINT16  ArcOrDO;     // ����;   ���ݷŴ�10�� ; ��Ϊ��բ��Ԫʱ�����ֶα�ʾ���״̬ ��λ����
	UINT8   IO;      // �ɼ���������IO״̬����λ��ʾ��4λ��������4λ����
	UINT8   Version; // ��4λ����4λ          
	char buf[DEBUG_BUF_LEN];//��ӡ��Ϣ
}stUnitReadDbg_Def;

////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /*COMMTYPE_H_*/

