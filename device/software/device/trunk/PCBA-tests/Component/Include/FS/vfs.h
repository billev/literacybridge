#ifndef	__POSIX_INTERFACE_h__
#define	__POSIX_INTERFACE_h__

#define LFN_API
#define WITHFAT32

#include "Typedef.h"

/**************************************************************************/
/*                              M A C R O S                               */
/***************************************************************************/

#define FAT16_Type	        		0x01
#define FAT32_Type	        		0x02
#define FORCE_FAT32_Type	        0x12
#define FORCE_FAT16_Type	        0x11

/*-----------------  seek flags  ------------------*/
#define SEEK_SET     0 /* offset from beginning of the file*/
#define SEEK_CUR     1 /* offset from current location     */
#define SEEK_END     2 /* offset from eof  */

/***************** open flags (the 2nd parameter)**********************/   //wanghuidi
#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_ACCMODE       0x0003

#define O_TRUNC         0x0200 /*    both */
#define O_CREAT         0x0400
#define O_EXCL		    0x4000	/* not fcntl */

/*-----------------------------------------------------*/
/* File attribute constants for _findfirst() */

#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

/*-----------------------------------------------------*/

/* FAT file system attribute bits                                   */
#define D_NORMAL        0       /* normal                       */
#define D_RDONLY        0x01    /* read-only file               */
#define D_HIDDEN        0x02    /* hidden                       */
#define D_SYSTEM        0x04    /* system                       */
#define D_VOLID         0x08    /* volume id                    */
#define D_DIR           0x10    /* subdir                       */
#define D_ARCHIVE       0x20    /* archive bit                  */
    							/* /// Added D_DEVICE bit.  - Ron Cemer */

#define D_FILE			(0x40)	
#define D_ALL (D_FILE | D_RDONLY | D_HIDDEN | D_SYSTEM | D_DIR | D_ARCHIVE)

#define UNI_GBK			0
#define UNI_BIG5		1
#define UNI_SJIS		2

#define UNI_ENGLISH		0x8003
#define UNI_ARABIC		0x8004
#define UNI_CP1252		0x8005

#define UNI_UNICODE		0x8100
//#define UNI_INVALID_TAB	0xffff

/**************************************************************************/
/*                              DATA TYPES ABOUT FILE                     */
/***************************************************************************/
//----------------------------------
struct stat_t 
{	
	UINT16	st_mode;
	INT32	st_size;
	UINT32	st_mtime;
};

//----------------------------------
struct _diskfree_t {
	UINT32 total_clusters;
	UINT32 avail_clusters;
	UINT32 sectors_per_cluster;
	UINT32 bytes_per_sector;
};

struct deviceinfo 	 {
	INT8 device_name[16];		// device name
	INT8 device_enable;			// device enable status
	INT8 device_typeFAT;		// device FAT type
	UINT32 device_availspace;	// device available space
	UINT32 device_capacity;		// device capacity
};

// data structure for _setftime()
struct timesbuf 	 {
	UINT16 modtime;
	UINT16 moddate;
	UINT16 accdate;
};

// data structure for _getfstat(), 555FS
struct statbuf 	 {
	UINT32 st_size;
	UINT8 st_mode;
	UINT16 crtime;
	UINT16 crdate;
	UINT16 modtime;
	UINT16 moddate;
	UINT16 accdate;
};

struct f_info         
{
	UINT8 f_attrib;
	UINT16  f_time;
	UINT16  f_date;
	UINT32 f_size;
	UINT16  entry;			//2006.7.12
#ifdef LFN_API
	 INT8 f_name[256]; 	 //change to 256 form 255
#else
	 INT8 f_name[13];
#endif
};

typedef struct        
{
	INT8    f_dsk;
	UINT32  f_entry;
	UINT16  f_offset;
}f_pos, *f_ppos;

struct sfn_info
{
    UINT8   f_attrib;
	UINT16  f_time;
	UINT16  f_date;
	UINT32  f_size;
    INT8    f_name[9];
    INT8    f_extname[4];
    f_pos	f_pos;
};

struct nls_table {
	INT8			*charset;						//���ر�������
	UINT16			Status;							//unicode����״��
	INT16			(*init_nls)(void);				//��ʼ��unicodeת������
	INT16			(*exit_nls)(void);				//�˳�unicodeת������
	UINT16			(*uni2char)(UINT16 uni);		//unicodeת��Ϊ���ر���
	UINT16			(*char2uni)(UINT8 **rawstring);	//���ر���ת��Ϊunicode
};

