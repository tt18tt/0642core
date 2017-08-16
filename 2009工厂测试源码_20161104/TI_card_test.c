#include "mifare530.h"
#include "iso-15693.h"


unsigned long Read_Ticard_Block()
{
	unsigned char UID[10];
	short i, l_wStatus;
	unsigned short RecLength;
	Tag_Info TagInfo;
	int nBlock=1;
	unsigned char SendData[32], RecData[32];
	unsigned char TempStatus;

	char bcode[255],bcode1[255];

	//flags 26 means: Request is addressed, Invertory mode, High data rate.
	l_wStatus=ISO15693_FindOneCard(0x26, 0x00, &TagInfo);
	if(l_wStatus == MI_OK)
	{
		memset(UID,0,10);
		printf("DSFID:%02X\n", TagInfo.DSFID);
		printf("UID:");
		for(i=0; i<8; i++)
		{
			printf(" %02X", TagInfo.UID[i]);
		}
		printf("\n");
		memset(RecData,0,32);
		memset(SendData,0,32);
		memcpy(SendData,"1234",4);
		SendData[4] = '\0';

			TempStatus = ISO15693_ReadBlock(0x62, TagInfo.UID, nBlock, 0x01, &RecLength, RecData);
			if(TempStatus == MI_OK )
			{
				printf("%c%c%c%c\n",RecData[1],RecData[2],RecData[3],RecData[4]);
				Beep_AC(200);
				memset(bcode,0,255);
				strcpy(bcode,"读卡成功!\r\n");
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return MI_OK;
			}
			else
			{
				Beep_AC(200);
				usleep(500);
				Beep_AC(200);
				usleep(500);
				Beep_AC(200);							
				memset(bcode,0,255);
				strcpy(bcode,"读卡失败!\r\n");
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}			
     
	}

	return 1;
}

void Read_Ticard_test()
{
	int time1=9;
	int time_read=0;
	char buffer[100];
	char buffer1[100];	
	int time_ok=0;
	int time_error=0;
	int i;
	unsigned long TempStatus,card_read_ok;
	unsigned long rcode;
	int keych;


re_readticard:
		TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次读卡测试");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.10次读卡测试");
	TERM_gotoxy(0,3);
	TERM_display(0,"3.100次读卡测试");	
	TERM_gotoxy(0,4);
	TERM_display(0,"4.返回键返回");		
	while(1)
	{
		time_ok=0;
		time_error=0;
		keych =TERM_getkey(KEEPWAITING);
		if(keych==0x31)
			{
			time_read=1;
			break;
		  }
		else if(keych==0x32)
			{
			time_read=10;
			break;
		}
		else if(keych==0x33)
			{
			time_read=100;
			break;
		  }
		else if(keych==KEY_ESC)
			return 0;			
		}

	card_read_ok=0;

	for(i=0;i<time_read;i++)
	{
		  time1=9;
			if(ISO15693_VCD_On() != MI_OK)
			{
				ISO15693_VCD_Off();
				TERM_clearscreen();
				TERM_displayxy(1,1,NORMALDISPLAY,"设备初始化错误!");
				sleep(1);
				goto re_readticard;
			}
			
			while (1)
			{
				TERM_clearscreen();
				if(time1<=0)
				{
					card_read_ok = 0;
					ISO15693_VCD_Off();
					time_error++;
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
			  	TERM_displayxy(1,2,NORMALDISPLAY,buffer);
			  	TERM_displayxy(1,3,NORMALDISPLAY,buffer1);
			  	usleep(100000);
			  	break;
				}
				
				sprintf(buffer,"请刷卡.........%d",time1);	
				TERM_displayxy(1,2,NORMALDISPLAY,buffer);
				TempStatus = Read_Ticard_Block();
				if(TempStatus == MI_OK )
				{
					card_read_ok = 1;
					ISO15693_VCD_Off();
					time_ok++;
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
			  	TERM_displayxy(1,2,NORMALDISPLAY,buffer);
			  	TERM_displayxy(1,3,NORMALDISPLAY,buffer1);
			  	usleep(100000);
			  	break;					
				}
				else if (TempStatus == -1)
				{
					card_read_ok = 1;
					ISO15693_VCD_Off();
					time_error++;
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
			  	TERM_displayxy(1,2,NORMALDISPLAY,buffer);
			  	TERM_displayxy(1,3,NORMALDISPLAY,buffer1);
			  	usleep(100000);
			  	break;	
				}
				else if (TempStatus == 1)
				{
					if(card_read_ok==0)time1--;
					Beep_AC(100);
					sleep(1);
				}

			}
		
	}
	  TERM_gotoxy(0,4);
	  TERM_display(0,"按返回键返回");		
		keych =TERM_getkey(KEEPWAITING);
		if(keych>0)
			{			
			goto re_readticard;
		  }
}