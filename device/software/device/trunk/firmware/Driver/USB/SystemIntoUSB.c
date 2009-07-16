/*-------------------------------------------------*
* Function		:
* Usage         :
* Version		:
* Return Code   :
* Revision      :
* Remark        :
*--------------------------------------------------*/
#define CREATEUSBVAR				
#define	USB_TIME_OUT	10			// timeout was  3s

#define USB_Host		0x01
#define USB_Devices		0x02

//for USB head file
#include "./system/include/system_head.h"
#include "./component/include/component_head.h"
#include ".\Reprog\USBD_Define.h"
#include ".\Reprog\USBD.h"

#include ".\Reprog\USB_Flash_reprog.h"

#include ".\Component\FS\usb_host\USB_Host_Constant.h"
#include ".\Component\FS\usb_host\USBHostMSDC.h"
#include ".\Application\Talkingbook\Include\talkingbook.h"


//unsigned int R_Write_protect = 0;//add by haoyu for no err
extern unsigned int	_Nand_ErasePhysicalBlock(unsigned long BlockAddr);
extern void USB_Reset(void);
//extern void Sound_Stop( void );
extern void USB_ServiceLoop(unsigned int unUseLoop);
void USB_TimeOut_Disable(void);
void USB_TimeOut_Enable(void);
extern unsigned int	SetBadBlock(unsigned long BlockAddr);
extern void USB_ISR(void);
extern void USB_Insert_TimeOut(void);
//key add by haoyu 2007.4.19
//extern void EnableKeyChangInt(void);
extern void DisableKeyChangeProcess(void);
//RTC add by haoyu 2007.4.19
extern void DrvRTCIntEnable( void );
extern void DrvRTCIntDisable( void );
extern void DrvEnableReminderISR( void );
extern void DrvDisableReminderISR( void );
extern unsigned int CLOCK_RATE;

#if SIMULATOR  //add by haoyu for no err 2006.2.6
int _NAND_ReadSector_USB(unsigned long  LABAddr, unsigned int blkcnt, unsigned long StoreAddr, unsigned int DMA_Channel) {};
int _NAND_WriteSector_USB(unsigned long  LABAddr, unsigned int blkcnt, unsigned long StoreAddr, unsigned int DMA_Channel,unsigned int DMA_RB_Flag) {};

#else
#endif

#ifdef USBRP
extern unsigned long  usb_time_out;
extern unsigned int R_USB_DisplayTime;
#else
unsigned long  usb_time_out;
#endif
/////////////////////////////User setting initial value start//////////////////////////////////////////
//Ҫ��main.c��������ͬ  
//#define USB_Nand_Part0_Offset	0x00002800
//#define USB_Nand_Part0_Size 	0x0000C800
//#define USB_Nand_Part1_Offset	0x0002F000
//#define USB_Nand_Part1_Size 	0x0004BE00
//#define USB_Nand_Part2_Offset	0x00000000
//#define USB_Nand_Part2_Size 	0x00000000


//Ҫ��main.c��������ͬ
#define USB_Nand_Part0_Offset	0x00000000
#define USB_Nand_Part0_Size 	0x0003C000
#define USB_Nand_Part1_Offset	0x0003C000
#define USB_Nand_Part1_Size 	0x0003C000
#define USB_Nand_Part2_Offset	0x00000000
#define USB_Nand_Part2_Size 	0x00000000

#define C_USBDiskPlugIn 		0x01
#define C_USBDiskPlugOut  		0x02
#define C_USBPlugInTimeOut		0x03
#define C_USBDevicesmountOK    	0x00
#define C_USBDevicesUnmount    	0x05
#define C_USBHUnsearchDevices	0xfffe
#define C_USBHostError  		0xffff
typedef enum 
{
		T_USBDiskPlugIn  	= 		C_USBDiskPlugIn,
		T_USBDiskPlugOut 	=		C_USBDiskPlugOut,
		USBPlugInTimeOut = 		C_USBPlugInTimeOut,
		USBDevicesUnmount	= 	C_USBDevicesUnmount,
		USBDevicesmountOK		= C_USBDevicesmountOK,
		USBHUnsearchDevices = 	C_USBHUnsearchDevices,
		USBHostError	=		C_USBHostError
}USBHostType ;

