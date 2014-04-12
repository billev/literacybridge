//#define		CREAT_DRIVERLAYER_STRUCT
#include	"./component/include/fs/DriverLayer.h"
//NAND FLASH partition define
const  UINT32 Nand_Part0_Offset = NAND_PART0_OFFSET;
const  UINT32 Nand_Part0_Size = NAND_PART0_SIZE;
const  UINT16 Nand_Part0_Mode = NAND_PART0_MODE;

const  UINT32 Nand_Part1_Offset = NAND_PART1_OFFSET;
const  UINT32 Nand_Part1_Size = NAND_PART1_SIZE;
const  UINT16 Nand_Part1_Mode = NAND_PART1_MODE;

const  UINT32 Nand_Part2_Offset = NAND_PART2_OFFSET;
const  UINT32 Nand_Part2_Size = NAND_PART2_SIZE;
const  UINT16 Nand_Part2_Mode = NAND_PART2_MODE;

const UINT32 Nand_Reserve_Offset = NAND_RESERVE_OFFSET;

int	bNandInit;
int bUSB_Init;

//���岻ʹ��IRAM����NAND BOOT������²���ʹ��IRAM������ʼֵ��ȫ�ֱ�����

struct Drv_FileSystem *FileSysDrv[NBLKDEV];

void FS_LoadDriver(void)
{	
	FileSysDrv[0] =&FS_SD_driver;
//	FileSysDrv[1] =&FS_NAND_driver;
//	FileSysDrv[2] =&FS_NAND2_driver;
	
	bNandInit = -1;
	bUSB_Init = -1;
}