//struct STFileNodeInfo
//{
//	INT8		flag;
//	INT8		disk;
//	INT8*		extname;
//	UINT16*		Buffer;
//	UINT16		BufferSize;
//};
struct STFileNodeInfo
{
	INT8		flag;					//��struct�Ƿ���Ч
	INT8		disk;					//disk
	INT8		extname[4];				//��ͳ�Ƶ��ļ�����չ��
	UINT16*		FileBuffer;				//�ļ��ڵ�buffer
	UINT16		FileBufferSize;			//�ļ��ڵ�buffer��С��ÿ20 wordΪһ���ڵ㣬��������20�ı���
	UINT16*		FolderBuffer;			//Ŀ¼�ڵ�buffer
	UINT16		FolderBufferSize;		//Ŀ¼�ڵ�buffer��С�������СΪ0����ʾ��ͳ��Ŀ¼��Ϣ
};

/***************************************************************************/
/*        F U N C T I O N    D E C L A R A T I O N S	     			   */
/***************************************************************************/
//����disk�ĵ�һ���ļ�
f_ppos getfirstfile(INT16 dsk, INT8* extname, struct sfn_info* f_info, INT16 attr);
//����disk���ļ�
f_ppos getnextfile(INT16 dsk, INT8* extname, struct sfn_info* f_info, INT16 attr);
//open
INT16 sfn_open(f_ppos ppos);
//get file status
INT16 sfn_stat(INT16 fd, struct sfn_info *sfn_info);
//�����ļ����ڵ㷵�س��ļ�����Ϣf_info
INT16 GetFileInfo(f_ppos ppos, struct f_info *f_info);
//����Ŀ¼���ڵ㷵�س��ļ�����Ϣf_info
INT16 GetFolderInfo(f_ppos ppos, struct f_info *f_info);
//�����ļ����ڵ�ɾ�����ļ�
INT16 sfn_unlink(f_ppos ppos);

//ͳ��ĳ�����͵��ļ�������
INT16 StatFileNumByExtName(INT16 dsk, LPSTR extname, UINT32 *filenum);
//ͳ�ƴ�����ָ����չ�����ļ������Ͱ��������ļ���Ŀ¼����
INT16 GetFileNumEx(struct STFileNodeInfo *stFNodeInfo, UINT32 *nFolderNum, UINT32 *nFileNum);
//����index��ȡ�ļ����ڵ���Ϣ
//ע��ִ�д˺���ǰ��ȷ��ִ���˺���GetFileNumEx
//0 <= nIndex < nMaxFileNum
f_ppos GetFileNodeInfo(struct STFileNodeInfo *stFNodeInfo, UINT32 nIndex, struct sfn_info* f_info);
//����Ŀ¼��index��ȡĿ¼�е��ض���չ�����ļ�����Ŀ
//ע��ִ�д˺���ǰ��ȷ��ִ���˺���GetFileNumEx
//�ɹ�����0��ʧ�ܷ���-1
INT16 GetFileNumOfFolder(struct STFileNodeInfo *stFNodeInfo, UINT32 nFolderIndex, UINT16* nFile);
//����folder��index�õ���folder�ĵ�һ���ļ���index
//ע��ִ�д˺���ǰ��ȷ��ִ���˺���GetFileNumEx
//�ɹ�����0��ʧ�ܷ���-1
INT16 FolderIndexToFileIndex(struct STFileNodeInfo *stFNodeInfo, UINT32 nFolderIndex, UINT32* nFileIndex);
//����file��index�õ���file���ڵ�folder��index
//ע��ִ�д˺���ǰ��ȷ��ִ���˺���GetFileNumEx
//�ɹ�����0��ʧ�ܷ���-1
INT16 FileIndexToFolderIndex(struct STFileNodeInfo *stFNodeInfo, UINT32 nFileIndex, UINT32* nFolderIndex);
//����Ŀ¼index��ȡ��Ŀ¼�ڵ����Ϣ
//ע��ִ�д˺���ǰ��ȷ��ִ���˺���GetFileNumEx
f_ppos GetFolderNodeInfo(struct STFileNodeInfo *stFNodeInfo, UINT32 nFolderIndex, struct sfn_info* f_info);

//convert the dos_data to year, month, day
void dosdate_decode(UINT16 dos_date, UINT16 *pyear, UINT8 *pmonth, UINT8 *pday);
//convert the dos_time to hour, minute, second
void dostime_decode(UWORD dos_time, UINT8 *phour, UINT8 *pminute, UINT8 *psecond);

//convert *tp to a string like "hh:mm:ss"
INT8 *time_decode(UINT16 *tp, INT8 *timec);
//convert *dp to a string like "yyyy-mm-dd" 
INT8 *date_decode(UINT16 *dp, INT8 *datec);

//close all the opened files except the registed file
void fs_safexit(void);
//regist opened file so when you call function fs_safexit() this file will not close
void fs_registerfd(INT16 fd);

