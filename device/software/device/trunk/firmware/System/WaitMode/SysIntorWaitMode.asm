.INCLUDE ./system/include/system_head.inc

.PUBLIC	_SysIntoWaitMode
.PUBLIC _SysWaitLCDFP

.EXTERNAL _SysIntoHighSpeed
.EXTERNAL _EnableKeyChangInt

.CODE

_SysWaitLCDFP: .PROC
	r1 = [P_LCD_Setup]			//clear LCD FP signal interrupt flag
	[P_LCD_Setup] = r1

?L_WaitFP:
	r1 = [P_LCD_Setup]			//clear LCD FP signal interrupt flag
	jpl ?L_WaitFP				//wait for a frame refresh finish

	retf
.ENDP	

_SysIntoWaitMode:	.PROC
	//�ж��Ƿ���Ҫ����Ƶ��
	R1 = [P_Clock_Ctrl]
	CMP R1,SYS_12MCLOCK
	JE	?L_IntoWaitMode
	
	CALL _SysWaitLCDFP

	R1 = SYS_12MCLOCK			
	__outp P_Clock_Ctrl,R1

	//�˴��������״̬���ȴ�Ƶ��ת�����.
	//���û�еȴ�,����key change���ܻ���.ע��ֻ��pb0-2��������������.
?WaitPll0:
	r1 = [P_Power_State]
	r1 &= 0x07
	jz ?WaitPll0
	
	R1 = LCD_12MCLOCK			
	__outp P_LCD_Clock,R1

//  Test Programme
//	call	_EnableKeyChangInt
//	__clrb P_TimeBaseC_Ctrl ,13,R1			//clear timebasec interrupt
//	__clrb P_RTC_INT_Ctrl ,0,R1				//clear half second
//	__clrb P_RTC_INT_Ctrl ,1,R1				//clear  second
//	__clrb P_RTC_INT_Ctrl ,2,R1				//clear  miniute
//	__clrb P_RTC_INT_Ctrl ,3,R1				//clear  hour
//	__clrb P_RTC_INT_Ctrl ,8,R1				//clear  schedule

?L_IntoWaitMode:
	R1 = WAITMODE_CONST			//into waitmode
	__outp P_WAIT,R1
	//����6��nop
	nop
	nop
	nop
	nop
	nop
	nop
			
	retf

.ENDP

.END