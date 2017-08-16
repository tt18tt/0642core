#include "libtty2006.h"
#include "libdriver2006.h"

#include "mifare511.h"

#define TESTING_BLOCK 10
unsigned char newkey[]="\xFF\xFF\xFF\xFF\xFF\xFF";
unsigned char newdate[]="0123456789abcdef";

int Read_pn512()
{
	int curx, cury;
	int keyread;
	unsigned char ch_ft_display[50],bcode[255];
	unsigned char cntr, card_read_ok;
	unsigned long rcode;
	unsigned short ATQ;
	unsigned char SAK;
	unsigned char PICCsnr[20], gcode, * tptr, UIClen;
	unsigned char rdata[512];
	unsigned char wdate[512];
	unsigned char ReadCard_Time[3];
	unsigned char ShowBuf[20];
	unsigned char Showsucc[20];
	char i;
	char time=0;
	int j=0,readtime=0,ReadCardOK=0;
	
  memset(ReadCard_Time,0,3);

re_readm1card:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次读卡");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.循环10次读卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"返回键返回上级");

do
{
  keyread = TERM_getkey(KEEPWAITING);
			if(keyread == 0x0031)
				{
					readtime=1;
					goto Read512Card;
				}
			else if(keyread == KEY_ESC)
					return -1;
		   else if(keyread == 0x0032)
		   	{
		    	readtime=10;
					goto Read512Card;
				}		
  }while(1);

Read512Card:  
	sprintf(ShowBuf, "将进行读卡%d 次", readtime);
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,ShowBuf);
	TERM_gotoxy(0,3);
	TERM_display(0,"确认键进行读卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"返回键退出菜单");

	do 
	{
		keyread = TERM_getkey(KEEPWAITING);
		if (keyread == KEY_ESC)
		{
			return -1;
		}
		else if (keyread == KEY_ENTER)    //确认键
			break;
	}
	while (1);

	rcode = InitMifare();
	if (rcode) 
	{

		TERM_clearscreen();
		TERM_gotoxy(0,4);
   	TERM_display(0,"设备初始化失败");
   	Beep_AC(200);
		usleep(500);
		Beep_AC(200);
		MifareOff();
		return 0;
	}
  
ReadCardAgain:  	
  sprintf(ShowBuf, "将进行第%d 次读卡", j);
  j++;
		TERM_clearscreen();
		TERM_gotoxy(0,1);
   	TERM_display(0,ShowBuf);
		TERM_gotoxy(0,4);
   	TERM_display(0,"请在8秒内读卡");

	for(card_read_ok = 0, cntr = 8; cntr && !card_read_ok;)
	{
		rcode = CardTypeARequest(PICC_REQIDL, (unsigned char*)&ATQ);
		if (!rcode) 
		{
			card_read_ok = 1;
			memset(PICCsnr,0,20);
			for  (gcode = PICC_ANTICOLL1, tptr = PICCsnr, UIClen = 0;gcode < 0x99;gcode +=2, tptr+=4, UIClen++) 
			{
				if((rcode = CardTypeAAnticoll(gcode,tptr)))  
				{
					printf("pic anticoll %02x error!--%08X\n", gcode, rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"pic anticoll %02x error!--%08X\n", gcode, rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
				}
				if( (rcode = CardTypeASelect(gcode,tptr,&SAK)))  
				{
					printf("pic select %02x error!--%08X\n", gcode, rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"pic select %02x error!--%08X\n", gcode, rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
 					TERM_clearscreen();
	      	TERM_gotoxy(0,3);
   	      TERM_display(0,ch_ft_display); 
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
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

			printf("SAK is %02X :", SAK);
			printf("The CARD Serial Number is ");
			memset(bcode,0,255);
			for (i = 0; i <UIClen; i++)
			{
				printf(" %02x", PICCsnr[i]);
				sprintf(bcode + 3*i," %02X", PICCsnr[i]);
			}
		   printf(" (hex)\n");

			if ((UIClen == 7) && (SAK == 0) && (ATQ == 0x0044)) 
			{
				//
				// Read/Write tests for Mifrare UltraLite cards
				//
				printf("suppose we got UltraLite:\n");
				for (i = 0; i < 16; i +=4) 
				{
					rcode = CardMFCRead16Bytes(i, rdata+i*4);
					if (rcode) 
					{
						printf("Read error for block #%d: %08X\n", i, rcode);
						memset(ch_ft_display,0,50);
						sprintf(ch_ft_display,"Read error for block #%d: %08X\n", i, rcode);
						ch_ft_display[strlen(ch_ft_display)] = '\0';
 					  TERM_clearscreen();
	      	  TERM_gotoxy(0,3);
   	        TERM_display(0,ch_ft_display); 
						Beep_AC(200);
						usleep(500);
						Beep_AC(200);
						MifareOff();
						return 0;
					}
				}
				for (i = 0; i <64; i++) 
				{
					if (!(i % 16))
						printf("\n");
					printf("%02x ", rdata[i]);
				}
				printf("\n");
				//
				// Only first 4 bytes are written actually 
				// for Mifrare UltraLite cards
				//
				memset(wdate,0,512);
				memcpy(wdate,"broslab",7);
				rcode = CardMFCWrite16Bytes(10, wdate);
				if (rcode) 
				{
					printf("Write error for block #10: %08X\n", rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"Write error for block #10: %08X\n", rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
					TERM_clearscreen();
	      	TERM_gotoxy(0,3);
   	      TERM_display(0,ch_ft_display);
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
				}
			}
			else 
			{
				printf("trying Mifare standard:\n");
				printf("testing block: %d\n", TESTING_BLOCK);
				 //
				// tests for Mifrare standard cards (S50/S70)
				//
				// step 3. 3-Pass Authentication
				//
				rcode = CardMFCAuthKey( PICC_AUTHENT1A, PICCsnr, newkey, TESTING_BLOCK);
				if (rcode) 
				{
					printf("Auth error: %08X\n", rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"Auth error: %08X\n", rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
 					TERM_clearscreen();
	      	TERM_gotoxy(0,3);
   	      TERM_display(0,ch_ft_display);         
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
				}
				//
				// step 4. Read the sector that contains the test block (4 blocks)
				//
				for (i = 0; i < 4; i ++) 
				{
					rcode = CardMFCRead16Bytes((TESTING_BLOCK & 0xFC) + i, rdata+i*16);
					if (rcode) 
					{
						printf("Read error for block #%d: %08X\n", (TESTING_BLOCK & 0xFC) + i, rcode);
						memset(ch_ft_display,0,50);
						sprintf(ch_ft_display,"Read error for block #%d: %08X\n", (TESTING_BLOCK & 0xFC) + i, rcode);
						ch_ft_display[strlen(ch_ft_display)] = '\0';
 					  TERM_clearscreen();
	      	  TERM_gotoxy(0,3);
   	        TERM_display(0,ch_ft_display); 
						Beep_AC(200);
						usleep(500);
						Beep_AC(200);
						MifareOff();
						return 0;
					}
				}
			   printf("Block #%d-%d: ", (TESTING_BLOCK & 0xFC), (TESTING_BLOCK & 0xFC)+3);
				for (i = 0; i <64; i++) 
				{
					if (!(i % 16))
						printf("\n[%02d]: ", (TESTING_BLOCK & 0xFC)+(i/16));
					  printf("%02x ", rdata[i]);
				}
				   printf("\n");
				//
				// step 5. Write 1 block as a value block. c.f. S50/S70 datasheet
				//
				printf("Wrt ");
				rcode = CardMFCWrite16Bytes(TESTING_BLOCK,newdate);
				if (rcode) 
				{
					printf("error for block #%d: %08X\n", TESTING_BLOCK, rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"error for block #%d: %08X\n", TESTING_BLOCK, rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
 					TERM_clearscreen();
	      	TERM_gotoxy(0,3);
   	      TERM_display(0,ch_ft_display); 
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
				}
				//
				// step 5.1 Read it back. !! absolutely needed, could not proceed 
				// otherwise
				//
				rcode = CardMFCRead16Bytes(TESTING_BLOCK, rdata);
				if (rcode) 
				{
					printf("re-read error: %08X\n", rcode);
					memset(ch_ft_display,0,50);
					sprintf(ch_ft_display,"re-read error: %08X\n", rcode);
					ch_ft_display[strlen(ch_ft_display)] = '\0';
 					TERM_clearscreen();
	      	TERM_gotoxy(0,3);
   	      TERM_display(0,ch_ft_display); 
					Beep_AC(200);
					usleep(500);
					Beep_AC(200);
					MifareOff();
					return 0;
				}
				printf("block #%d: ", TESTING_BLOCK);
				for (i = 0; i <16; i++)
					printf("%02x ", rdata[i]);
				printf("\n");
			}
      
      if(j<readtime)
      	{
      		ReadCardOK++;
      	goto ReadCardAgain;
        }
      	else
      		{
      			ReadCardOK++;
		//	Beep_AC(500);
		  sprintf(Showsucc,"读卡成功%d 次 ",ReadCardOK);
		  TERM_clearscreen();
	    TERM_gotoxy(0,3);
   	  TERM_display(0,Showsucc); 
   	  TERM_gotoxy(0,4);
   	  TERM_display(0," 按任意键返回主菜单"); 
        Beep_AC(200);
				usleep(500);
				Beep_AC(200);
			  MifareOff();
      	do{
	        	keyread = TERM_getkey(KEEPWAITING);
		        if (keyread >0)
		            {
		            	return 1;
	               	}
          	}	while (1);
		}
		}

		memset(ch_ft_display,0,50);
		sprintf(ch_ft_display," 请在%d秒内读卡...   ",cntr - 1);		
		ch_ft_display[strlen(ch_ft_display)] = '\0';
			//TERM_clearscreen();
	    TERM_gotoxy(0,2);
   	  TERM_display(0,ch_ft_display); 
   	  TERM_gotoxy(0,3);
   	  TERM_display(0,"请刷卡"); 
		sleep(1);
		cntr --;
		Beep_AC(200);

	}
	if (!card_read_ok)
	{
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);
			TERM_clearscreen();
	    TERM_gotoxy(0,2);
   	  TERM_display(0,"没有读到有效卡");
		MifareOff();
		return 0;
	}

	return 0;
}


