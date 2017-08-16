#include "mifare530.h"
#include "iso-15693.h"

#define TESTING_BLOCK 10

unsigned char newkey[]="\xFF\xFF\xFF\xFF\xFF\xFF";
unsigned char newdate[]="\x55\x55\x55\x55\xAA\xAA\xAA\xAA\x55\x55\x55\x55\x01\xFE\x01\xFE";

#define u_puts puts
#define u_printf printf

unsigned long Read_M1card_Block()
{
	unsigned long rcode;
	unsigned short ATQ;
	unsigned char SAK;
	unsigned char PICCsnr[20], gcode, * tptr, UIClen;
	unsigned char rdata[512];
	unsigned char wdate[512];
	char i;
	unsigned char bcode[255],bcode1[255];

	memset(bcode,0,255);

	rcode = CardTypeARequest(PICC_REQIDL, (unsigned char*)&ATQ);
	if (!rcode) 
	{
		memset(PICCsnr,0,20);
		for  (gcode = PICC_ANTICOLL1, tptr = PICCsnr, UIClen = 0;gcode < 0x99;gcode +=2, tptr+=4, UIClen++) 
		{
			if((rcode = CardTypeAAnticoll(gcode,tptr)))  
			{
				u_printf("pic anticoll %02x error!--%08X\n", gcode, rcode);
				memset(bcode,0,255);
				sprintf(bcode,"pic anticoll %02x error!--%08X\n", gcode, rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
			if( (rcode = CardTypeASelect(gcode,tptr,&SAK)))  
			{
				u_printf("pic select %02x error!--%08X\n", gcode, rcode);
				memset(bcode,0,255);
				sprintf(bcode,"pic select %02x error!--%08X\n", gcode, rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
			if ((SAK & 0x04) != 0x04)
				break;
		}
		if (UIClen == 0)
			UIClen = 4;
		else if (UIClen == 1) 
		{
			UIClen = 7;
			memcpy(PICCsnr, PICCsnr + 1, 7);
		}
		else 
		{
			UIClen = 10;
			memcpy(PICCsnr+4, PICCsnr + 5, 7);
			memcpy(PICCsnr, PICCsnr+1, 11);
		}
		
		u_printf("SAK is %02X ", SAK);
		u_printf("The CARD Serial Number is ");
		for (i = 0; i <UIClen; i++)
		{
			u_printf(" %02x", PICCsnr[i]);
		}
		u_printf(" (hex)");

		if ((UIClen == 7) && (SAK == 0) && (ATQ == 0x0044)) 
		{
			//
			// Read/Write tests for Mifrare UltraLite cards
			//
			u_printf("suppose we got UltraLite:\n");
			for (i = 0; i < 16; i +=4) 
			{
				rcode = CardMFCRead16Bytes(i, rdata+i*4);
				if (rcode) 
				{
					memset(bcode,0,255);
					u_printf("Read error for block #%d: %08X\n", i, rcode);
					sprintf(bcode,"\r\nRead error for block #%d: %08X\n", i, rcode);
					Beep_AC(100);
					usleep(500);
					Beep_AC(100);
					TERM_clearscreen();
					TERM_displayxy(0,1,NORMALDISPLAY,bcode);
					return -1;
				}
			}
			for (i = 0; i <64; i++) 
			{
				if (!(i % 16))
					u_printf("\n");
				u_printf("%02x ", rdata[i]);
			}
			u_printf("\n");
			//
			// Only first 4 bytes are written actually 
			// for Mifrare UltraLite cards
			//
			memset(wdate,0,512);
			memcpy(wdate,"broslab",7);
			rcode = CardMFCWrite16Bytes(10, wdate);
			if (rcode) 
			{
				memset(bcode,0,255);
				u_printf("Write error for block #10: %08X\n", rcode);
				sprintf(bcode,"Write error for block #10: %08X\n", rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
//			memset(bcode,0,255);
//			strcpy(bcode,"读写卡成功!\r\n");
//			TERM_clearscreen();
//			TERM_displayxy(0,1,NORMALDISPLAY,bcode);
		}
		else 
		{
			u_printf("trying Mifare standard:\n");
			u_printf("testing block: %d\n", TESTING_BLOCK);
			 //
			// tests for Mifrare standard cards (S50/S70)
			//
			// step 3. 3-Pass Authentication
			//
			memset(bcode,0,255);
			rcode = CardMFCAuthKey( PICC_AUTHENT1A, PICCsnr, newkey, TESTING_BLOCK);
			if (rcode) 
			{
				u_printf("Auth error: %08X\n", rcode);
				sprintf(bcode,"Auth error: %08X\n", rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
			//
			// step 4. Read the sector that contains the test block (4 blocks)
			//
			memset(bcode,0,255);
			for (i = 0; i < 4; i ++) 
			{
				rcode = CardMFCRead16Bytes((TESTING_BLOCK & 0xFC) + i, rdata+i*16);
				if (rcode) 
				{
					u_printf("Read error for block #%d: %08X\n", (TESTING_BLOCK & 0xFC) + i, rcode);
					sprintf(bcode,"Read error for block #%d: %08X\n", (TESTING_BLOCK & 0xFC) + i, rcode);
					Beep_AC(100);
					usleep(500);
					Beep_AC(100);
					TERM_clearscreen();
					TERM_displayxy(0,1,NORMALDISPLAY,bcode);
					return -1;
				}
			}
			u_printf("Read Block #%d-%d: ", (TESTING_BLOCK & 0xFC), (TESTING_BLOCK & 0xFC)+3);
			for (i = 0; i <64; i++) 
			{
				if (!(i % 16))
				{
					u_printf("\n[%02d]: ", (TESTING_BLOCK & 0xFC)+(i/16));
				}
				u_printf("%02x ", rdata[i]);
			}
			u_printf("\n");
			//
			// step 5. Write 1 block as a value block. c.f. S50/S70 datasheet
			//
			rcode = CardMFCWrite16Bytes(TESTING_BLOCK,newdate);
			if (rcode) 
			{
				memset(bcode,0,255);
				u_printf("error for block #%d: %08X\n", TESTING_BLOCK, rcode);
				sprintf(bcode,"error for block #%d: %08X\n", TESTING_BLOCK, rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
			u_printf("Wrt ");
			//
			// step 5.1 Read it back. !! absolutely needed, could not proceed 
			// otherwise
			//
			rcode = CardMFCRead16Bytes(TESTING_BLOCK, rdata);
			if (rcode) 
			{
				memset(bcode,0,255);
				u_printf("re-read error: %08X\n", rcode);
				sprintf(bcode,"re-read error: %08X\n", rcode);
				Beep_AC(100);
				usleep(500);
				Beep_AC(100);
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return -1;
			}
			u_printf("block #%d: ", TESTING_BLOCK);
			for (i = 0; i <16; i++)
			{
				u_printf("%02x ", rdata[i]);
			}
			u_printf("\n");
//			memset(bcode,0,255);
//			strcpy(bcode,"读写卡成功!\r\n");
//			TERM_clearscreen();
//			TERM_displayxy(0,1,NORMALDISPLAY,bcode);
		}

		Beep_AC(200);
		return MI_OK;
	}

	return 1;
}

void Read_M1card_test()
{
	int time1=9;
	unsigned long TempStatus,card_read_ok;
	unsigned long rcode;
	int keych;
	char buffer[100];
	char buffer1[100];
	int i;
	int time_read=0;
	int time_ok=0;
	int time_error=0;

m1card_menu:
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
		key =TERM_getkey(KEEPWAITING);
		if(key==0x31)
			{
			time_read=1;
			break;
		  }
		else if(key==0x32)
			{
			time_read=10;
			break;
		}
		else if(key==0x33)
			{
			time_read=100;
			break;
		  }
		else if(key==KEY_ESC)
			return 0;			
		}
	
for(i=0;i<time_read;i++)
{	
re_readm1card:

	card_read_ok=0;
	time1=9;

			if( InitMC530(0) != MI_OK) 
			{
				MC530Off();
				TERM_clearscreen();
				TERM_displayxy(1,1,NORMALDISPLAY,"设备初始化错误!");
				sleep(1);
				goto re_readm1card;
			}

			ResetAllCard();
			
			while (1)
			{
				TERM_clearscreen();
				if(time1<=0)
				{
					card_read_ok = 0;
					MC530Off();
					time_error++;
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
					TERM_clearscreen();
			  	TERM_displayxy(1,2,NORMALDISPLAY,buffer);
			  	TERM_displayxy(1,3,NORMALDISPLAY,buffer1);
			  	usleep(100000);
			  	break;
				}
				
				sprintf(buffer,"请刷卡.........%d",time1);	
				TERM_displayxy(1,1,NORMALDISPLAY,buffer);
				TempStatus = Read_M1card_Block();
				if(TempStatus == MI_OK )
				{
					time_ok++;
					card_read_ok = 1;
					//Beep_AC(500);
					MC530Off();
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
					TERM_clearscreen();
			  	TERM_displayxy(1,2,NORMALDISPLAY,buffer);
			  	TERM_displayxy(1,3,NORMALDISPLAY,buffer1);
			  	usleep(100000);
					break;
				}
				else if (TempStatus == -1)
				{
					card_read_ok = 1;
					time_error++;
					MC530Off();
					memset(buffer,0,100);
					memset(buffer1,0,100);
					sprintf(buffer,"刷卡成功%d次",time_ok);	
					sprintf(buffer1,"刷卡失败%d次",time_error);	
					TERM_clearscreen();
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
	while(1)
	{
		TERM_displayxy(1,4,NORMALDISPLAY,"请按返回键返回");
		key =TERM_getkey(KEEPWAITING);
		if(key==KEY_ESC)
			{
			time_read=0;
    	time_ok=0;
    	time_error=0;
			break;
		  }
		}
	goto m1card_menu;
}