#ifdef USBRP
extern USBHostType  USBHost_Flag;
#include "Application\TalkingBook\Include\device.h"
#include "Reprog\USBD_Define.h"
/*
str_USB_Lun_Info USB_Lun_Define[] =
{
//	{0x00, LunType_NandDrive	, USB_Nand_Part0_Offset,	USB_Nand_Part0_Size, 	LunStatus_Normal},
//	{0x01, LunType_NandDrive	, USB_Nand_Part1_Offset,	USB_Nand_Part1_Size, 	LunStatus_Normal},	
	{0x00, LunType_SDCardReader	, 0L,	0L, 	LunStatus_Normal, 0L},
	//RHM
	{0x01, LunType_RAM	, 0L,	(long)(30 * 1024), 	LunStatus_Normal, 0L},
	{0x02, LunType_NOR  , 0x30000L, (long)(128 * 1024), LunStatus_Normal, 0L},
	//RHM
}; */
#define N_USB_LUN 3

//

#include "./Reprog/USB_Flash_reprog.h"
extern int fl_size;
extern unsigned int REPROG_IN_Progress;
extern unsigned int RHM_USBreprogBuf_Full;
extern unsigned int *RHM_DEBUG_LAST_LBA;
extern unsigned int *RHM_DEBUG_LAST_WRITE;
extern flash *RHM_FlashPtr;
extern unsigned int *RHM_FlashBuf;
extern long USB_INSERT_PTR;
extern long USB_ISR_PTR;
extern void (*ReprogJump)();
extern str_USB_Lun_Info 	*FP_USB_Lun_Define[];
#else  //USBRP
unsigned int R_USB_DisplayTime;
USBHostType  USBHost_Flag;
str_USB_Lun_Info USB_Lun_Define[]=
{
//	{0x00, LunType_NandDrive	, USB_Nand_Part0_Offset,	USB_Nand_Part0_Size, 	LunStatus_Normal},
//	{0x01, LunType_NandDrive	, USB_Nand_Part1_Offset,	USB_Nand_Part1_Size, 	LunStatus_Normal},	
	{0x00, LunType_SDCardReader	, 0,	0, 	LunStatus_Normal},

};
#define N_USB_LUN 1
str_USB_Lun_Info 	*FP_USB_Lun_Define[N_USB_LUN];
#endif

int SystemIntoUDisk(unsigned int serviceloop)
{
	int ret,i;
#ifdef USBRP
	extern flash *RHM_FlashPtr; //RHM , *FP_RHM_FlashPtr;
	int fl_size = USB_Flash_init((flash *)0, 0);
	flash FL;
	int flash_execution_buf[fl_size];
	RHM_FlashPtr = &FL;

	for(i=0; i<N_USB_LUN; i++) {
		FP_USB_Lun_Define[i] = &USB_Lun_Define[i];
		if( USB_Lun_Define[i].unLunType == LunType_NOR) {
			USB_Lun_Define[i].rhmLunData = (unsigned long) RHM_FlashPtr;
		}
	}
	FL.flash_exe_buf = (void *) &flash_execution_buf[0];
	USB_Flash_init(&FL, 1);
	
#endif
	SysDisableWaitMode(3);

	SetSystemClockRate(32);


	if(serviceloop) {
		int j;
		R_NAND_Present=0;
		MaxLUN = 0;
		R_SDC_Present=1;
		_deviceunmount(0);
	
		USB_TimeOut_Enable();		
		USB_Initial();
		USB_Reset();
		
		if(serviceloop == USB_CLIENT_SETUP_ONLY) {
			return(1);
		}
	}
	
	if(!serviceloop) {
		int tmp;
		long j;
		tmp = R_USB_State_Machine;
		for(j=0; j<100000; j++) {
			USB_ServiceLoop(0);
			if(R_USB_State_Machine > 0 && R_USB_State_Machine <= SCSI_CSW_Stage) {
				goto xxx;
			}
		}
		if (tmp == 0 && R_USB_State_Machine == 0) {
			return(2);
		}

		if(R_USB_State_Machine == 0xf5f5) {
			return(2);
		}
		if(!(R_USB_State_Machine > 0 && R_USB_State_Machine <= SCSI_CSW_Stage)) {
			SysEnableWaitMode(3);
			SetSystemClockRate(CLOCK_RATE);
#ifdef USBRP
			RHM_FlashPtr = 0;
#endif
			return 1;
		}
	}
xxx:
	USB_ServiceLoop(1);

	*P_USBD_Config=0x00;
	*P_USBD_INTEN=0x00;	

    if(1 == R_SDC_Present)
    {
	    _devicemount(0);
	}
	SysEnableWaitMode( 3 );
	
	SetSystemClockRate(CLOCK_RATE);

	R_USB_State_Machine == 0xf5f5; //debug
#ifdef USBRP
	RHM_FlashPtr = 0;
#endif
	
	checkInactivity(TRUE); // count being in usb as acrive ??

	return 0;
}

