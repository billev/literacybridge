#ifndef  __Mp3__
#define	  __Mp3__

typedef struct{
	unsigned long   firstframe;			//data����ʼλ��
	unsigned long   filesize;			//�ļ�����
	unsigned long   frametime;			//һ֡��ʱ��
	unsigned int 	bitrate;			//biterate kbps
	unsigned int 	mpegtype;			// mpeg type
	unsigned int 	layer;				// layer type
	unsigned int 	samplerate; 		//samplerate hz
	unsigned int 	framesize;			//һ֡�ĳ���
//	unsigned long 	fisrtframeaddr;		
	unsigned long 	total_time;			//�ļ�����ʱ�� //����
}st_MP3FILE_INFO;

typedef struct Node
{
	unsigned long 	MusicFilePath;	//n Word
	unsigned int 	FileID;			//1,2,3......n
	struct 			Node *Next;
	struct 			Node *Back;
	unsigned int 	Param;			//Already play or not
}List;

extern unsigned int Snd_MP3_Status;

extern int Snd_MP3_PlayMemory(unsigned long start_addr, unsigned long end_addr);
extern int Snd_PlayMP3FAT( const char * filename );		//added by chengye 2007/7/2
extern int Snd_PlayMP3FAT_Part(char *filename, unsigned long offset, unsigned long datalength);
extern void PlayNextFile( void );
extern int GetMp3Info( const char * FilePath, st_MP3FILE_INFO *mp3file_info );
extern void MediaPlayer_SetA( void );
extern void MediaPlayer_SetB( void );
extern void MediaPlayer_ClearAB( void );
extern void MediaPlayer_PlayAB( void );
//extern void MediaPlayer_PlayABDriver(void);						//delete by chengye 2007/7/2
extern int Snd_PlayMP3FATTimeOffset(const char *filename, unsigned long Offset, unsigned long length, unsigned int flag);//added by chengye 2007/7/2

extern void MediaPlayer_PlayBack(void);
extern void MediaPlayer_PlayNext(void);
extern void MediaPlayer_SetPlayMode(unsigned int mode);
extern int Snd_MP3Player_SeekTime(long second,int orientation);
extern int Snd_MP3Player_SeekLength(long mp3length,int orientation);
extern int MediaPlayer_PlayList(List *Head,unsigned int FileID);
extern int GetCurPlayTime(void);
extern unsigned long GetCurPlayms(void);

extern void Snd_MP3Speech_Pause(void);
extern void Snd_MP3Speech_Resume(void);
//Error Return Value:
#define C_PLAY_SHUFFLE 	0x01			//����
#define C_PLAY_NORMAL 	0x02			//˳��
#define C_PLAY_ONE 		0x04			//����
#define C_PLAY_REPEAT 	0x08			//ѭ��
#define C_PLAY_LISTPLAY	0x10			//PlayList�ı�־

//MP3���⿪��2048��Buffer���ݴ����ݵĺ궨��:

#define C_MP3FATBUFFERSIZE		1024	//MP3��ȡFAT��Bufferһ���С, �������Ƹ���Buffer��Size.
#define C_MP3FATFIRSTBUFFER		1		//��ʾ������MP3FAT�ĵ�һ��Buffer��д����
#define C_MP3FATSECONDBUFFER	2		//��ʾ������MP3FAT�ĵڶ���Buffer��д����
#define C_MP3FIRSTBUFFERFULL	11		//��һ��Buffer������
#define C_MP3SECONDBUFFERFULL	12		//�ڶ���Buffer������
//MP3�⿪Buffer�ĺ궨�����

#define C_Forward				0		//��ǰ(mp3��β��)����
#define C_Backward				1		//���(mp3����ʼ)����

#if 0			//һЩϵͳ����û�ж�������ĺ꣬�ڴ˼���

#ifndef LPSTR
	typedef unsigned long LPSTR;
#endif

#ifndef LPTR
	typedef unsigned long LPTR;
#endif
#endif

#ifndef SP2LP
	#define SP2LP(b) \
	(LPTR)(unsigned int) b
#endif

#ifndef LB2LP
	#define LB2LP(label)	({					\
	register union {						\
		unsigned long oval;					\
		unsigned int ival[2];				\
	}__t;									\
	__asm__ ("%0 = offset %2\n"				\
	"\t%1 = seg %2"							\
	:										\
	:"r"(__t.ival[0]),"r"(__t.ival[1]),		\
	"p"(&label)								\
	);										\
	__t.oval;								\
})
#endif

#endif
