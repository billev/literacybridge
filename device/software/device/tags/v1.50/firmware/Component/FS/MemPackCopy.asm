.include .\System\Include\System\GPL162002.inc


.ram
.var	_gBankTemp


.code
//�����ַ���л�bank��ds,����r1Ϊdsֵ
//�ƻ�r1

M_MemPackSwitchBank:		.macro	addrh
	//set [P_BankSwitch]
	r1 = addrh
	r1 = r1 lsr 4
	r1 = r1 lsr 1
	[P_Segment_Num] = r1
	
	//return DS
	r1 = addrh
	r1 = r1 & 0x003f
	r1 += 0x0020
	ds = r1
.endm

//����[P_BankSwitch]
//void MemPackBackupBankAddr(void);
.public _MemPackBackupBankAddr
_MemPackBackupBankAddr:
	r1 = [P_Segment_Num]
	[_gBankTemp] = r1
	retf

//�ָ�[P_BankSwitch]
//void MemPackRestoreBankAddr(void);
.public _MemPackRestoreBankAddr
_MemPackRestoreBankAddr:
	r1 = [_gBankTemp]
	[P_Segment_Num] = r1
	retf

//����word��ַ����һ��word
//unsigned int MemPackReadWord(unsigned long addr);
.public _MemPackReadWord
_MemPackReadWord:
	r4 = sp + 3
	r2 = [r4++]			//addrl
	r3 = [r4++]			//addrh
	
	M_MemPackSwitchBank	r3
	r1 = d:[r2]
	retf

//����word��ַ��дһ��word
//void MemPackWriteWord(unsigned long addr, unsigned int data);
.public _MemPackWriteWord
_MemPackWriteWord:
	r4 = sp + 3
	r2 = [r4++]			//addrl
	r3 = [r4++]			//addrh
	r4 = [r4]			//data
	
	M_MemPackSwitchBank	r3
	d:[r2] = r4
	retf
	
//����byte��ַ����һ��byte
//unsigned int MemPackReadByte(unsigned long addr);
.public _MemPackReadByte
_MemPackReadByte:
	r4 = sp + 3
	r3 = [r4++]			//addrl
	r4 = [r4]			//addrh
	
	r4 = r4 lsr 1		//byte��ַ����2
	r2 = r2 lsr 3
	r2 = r3
	r2 = r2 ror 1
	
	M_MemPackSwitchBank	r4
	r1 = d:[r2]
	
	test r3, 1
	jz	?L_ReadLowByte
	r1 = r1 lsr 4
	r1 = r1 lsr 4
	retf
	
?L_ReadLowByte:
	r1 &= 0x00ff	
	retf
	
//����byte��ַ��дһ��byte
//void MemPackWriteByte(unsigned long addr, unsigned int data);
.public _MemPackWriteByte
_MemPackWriteByte:
	push bp to [sp]
	bp = sp + 3
	r3 = [bp + 1]		//addrl
	r4 = [bp + 2]		//addrh
	
	r4 = r4 lsr 1		//byte��ַ����2
	r2 = r2 lsr 3
	r2 = r3
	r2 = r2 ror 1
	
	M_MemPackSwitchBank	r4
	r1 = d:[r2]
	
	r4 = [bp + 3]
	
	test r3, 1
	jz	?L_WriteLowByte
	r1 &= 0x00ff
	r4 = r4 lsl 4
	r4 = r4 lsl 4
	r1 |= r4
	d:[r2] = r1
	
	pop bp from [sp]
	retf
	
?L_WriteLowByte:
	r4 &= 0x00ff
	r1 &= 0xff00
	r1 |= r4
	d:[r2] = r1
	
	pop bp from [sp]
	retf
