
#include "./system/include/system_head.h"
#include "./driver/include/driver_head.h"
#include "./component/include/component_head.h"

unsigned int CheckSystemStatus(void);

unsigned int ServiceLoop(void)
{
	unsigned int Status;

	//check system status
	while( 0==(Status=CheckSystemStatus()) )
	{
		//check halt mode
		if(SYS_ON2OFF==SysGetOnOffKey())
		{
			SysSetOnOffKey(SYS_NULL);
			if(0==SysGetHaltModeFlag())
				SysIntoHaltMode();
		}

		//check wait mode
		if(0==SysGetWaitModeFlag())
			SysIntoWaitMode();
	}

	return(Status);
}

//========================================================================================
//========================================================================================
//========================================================================================
// 	Please add your status checking here.
//========================================================================================
//========================================================================================
//========================================================================================
#define CheckStatus_RTC GetRTC_MSGType
unsigned int CheckSystemStatus(void)
{
	unsigned int Status=0;

	//�����״̬��Ҫ���ظ��û�����,��״̬��麯�����ط�0����Ϣ����,���򷵻�0.
//	if(Status = CheckStatus_Snd())		//����������ʱ����Ҫ���û�������SysDisableWaitMode,��֤һֱ�ڸ�Ƶ.
	{
		return(Status);
	}

	return(Status);
}
