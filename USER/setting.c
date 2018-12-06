#include<string.h>
#include "setting.h"
#include "type_slv.h"


stSysInfo_Def stSysInfo;

stSysEven_Def stSlvEventBuf[EVENT_BUF_CNT];


//����¼���������
void AddEvent(stUserSlvEventReq_Def *SlvEvent)
{
	UINT8 ui;
	for(ui=0;ui<EVENT_BUF_CNT;ui++)
	{
		if(stSlvEventBuf[ui].EventStatus ==0)
		{
			memcpy((UINT8 *)&stSlvEventBuf[ui].stSlvEvent,(UINT8 *)SlvEvent,sizeof(stUserSlvEventReq_Def));
			stSlvEventBuf[ui].SendCnt =0;//�ٴ�����
			memset((UINT8*)&stSlvEventBuf[ui].stSlvEvent,0,sizeof(stUserSlvEventReq_Def));
			stSlvEventBuf[ui].EventStatus =1;//ֵ��־
			break;
		}
	}	
}

//�����¼� ���Ͳ���ֵ
//ÿ 1  S ����
void SendSampling(void)
{
	if(stSysInfo.stSysVar.SysWorkMod !=3 )
		return;
	
	SlvSampTempReq();
	SlvSampTempReq();

	stSysInfo.stSysVar.HeartCnt ++;
	if(stSysInfo.stSysVar.HeartCnt > 5)
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 1;
		stSysInfo.stSysVar.HeartCnt =5;
	}
	else
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_LINKLOST_WARN = 0;
	}
}

//����ط�
//ÿ250ms����
void CheckResend(void)
{
	UINT8 ui;
	if(stSysInfo.stSysVar.TripEvent)//��鷢����բ
	{
		SlvTripReq();
	}
	for(ui=0;ui<EVENT_BUF_CNT;ui++)//��鷢���¼�
	{
		if(stSlvEventBuf[ui].EventStatus)
		{
		//	SlvEventReq((stSysEven_Def*)&stSlvEventBuf[ui]);
			//break;//ÿ��ֻ����1���¼�
		}		
	}
}