void USB_TimeOut_Enable(void)
{
	usb_time_out = 0;
	
	*P_TimeBaseA_Ctrl = 0xE001;
}

void USB_Insert_TimeOut(void)
{
	int temp;

	usb_time_out += 1;
		temp=*P_TimeBaseA_Ctrl;
	*P_TimeBaseA_Ctrl=temp;	//clear timerA int flag	
	
	if( usb_time_out > USB_TIME_OUT ) {
		if(USBHost_Flag == C_USBDiskPlugIn){			
		   USBHost_Flag = C_USBPlugInTimeOut;					
		}else if (!USBD_PlugIn){		//Check USB plug in		
			R_USB_Suspend = 1;
		}
	*P_TimeBaseA_Ctrl = 0x0000;
	}
}

///////////////////////////////////////////////////////////
#if 0
int SystemIntoUSB(unsigned int USBHorD_Flag)
{
	int trials, x;
	const int maxTrials = 20;
	
	if(USBHorD_Flag == USB_Host){
		
//		SysIntoHighSpeed();
		SetSystemClockRate(16);

		setLED(LED_GREEN,FALSE);
		for (trials = 0; trials < maxTrials; trials++) {
			setLED(LED_RED,TRUE);
			x = _devicemount(1); 
			setLED(LED_RED,FALSE);
			if (x == C_USBDevicesmountOK)
				break;
			wait (1000);			
		}
		if (trials < maxTrials) {
			setLED(LED_GREEN,TRUE);
			wait(1000);
			USBHost_Flag = C_USBDevicesmountOK;
			return(1);
		}
//			USB_HostProcess();
			//  unmount USB devices;  
			if(_deviceunmount(1) != 0x00)
				USBHost_Flag = C_USBDiskPlugOut;
			else
				USBHost_Flag = C_USBDevicesUnmount;
			*P_USBH_Config = 0;
			*P_USBH_INTEN  = 0;		
		}	
		
		SetSystemClockRate(CLOCK_RATE);

	}else if(USBHorD_Flag == USB_Device){
		SystemIntoUDisk(1);	
	}

	return 0;
}
#endif
int setUSBHost(BOOL enter) {
	int trials, x;
	const int maxTrials = 10;
	long l1;

	if (enter){	
		SetSystemClockRate(16);	
		setLED(LED_GREEN,FALSE);
		for (trials = 0; trials < maxTrials; trials++) {
			setLED(LED_RED,TRUE);
			x = _devicemount(1); 
			setLED(LED_RED,FALSE);
			if (x == C_USBDevicesmountOK)
				break;
			wait (1000);			
		}
		if (trials < maxTrials) {
			setLED(LED_GREEN,TRUE);
			wait(1000);
			USBHost_Flag = C_USBDevicesmountOK;
		} else {
			SetSystemClockRate(CLOCK_RATE);
		}
	} else {
			//  unmount USB devices;  
			if(_deviceunmount(1) != 0x00)
				USBHost_Flag = C_USBDiskPlugOut;
			else
				USBHost_Flag = C_USBDevicesUnmount;
			SetSystemClockRate(CLOCK_RATE);
			*P_USBH_Config = 0;
			*P_USBH_INTEN  = 0;		
	}

	return 0;
}
#if 0
void USB_HostProcess(void) {
	int keystroke, i;
	char from[30], to[30], num[2];
	int maxTrials = 10;

	i = 0;		
	do {
//		key = getKey();
		keystroke = keyCheck(0);
		if(keystroke == KEY_LEFT) {
//			case KEY_LEFT:/*0x01*/:
			  // copy test file from device to host
			  strcpy(from,"b:\\\\test");
			  strcpy(to,"a:\\\\test");
			  intToString(i++,num,2);
			  strcat(to,num);
			  testCopy(from,to,maxTrials);
//			  break;
		} else if (keystroke == KEY_RIGHT) {
//			case 0x10/*0x02*/:
			  // copy test file from host to device
			  strcpy(from,"a:\\\\test");
			  strcpy(to,"b:\\\\test");
			  intToString(i++,num,2);
			  strcat(to,num);
			  testCopy(from,to,maxTrials);
//			  break;
		}
	} while (keystroke != 0x04);
}
#endif
void
HandleHostISR()
{
	extern void USBDiskPlugOut();
	extern void USBDiskPlugIn();

	unsigned int r1;
	
	r1 = *P_USBH_INT;
	
	if(r1 & 0x800) {
		*P_USBH_INT = r1;
		USBDiskPlugOut();
	} else if( r1 & 0x1) {
		*P_USBH_INT = r1;
		USBDiskPlugIn();
	} else {
		USBHostISR();
	}
	
}
