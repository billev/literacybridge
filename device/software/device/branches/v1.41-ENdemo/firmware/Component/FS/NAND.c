#define		CREAT_DRIVERLAYER_STRUCT

#include	"typedef.h"
#include	"DriverLayer.h"
#include	".\Driver\Include\NandDriver\Nand2_Oper.h"


extern INT16	bNandInit;

/************************************************************************/
/* NAND_Initial 
func:	
		��������������ʹ��
		ʹ�ø�λָ�ʹ��������ǿ�Ƹ�λ����
		��ʼ�� ������ ��ת����

input:	void

output:	0		�ɹ�
		��1		ʧ��
   
note:	��ʼ���ɹ���������һ��ȫ�ֱ��� bNandInit 

                                                                  */
/************************************************************************/
INT16 NAND_Initial(void)
{
	return 0;
}

INT16 NAND_Uninitial(void)
{

	return 0;
}

void NAND_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors = Nand_Part0_Size;
	info->nBytesPerSector = 512;
}

INT16 NAND_ReadSector(UINT32 blkno , UINT16 blkcnt ,  UINT32 buf)
{
	return	_NAND_ReadSector(blkno + Nand_Part0_Offset, blkcnt, buf);
}

/************************************************************************/
/*		NAND_WriteSector
func:	�����������д������
input:	
output:	0		�ɹ�
		����ֵ	ʧ��

note:	�����˶Ե����Ƿ���Ը�ʽ���Ŀ���
		�������� _PC_UNABLE_FORMAT_UDISK ʱ����ֹ�˶��߼��� �߼�0 �ȵ�д�붯��
		PC ���ܶ�U�̽��и�ʽ��  
                                                                     */
/************************************************************************/
INT16 NAND_WriteSector(UINT32 blkno , UINT16 blkcnt ,  UINT32 buf)
{
	return _NAND_WriteSector(blkno + Nand_Part0_Offset, blkcnt, buf);	
}

CONST struct Drv_FileSystem FS_NAND_driver = {
	"NAND" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	NAND_Initial ,
	NAND_Uninitial ,
	NAND_GetDrvInfo ,
	NAND_ReadSector ,
	NAND_WriteSector ,
};