//close all the opened files of the disk
void disk_safe_exit(INT16 dsk);

INT16 close(INT16 fd);

INT16 open(LPSTR path , INT16 open_flag);

INT32 read(INT16 fd , LPDATA buf , UINT32 size);
INT32 write(INT16 fd , LPDATA buf ,  UINT32 size);

INT32 lseek(INT16 fd ,INT32 offset ,INT16 fromwhere);
INT32 tell(INT16 fd); 

INT16 unlink(LPSTR filename);
INT16 rename(LPSTR oldname , LPSTR newname);

INT16 readB(INT16 fd , LPDATA buf , UINT16 size);
INT16 writeB(INT16 fd , LPDATA buf ,  UINT16 size);

INT16 mkdir(LPSTR pathname);

INT16 rmdir(LPSTR pathname);
INT16 chdir(LPSTR path);
LPSTR getcwd(LPSTR buffer , INT16 maxlen );

INT16 fstat(INT16 handle , struct stat_t *statbuf);
INT16 stat(LPSTR path, struct stat_t *statbuf);

INT16 _findfirst(LPSTR name , struct f_info *f_info ,  UINT16 attr);
INT16 _findnext(struct f_info *f_info);

INT16 _getdiskfree(INT16 dsk, struct _diskfree_t *st_free);

#define _changedisk     fs_changedisk 
INT16 _changedisk( UINT8 disk);  
/****************************************************************************/
INT32 vfsFreeSpace(INT16 driver);
INT16 _copy(LPSTR srcfile , LPSTR destfile);

void fs_init(void);
INT16 fs_uninit();

UINT16 tellcurrentfiledir(void);
UINT16 telldir(void);
void seekdir( UINT16 pos);     //the parameter "pos" must be the return value of "telldir"
void rewinddir(void);

//set file attribute
INT16 _setfattr (LPSTR filename ,  UINT16 attr);
//set dir attribute
INT16 _setdirattr (LPSTR dirname ,  UINT16 attr); 
//get dir attribute
INT16 _getdirattr (LPSTR dirname ,  UINT16 *attr); 

INT16 _devicemount(INT16 disked);
INT16 _deviceunmount(INT16 disked);

INT16 _getfserrcode ();
void _clsfserrcode ();

INT16 _format ( UINT8 drv ,  UINT8 fstype);
INT16 _deleteall (LPSTR filename);

//��ȡ��ǰseek��λ�ö�Ӧ��cluster ID
INT32 _GetCluster(INT16 fd);
//cluster ID to sector address
INT32 Clus2Phy( UINT16 dsk ,  UINT32 cl_no);

/****************************************************************************/
//delete part of file,from offset,delete length byte
//but the file system will change the offset and length to cluster number 
INT16 DeletePartFile(INT16 fd, UINT32 offset, UINT32 length);
//insert the src file to tag file
INT16 InserPartFile(INT16 tagfd, INT16 srcfd, UINT32 tagoff, UINT32 srclen);
//split tag file into two file, one is remain in tag file, one is in src file
INT16 SplitFile(INT16 tagfd, INT16 srcfd, UINT32 splitpoint);

//һ��cluster�ж��ٸ�sector
UINT16 GetSectorsPerCluster(UINT16 dsk);

//select unicode page,the code page is defined above
UINT16 ChangeCodePage(UINT16 wCodePage);
UINT16 GetCodePage(void);
INT16 ChangeUnitab(struct nls_table *st_nls_table);

//is it fat16 or fat32?
INT16 checkfattype (UINT8 disk);

//updata dir information bu not close the dir
INT16 UpdataDir(INT16 fd);

//if the file is destroy for some reason,this function will repair the file
//note it can't deal with some complicated condition
INT16 FileRepair(INT16 fd);

//֧��С�����Ĵ��̵ĸ�ʽ�����̶���ʽ��ΪFAT16������MBR����Ҫָ�����̵����sector����ʵ��sector��
//��Ϊ���ǵ��ļ�ϵͳ��֧��FAT12��������������С��16MBʱ��PC��Ѵ�������FAT12�����������������������
//��С��16MB�Ĵ��̸�ʽ��Ϊ����16Mb����ָ����С����������ָ������ʵ�ʵĿռ��С
INT16 sformat(UINT8 drv, UINT32 totalsectors, UINT32 realsectors);

//�õ��򿪵��ļ������Ĵ��̣�����ֵ0,1,2...
INT16 GetDiskOfFile(INT16 fd);

//����ָ����С���ļ���size is byte size
//���ظ��ļ��ľ��
//����ʧ�ܣ�����-1
INT16 CreatFileBySize(LPSTR path ,UINT32 size);

#endif //# end __FS_INTERFACE_h__