void AddTripEvent(UINT8 CH)
{
	stUserSlvEventReq_Def SlvEvent;

	if(CH>2)
		CH =0;
	
	SlvEvent.EventType.bit.uiEventType_Trip = 1;
	SlvEvent.Data1.TempChData1 = CH;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.SampTemp[CH] ;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.SampArc;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddOldTripEvent(void)
{
	stUserSlvEventReq_Def SlvEvent;

	SlvEvent.EventType.bit.uiEventType_Trip = 1;
	SlvEvent.Data1.TempChData1 = stSysInfo.stSysVar.TripTempCh ;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.TripTempVal;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.TripArcVal;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddTripFailedEvent(void)
{
	stUserSlvEventReq_Def SlvEvent;

	SlvEvent.EventType.bit.uiEventType_Record = 1;
	SlvEvent.Data1.RecordData1.bit.uiEvent_Record_TripFailed = 1;
	SlvEvent.Data2.TempData2 = 0;
	SlvEvent.Data3.ArcData3= 0;

	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}
void AddFireEvent(UINT8 CH)
{
	stUserSlvEventReq_Def SlvEvent;

	if(CH>2)
		CH =0;
	
	SlvEvent.EventType.bit.uiEventType_Fire = 1;
	SlvEvent.Data1.TempChData1 = CH;
	SlvEvent.Data2.TempData2 = stSysInfo.stSysVar.SampTemp[CH] ;
	SlvEvent.Data3.ArcData3= stSysInfo.stSysVar.SampArc;
	AddEvent((stUserSlvEventReq_Def *)&SlvEvent);
}


//��ʱ��100ms ����1 ��
//�߼�����
void LogicCheck(void)
{
	UINT8 ArcOver = 0;
	UINT8 TempOver =0;
	UINT8 InputOver =0;
	UINT8 TripRes =0;
	UINT8 FireRes =0;
	UINT8 TempCh =0;

	//�����ж�
	if(stSysInfo.stSysVar.SampArc > stSysInfo.stSetting.ArcLimit)
	{
		stSysInfo.stSysVar.TripArcTimeCnt ++;
		if(stSysInfo.stSysVar.TripArcTimeCnt > stSysInfo.stSetting.ArcDelay*10)
		{
			ArcOver = 1;
			stSysInfo.stSysVar.TripArcTimeCnt = stSysInfo.stSetting.ArcDelay*10;
		}
	}
	else
	{
		stSysInfo.stSysVar.TripArcTimeCnt =0;
	}

	//�¶��ж�
	if(stSysInfo.stSetting.TripType == LOGIC_TEMPAND)//�¶���
	{
		if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
		&&(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
		&&(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
		{
			stSysInfo.stSysVar.TripTempTimeCnt ++;
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*10)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*10;
				TempCh =0;
			}
		}
		else
		{
			stSysInfo.stSysVar.TripTempTimeCnt =0;
		}
	}
	else//�¶Ȼ�
	{
		if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
		||(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
		||(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
		{
			stSysInfo.stSysVar.TripTempTimeCnt ++;
			if(stSysInfo.stSysVar.TripTempTimeCnt > stSysInfo.stSetting.TempDelay*10)
			{
				TempOver = 1;
				stSysInfo.stSysVar.TripTempTimeCnt = stSysInfo.stSetting.TempDelay*10;
				if(stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
					TempCh =0;
				else if(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
					TempCh =1;
				else
					TempCh =2;
					
			}
		}
		else
		{
			stSysInfo.stSysVar.TripTempTimeCnt =0;
		}
	}
	//�����ж�
	if(stSysInfo.stSetting.InputBit & stSysInfo.stSysVar.SampInput)
	{
		InputOver = 1;
	}

	//��բ�߼�
	switch( stSysInfo.stSetting.TripType)
	{
		case LOGIC_ARC_TEMPOR : //���� + �¶�                
			if((ArcOver)&&(TempOver))
				TripRes = 1;				
		break; 
		case LOGIC_TEMPOR : //���¶� ���ϵ
			if(TempOver)
				TripRes = 1;	
		break; 
		case LOGIC_ARCONLY : // ������     
			if(ArcOver)
				TripRes = 1;	
		break; 
		case LOGIC_ARC_TEMP_INPUT : //�¶�+����+����
			if((ArcOver)&&(TempOver)&& (InputOver))
				TripRes = 1;
		break; 
		case LOGIC_TEMPAND : //���¶� ���ϵ
			if(TempOver)
				TripRes = 1;	
		break; 
		default :  break;
	}

	// ������߼�
	switch( stSysInfo.stSetting.OutFireType)
	{
		case LOGIC_ARC_TEMPOR : //���� + �¶�                
			if((ArcOver)&&(TempOver))
				FireRes = 1;				
		break; 
		case LOGIC_TEMPOR : //���¶� ���ϵ
			if(TempOver)
				FireRes = 1;	
		break; 
		case LOGIC_ARCONLY : // ������     
			if(ArcOver)
				FireRes = 1;	
		break; 
		case LOGIC_ARC_TEMP_INPUT : //�¶�+����+����
			if((ArcOver)&&(TempOver)&& (InputOver))
				FireRes = 1;
		break; 
		case LOGIC_TEMPAND : //���¶� ���ϵ
			if(TempOver)
				FireRes = 1;	
		break; 
		default :  break;
	}

	if((TripRes)&&(stSysInfo.stSysVar.TripFlag == 0))//������բ
	{
		stSysInfo.stSysVar.TripFlag = 1;
		SlvTripReq();		
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIP_WARN = 1; //��բ״̬
		//AddTripEvent(TempCh);//�ȴ��������Դʧȥ ���ͱ���
		stSysInfo.stSysVar.TripTempCh = TempCh;
		stSysInfo.stSysVar.TripTempVal = stSysInfo.stSysVar.SampTemp[TempCh] ;
		stSysInfo.stSysVar.TripArcVal = 	stSysInfo.stSysVar.SampArc;
		stSysInfo.stSysVar.TripFaileTimeCnt =0;//
	}
	if((FireRes)&&(stSysInfo.stSysVar.FireFlag == 0))//���������
	{
		stSysInfo.stSysVar.FireFlag = 1;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_FIRE_WARN = 1;//�������״̬
			//	add ���̵�����������
		AddFireEvent(TempCh);
	}	
}
	

//��ʱ��1s ����1 ��
//Խ�޹���
void OverLimitCheck(void)
{
	UINT8 ArcOver = 0;
	UINT8 TempOver =0;
	UINT8 InputOver =0;
	UINT8 TempCh =0;

	//�����ж�
	if(stSysInfo.stSysVar.SampArc > stSysInfo.stSetting.ArcLimit)
	{
		stSysInfo.stSysVar.AlmArcTimeCnt ++;
		if(stSysInfo.stSysVar.AlmArcTimeCnt > stSysInfo.stSetting.ArcDelay)
		{
			ArcOver = 1;
			stSysInfo.stSysVar.AlmArcTimeCnt = stSysInfo.stSetting.ArcDelay;
		}
	}
	else
	{
		stSysInfo.stSysVar.AlmArcTimeCnt =0;
	}

	//�¶��ж�
	if((stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
	||(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
	||(stSysInfo.stSysVar.SampTemp[2] > stSysInfo.stSetting.TempLimit))
	{
		stSysInfo.stSysVar.AlmTempTimeCnt ++;
		if(stSysInfo.stSysVar.AlmTempTimeCnt > stSysInfo.stSetting.TempDelay)
		{
			TempOver = 1;
			stSysInfo.stSysVar.AlmTempTimeCnt = stSysInfo.stSetting.TempDelay;
			if(stSysInfo.stSysVar.SampTemp[0] > stSysInfo.stSetting.TempLimit)
				TempCh =0;
			else if(stSysInfo.stSysVar.SampTemp[1] > stSysInfo.stSetting.TempLimit)
				TempCh =1;
			else
				TempCh =2;				
		}
	}
	else
	{
		stSysInfo.stSysVar.AlmTempTimeCnt =0;
	}
	
	//�����ж�
	if( stSysInfo.stSysVar.SampInput)
	{
		InputOver = 1;
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_INPUT_WARN = 1;//�����λ״̬
		stSysInfo.stSysVar.TripFaileTimeCnt =0;
	}

	if(ArcOver)//����Խ��  ����Ԫ��¼
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_ARCOVER_WARN = 1;//����Խ��״̬
	}
	if(TempOver)//�¶�Խ��  ����Ԫ��¼
	{
		stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TEMPOVER_WARN = 1;//����Խ��״̬
	}
	
	//������բ��¼
	if((InputOver ==0)&&(stSysInfo.stSysVar.TripFlag))
	{
		
		stSysInfo.stSysVar.TripFaileTimeCnt++;
		if(stSysInfo.stSysVar.TripFaileTimeCnt == 1)//ֻ��¼һ��
		{
			AddOldTripEvent();
		}
		if(stSysInfo.stSysVar.TripFaileTimeCnt == TRIP_FAILE_TIMEOUT)
		{
			stSysInfo.stSysVar.TripFaileTimeCnt = TRIP_FAILE_TIMEOUT;//ֻ��¼һ��
			stSysInfo.stSysVar.SysStatus.bit.uiSTATUS_TRIPFAILED_WARN = 1;// ��բʧ��״̬
			AddTripFailedEvent();//��բʧ�ܼ�¼ û�п���				
		}
	}

	//t��բ����
	if(stSysInfo.stSysVar.TripFlag)
	{
		stSysInfo.stSysVar.ResetTripTimeCnt ++;
		if((stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_RESET_TIMEOUT)
		&&(stSysInfo.stSysVar.ResetTripTimeCnt>=TRIP_FAILE_TIMEOUT))	
		{
			stSysInfo.stSysVar.ResetTripTimeCnt =0;
			stSysInfo.stSysVar.TripFaileTimeCnt = 0; //������բ
			stSysInfo.stSysVar.TripFlag = 0;		
		}
	}
	//t��𸴹�
	if(stSysInfo.stSysVar.FireFlag)
	{
		stSysInfo.stSysVar.ResetFireTimeCnt ++;
		if(stSysInfo.stSysVar.ResetFireTimeCnt>=FIRE_RESET_TIMEOUT)
		{
			stSysInfo.stSysVar.ResetFireTimeCnt =0;
			stSysInfo.stSysVar.FireFlag = 0;		
		}
	}
}



