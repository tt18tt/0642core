#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h> 
#include <netinet/in.h>
#include <time.h>



#include "wifi.c"
#include "TI_card_test.c"
#include "M1card_test.c"
#include "basic_test.c"
#include "cpucard_test.c"
#include "gprs_test.c"
#include "udisk_test.c"


#define FALSE 0
#define TRUE 1




void display_string(char *str_info)
{
	printf("\n*****\n");
	printf(str_info);
	printf("\n*****\n");

	return;
}

int str_n_cmp(char *a,char * b)//详细比较字符串a和b
{
	   //计算字符串长度
	if((strlen(a)==strlen(b))&&(strncmp(a,b,strlen(a))==0))//首先确定长度是否一致,再比较各位是否一致
		return 0;
	
	else 
		return -1;
}

int PressAnyKey()
{
  			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					return 1;
				}
			}
	}
	
int strtoint(char *send_string)
{
	int i,lg,lw=0,lt;

	display_string(send_string);
	
	lt = strlen(send_string);
	memset(all,0,10);
	for (i=0;i<lt;i++)
	{
		if(send_string[i]>='a'&&send_string[i]<'z')
			{
		  lg = send_string[i];
		  lw = lw + lg;
	    }
	}
	sprintf(all,"%ld",lw);
	all[strlen(all)] = '\0';
	return 0;
}



static void set_serial_raw_1(int fd, int with_hw_handshake) 
{
	struct termios newtio;
	
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD ;
	if (with_hw_handshake) 
	newtio.c_cflag  |= CRTSCTS;
	
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	
	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;
	
	newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 0;   /* no blocking, return immediately if none */
	
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
}

static void set_serial_raw(int fd) 
{
	struct termios newtio;
	
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD ;
	
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
}

int SendCommand_1(int fd232,char *send,char *recv)
{
	int res,c;
	
	res = read(fd232,recv,255);   /* clear com */
	
	printf("\r\n232 error\r\n");
	
	if ( !write(fd232, send, strlen(send)) ) //ascii mode
		puts("232 ASCII mode sent\r\n");
	
	sleep(2);
	
	res = read(fd232,recv,255);   /* return after 0~255 chars have been input */
	if (res) 
	{  //got sth. from RS485 line
		recv[res]=0;               /* so we can printf... */
		printf("rx:%s:%d\r\n", recv,res);
		for (c = 0; c < res; c++) 
		{
			int ch = recv[c];
			printf("%02X ", ch);
		}
		printf("\r\n");
	}
	printf("\r\n232 error_11\r\n");
	return 0;
}

int SendCommand(int fd232,char *send)
{
	char recv[255];
	int res;
	
	res = read(fd232,recv,255);   /* clear com */
	
	if ( !write(fd232, send, strlen(send)) ) //ascii mode
		puts("232 ASCII mode sent\r\n");
	
	return 0;
}

int RevCommand(int fd232,char *recv)
{
	int res,c;
	
	res = read(fd232,recv,255);   /* return after 0~255 chars have been input */
	if (res) 
	{  //got sth. from RS485 line
		recv[res]=0;               /* so we can printf... */
		printf("rx:%s:%d\r\n", recv,res);
		for (c = 0; c < res; c++) 
		{
			int ch = recv[c];
			printf("%02X ", ch);
		}
		printf("\r\n");
	}
	return 0;
}

int ReadNumeric_(char *rBuf, int intDigit, int PosX, int PosY)
{
	int i, j, keych;
	char xBuf[20];

	i = 0;
	memcpy(xBuf, rBuf, intDigit);
	xBuf[intDigit] = '\0';
	while(1)
	{
		if (i < intDigit)
			rBuf[i] = '_';
			TERM_displayxy(PosX, PosY, NORMALDISPLAY, rBuf);
			keych = TERM_getkey(KEEPWAITING);
			switch(keych)
			{
				case KEY_UP:
					if (i > 0)
					{
						if (i < intDigit)
							rBuf[i] = ' ';
						i --;
						while(xBuf[i] != ' ' && i > 0)
							i--;
					}
					break;
				case KEY_DOWN:
					if (i < intDigit - 1)
					{
						rBuf[i] = ' ';
						i ++;
						while(xBuf[i] != ' ' && i < intDigit)
							i++;
					}
					break;
				case KEY_ENTER:
					if (i < intDigit)
						rBuf[i] = ' ';
					TERM_displayxy(PosX, PosY, NORMALDISPLAY, rBuf);
					return i;
				case 67:
					if (i < intDigit)
						rBuf[i] = ' ';
					i--;
					while(xBuf[i] != ' ' && i > 0)
						i--;
					if (i < 0)
						return -1;
					break;
				case KEY_ESC:
					return -1;
				default:
					if ((keych-48) >= 0 && (keych-48)  <= 9 && i < intDigit)
					{
						rBuf[i]	= keych;
						i++;
						while(xBuf[i] != ' ' && i < intDigit)
							i++;
					}
					break;
			}
	}
}

int ReadString_term(char *rBuf, int intDigit, int PosX, int PosY, int blnPass)
{
	int lk=0,ln=0;
	int i, keych, start, lstkey, tiskey;
	char xBuf[20];
	char key_char[11][40] = {".,'?!\"-()@/:_;+&%*=<>$[]{}\\~^#| `\0",
							"0\0",
							"1\0",
							"2ABCabc\0",
							"3DEFdef\0",
							"4GHIghi\0",
							"5JKLjkl\0",
							"6MNOmno\0",
							"7PQRSpqrs\0",
							"8TUVtuv\0",
							"9WXYZwxyz\0"};

	i = 0;
	start = 0;
	lstkey = -1;
	memcpy(xBuf, rBuf, intDigit);
	xBuf[intDigit] = '\0';
	while(1)
	{
		if (i < intDigit)
		{
			rBuf[i] = '_';
			xBuf[i] = '_';
		}
		if (blnPass != 0)
		{
			TERM_displayxy(PosX, PosY, NORMALDISPLAY, xBuf);
		}
		else
		{
			TERM_displayxy(PosX, PosY, NORMALDISPLAY, rBuf);
		}

		keych = TERM_getkey(1);
		switch(keych)
		{
			case KEY_ENTER:
				if (i < intDigit)
				{
					rBuf[i] = ' ';
					xBuf[i] = ' ';
				}
				if (blnPass != 0)
				{
					TERM_displayxy(PosX, PosY, NORMALDISPLAY, xBuf);
				}
				else
				{
					TERM_displayxy(PosX, PosY, NORMALDISPLAY, rBuf);
				}
				return i;
			case 67:
				if (i < intDigit)
				{
					rBuf[i] = ' ';
					xBuf[i] = ' ';
				}
				i--;
				lstkey = -1;
				if (i < 0)
					return -1;

				ln = 0;
				lk = 0;
				break;
			case KEY_ESC:
				return -1;
			default:
				if ((((keych-48) >= 0 && (keych-48) <= 9) || keych == KEY_F1) && i <= intDigit)
				{
					if (keych == KEY_F1)
						tiskey = 0;
					else
						//tiskey = keych + 1;
						tiskey = (keych-48) + 1;

					if(lstkey == tiskey && blnPass == 0)
					{
						if (start >= strlen(key_char[tiskey])) start = 0;
						rBuf[i - 1] = key_char[tiskey][start];
						start ++;
						lstkey = tiskey;
						if ( (keych-48)  == 0 ||  (keych-48)  == 1) lstkey = -1;
					}
					else if(i < intDigit)
					{
						start = 0;
						rBuf[i] = key_char[tiskey][start];
						xBuf[i] = '*';
						i ++;
						lstkey = tiskey;
						if ( (keych-48)  == 0 ||  (keych-48)  == 1) lstkey = -1;
					}

					ln = 1;
					lk = 0;
				}
				lk++;

				if ((ln == 1) && (lk > 20))
				{
					ln = 0;
					lk = 0;
					lstkey = -1;
				}

				break;
		}

		//printf("\r\n%d\r\n",lk);
	}
}

int my_menu_scanbarcode(unsigned char *funcstr,unsigned char**menustr,unsigned char n)
{      
	unsigned char number,i;
	unsigned char num,num_b;
	num=0;
	num_b=0;
	number=n;   
	
	TERM_clearscreen();
	for(i=0;i<number;i++)
	{
		TERM_gotoxy(1,i+1);
		if(i==0)
		{
		TERM_display(1,menustr[i]);
		}
		else
		TERM_display(0,menustr[i]); 
	}   		  	
	while(1)
	{    
		switch(TERM_getkey(KEEPWAITING))
		{
			case KEY_ESC:
				return  0;
			case KEY_ENTER:
				return num+1;
			case 0x31:
				return 1;
			case 0x32:
				return 2;
			case 0x33:
				return 3;	
			case KEY_DOWN:
				if(num==number-1)   
					continue;		 	   
				else
				{    
					TERM_gotoxy(1,(num+1));
					TERM_display(0,menustr[num]); 
					num++;
					TERM_gotoxy(1,(num+1));
					TERM_display(1,menustr[num]);  	
				}			
					continue;
			case KEY_UP:
				if(num==0)
					continue;			
				else
				{   
					TERM_gotoxy(1,(num+1));
					TERM_display(0,menustr[num]); 
					num--;
					TERM_gotoxy(1,(num+1));
					TERM_display(1,menustr[num]); 
				}		
					continue;
			default:
				continue;
		}  //end switch
	} //end while
}




void Receive_Irda_info()
{
	char ch_rev[1024];
	char buf[1024];
	int ln;
	
	while(1)
	{
		memset(buf,0,1024);
	
		printf("\r\naaa\r\n");
		
		RevCommand(fd232,buf);
		
		printf("\r\naaa\r\n");
		
		memset(ch_rev,0,1024);
		memcpy(ch_rev,buf,strlen(buf));
		ch_rev[strlen(ch_rev)] = '\0';

		TERM_clearscreen();
		TERM_gotoxy(0,1);
		TERM_display(0,"接收到的数据:");
		TERM_gotoxy(0,2);
		TERM_display(0,ch_rev);

		key = TERM_getkey(10);
		switch(key)
		{
			case KEY_ESC:
				return;
		}
	}

	return;
}

void Send_Irda_info()
{
	char ch_irda_send[20];

re_send_info:
	TERM_clearscreen();
	memset(ch_irda_send,0,20);

	TERM_gotoxy(0,1);
	TERM_display(0,"请输入发送数据:");

	if (ReadString_term(ch_irda_send,16,0,2,0) < 0) return;

	ch_irda_send[strlen(ch_irda_send)] = '\0';
	printf("\r\naa\r\n");
	printf(ch_irda_send);
	printf("aa\r\n");

	SendCommand(fd232,ch_irda_send);

	sleep(1);
	TERM_gotoxy(0,1);
	TERM_display(0,"发送成功!");
	sleep(1);
	key = TERM_getkey(10);
	switch(key)
	{
		case KEY_ESC:
			return;
	}
	goto re_send_info;
	
	return;
}

int Irda_test()
{
	unsigned char *sys_menu[]={"1:发送红外数据","2:接收红外数据"};	
	
	while(1)
	{   
		switch(my_menu_scanbarcode(" ",sys_menu,2))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			default:
				continue;
		}
	}

	return;
}

void Open_Irda_com()
{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在打开串口");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");

	system("echo 1 > /sys/devices/platform/s3c2410-buttons/irdamode");
	
	system("setio 10000 10000");
	sleep(1);
	system("setio 10000 00000");
	
	tcgetattr(0,&savedtio);
	fd232 = open(RS232_DEVICE, O_RDWR | O_NOCTTY ); 
	if (fd232 <0) 
	{
		perror(RS232_DEVICE);      
		tcsetattr(0,TCSANOW,&savedtio);
		return; 
	}
	
  	set_serial_raw(fd232);
	
	sleep(1);
	return;
}

void Server_COM_Setting_term_1()
{
	unsigned char Port[6];
	unsigned int retCode,iport;
	FILE *fp,*stream;
	int len,key;
	
	TERM_clearscreen();
	memset(Port,0,6);
	if((fp=fopen("/mnt/a/userapp/sys_txt/com_1","a+"))==NULL) 
	{
		fclose(fp);
		return;
	}
	fgets(Port,10,fp);
	fclose(fp);
	Port[strlen(Port)] = '\0';
	len = strlen(Port);
	if (len == 0)
	{
PORT_AGAIN:
		TERM_gotoxy(0,1);
		TERM_display(0,"请输入服务端口:");
		
		memcpy(Port, "     ", 5);
		Port[5] = '\0';
		if (ReadNumeric_(Port, 5, 0, 2) < 0) return;
		iport = atoi(Port);
		if( iport > 65535 )
		{
			TERM_gotoxy(0,4);
			TERM_display(0,"服务端口输入有误");
			
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);

			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					TERM_clearscreen();
					goto PORT_AGAIN;
				}
			}
			goto PORT_AGAIN;
		}

		memset(Port, '\0', 6);
		sprintf(Port, "%d", iport);

		Run("rm","/mnt/a/userapp/sys_txt/com_1",NULL,NULL);
	
		if((stream=fopen("/mnt/a/userapp/sys_txt/com_1","at+"))==NULL)
		{
			fclose(stream);
			return;
		}
		fputs(Port,stream);
		fsync(fileno(stream));
		rewind(stream);
		fclose(stream);

		TERM_gotoxy(0,4);
		TERM_display(0,"服务端口保存完毕");
		Beep_AC(1000);

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return;
			}
		}
		return;
	}
	else
	{
		TERM_clearscreen();

		TERM_gotoxy(0,1);
		TERM_display(0,"服务端端口:");
		TERM_gotoxy(0,2);
		TERM_display(0,Port);

		retCode=TERM_getkey(KEEPWAITING);
		if(retCode!=KEY_ENTER)
		{
			return;	
		}
		else
		{
			goto PORT_AGAIN;
		}
	}
}

void Server_IP_Setting_term_1()
{
	unsigned char buf[16],IP_addr[16],i;
	unsigned int retCode,iTmp[4];
	FILE *fp,*stream;
	int len,key;
	
	TERM_clearscreen();
	memset(IP_addr,0,16);
	if((fp=fopen("/mnt/a/userapp/sys_txt/ip_1","a+"))==NULL) 
	{
		fclose(fp);
		return;
	}
	fgets(IP_addr,20,fp);
	fclose(fp);
	IP_addr[strlen(IP_addr)] = '\0';
	len = strlen(IP_addr);
	if (len == 0)
	{
IP_AGAIN:
		TERM_gotoxy(0,1);
		TERM_display(0,"请输入IP地址:");

		memcpy(IP_addr, "   .   .   .   ", 15);
		IP_addr[15] = '\0';
		if (ReadNumeric_(IP_addr, 15, 0, 2) < 0) return;
	
		for(i = 0; i < 4; i++)
		{
			memcpy(buf, IP_addr + i * 4, 3);
			buf[3] = '\0';
			iTmp[i] = atoi(buf);
			if(atoi(buf) > 255)
			{
				TERM_gotoxy(0,4);
				TERM_display(0,"IP地址输入有误!");
				
				Beep_AC(200);
				usleep(500);
				Beep_AC(200);

				while(1)
				{
					key =TERM_getkey(KEEPWAITING);
					if (key > 0)
					{
						TERM_clearscreen();
						goto IP_AGAIN;
					}
				}
				goto IP_AGAIN;
			}
		}

		memset(IP_addr, '\0', 16);
		sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);

		Run("rm","/mnt/a/userapp/sys_txt/ip_1",NULL,NULL);
	
		if((stream=fopen("/mnt/a/userapp/sys_txt/ip_1","at+"))==NULL)
		{
			fclose(stream);
			return;
		}
		fputs(IP_addr,stream);
		fsync(fileno(stream));
		rewind(stream);
		fclose(stream);

		TERM_gotoxy(0,4);
		TERM_display(0,"IP地址保存完毕!");
		
		Beep_AC(1000);

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return;
			}
		}
		
		return;
	}
	else
	{
		TERM_clearscreen();
		TERM_gotoxy(0,1);
		TERM_display(0,"服务端IP地址:");
		TERM_gotoxy(0,2);
		TERM_display(0,IP_addr);
		
		retCode=TERM_getkey(KEEPWAITING);
		if(retCode!=KEY_ENTER)
		{
			return;	
		}
		else
		{
			TERM_clearscreen();
			goto IP_AGAIN;
		}
	}
}

void Server_COM_Setting_term()
{
	unsigned char Port[6];
	unsigned int retCode,iport;
	FILE *fp,*stream;
	int len,key;
	
	TERM_clearscreen();
	memset(Port,0,6);
	if((fp=fopen("/mnt/a/userapp/sys_txt/com","a+"))==NULL) 
	{
		fclose(fp);
		return;
	}
	fgets(Port,10,fp);
	fclose(fp);
	Port[strlen(Port)] = '\0';
	len = strlen(Port);
	if (len == 0)
	{
PORT_AGAIN:
		TERM_gotoxy(0,1);
		TERM_display(0,"请输入服务端口:");
		
		memcpy(Port, "     ", 5);
		Port[5] = '\0';
		if (ReadNumeric_(Port, 5, 0, 2) < 0) return;
		iport = atoi(Port);
		if( iport > 65535 )
		{
			TERM_gotoxy(0,4);
			TERM_display(0,"服务端口输入有误");
			
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);

			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					TERM_clearscreen();
					goto PORT_AGAIN;
				}
			}
			goto PORT_AGAIN;
		}

		memset(Port, '\0', 6);
		sprintf(Port, "%d", iport);

		Run("rm","/mnt/a/userapp/sys_txt/com",NULL,NULL);
	
		if((stream=fopen("/mnt/a/userapp/sys_txt/com","at+"))==NULL)
		{
			fclose(stream);
			return;
		}
		fputs(Port,stream);
		fsync(fileno(stream));
		rewind(stream);
		fclose(stream);

		TERM_gotoxy(0,4);
		TERM_display(0,"服务端口保存完毕");
		Beep_AC(1000);

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return;
			}
		}
		return;
	}
	else
	{
		TERM_clearscreen();

		TERM_gotoxy(0,1);
		TERM_display(0,"服务端端口:");
		TERM_gotoxy(0,2);
		TERM_display(0,Port);

		retCode=TERM_getkey(KEEPWAITING);
		if(retCode!=KEY_ENTER)
		{
			return;	
		}
		else
		{
			goto PORT_AGAIN;
		}
	}
}

void Server_IP_Setting_term()
{
	unsigned char buf[16],IP_addr[16],i;
	unsigned int retCode,iTmp[4];
	FILE *fp,*stream;
	int len,key;
	
	TERM_clearscreen();
	memset(IP_addr,0,16);
	if((fp=fopen("/mnt/a/userapp/sys_txt/ip","a+"))==NULL) 
	{
		fclose(fp);
		return;
	}
	fgets(IP_addr,20,fp);
	fclose(fp);
	IP_addr[strlen(IP_addr)] = '\0';
	len = strlen(IP_addr);
	if (len == 0)
	{
IP_AGAIN:
		TERM_gotoxy(0,1);
		TERM_display(0,"请输入IP地址:");

		memcpy(IP_addr, "   .   .   .   ", 15);
		IP_addr[15] = '\0';
		if (ReadNumeric_(IP_addr, 15, 0, 2) < 0) return;
	
		for(i = 0; i < 4; i++)
		{
			memcpy(buf, IP_addr + i * 4, 3);
			buf[3] = '\0';
			iTmp[i] = atoi(buf);
			if(atoi(buf) > 255)
			{
				TERM_gotoxy(0,4);
				TERM_display(0,"IP地址输入有误!");
				
				Beep_AC(200);
				usleep(500);
				Beep_AC(200);

				while(1)
				{
					key =TERM_getkey(KEEPWAITING);
					if (key > 0)
					{
						TERM_clearscreen();
						goto IP_AGAIN;
					}
				}
				goto IP_AGAIN;
			}
		}

		memset(IP_addr, '\0', 16);
		sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);

		Run("rm","/mnt/a/userapp/sys_txt/ip",NULL,NULL);
	
		if((stream=fopen("/mnt/a/userapp/sys_txt/ip","at+"))==NULL)
		{
			fclose(stream);
			return;
		}
		fputs(IP_addr,stream);
		fsync(fileno(stream));
		rewind(stream);
		fclose(stream);

		TERM_gotoxy(0,4);
		TERM_display(0,"IP地址保存完毕!");
		
		Beep_AC(1000);

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return;
			}
		}
		
		return;
	}
	else
	{
		TERM_clearscreen();
		TERM_gotoxy(0,1);
		TERM_display(0,"服务端IP地址:");
		TERM_gotoxy(0,2);
		TERM_display(0,IP_addr);
		
		retCode=TERM_getkey(KEEPWAITING);
		if(retCode!=KEY_ENTER)
		{
			return;	
		}
		else
		{
			TERM_clearscreen();
			goto IP_AGAIN;
		}
	}
}


int my_menu_system(unsigned char *funcstr,unsigned char**menustr,unsigned char n)
{      
	unsigned char number,i;
	unsigned char num,num_b;
	num=0;
	num_b=0;
	number=n;   
	
	TERM_clearscreen();
	for(i=0;i<number;i++)
	{
		TERM_gotoxy(1,i+1);
		if(i==0)
		{
		TERM_display(1,menustr[i]);
		}
		else
		TERM_display(0,menustr[i]); 
	}   		  	
	while(1)
	{    
		switch(TERM_getkey(KEEPWAITING))
		{
			case KEY_ESC:
				return  0;
			case KEY_ENTER:
				return num+1;
			case 0x31:
				return 1;
			case 0x32:
				return 2;
			case 0x33:
				return 3;
			case KEY_DOWN:
				if(num==number-1)   
					continue;		 	   
				else
				{    
					TERM_gotoxy(1,(num+1));
					TERM_display(0,menustr[num]); 
					num++;
					TERM_gotoxy(1,(num+1));
					TERM_display(1,menustr[num]);  	
				}			
					continue;
			case KEY_UP:
				if(num==0)
					continue;			
				else
				{   
					TERM_gotoxy(1,(num+1));
					TERM_display(0,menustr[num]); 
					num--;
					TERM_gotoxy(1,(num+1));
					TERM_display(1,menustr[num]); 
				}		
					continue;
			default:
				continue;
		}  //end switch
	} //end while
}

int sub_system_menu()
{
	unsigned char *sys_menu[]={"1:无线ESSID设置","2:WIFI服务器设置","3:GPRS&CDMA服务器设置"};	
	
	while(1)
	{   
		switch(my_menu_system(" ",sys_menu,3))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			default:
				continue;
		}
	}
}



int mmc_test_circle(int time_test)
{
	FILE *fd;
	int i=0;
	
	type_2num cc;
  type_2num dd;
  
			for(i=0;i<time_test;i++)
			  {
		    mmc_load();								
				mmc_load_ext2();
			  }
		if(time_test==50)	
			{
				  dd.byte_2_num=time_test;
          cc.byte_2_num=mount_time;
          fd=fopen("/mnt/a/userapp/record","ab+"); 
          usleep(200);
          fwrite(dd.buff, 2, 1,fd);
          usleep(200);
          fwrite(cc.buff, 2, 1,fd); 
          usleep(200);
          fwrite("\n", 1, 1,fd);
          fclose(fd);
				}  
	  TERM_gotoxy(0,4);
	TERM_display(0,"按任意键进行条码测试");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				return 0;
		  }  
  }			
	}


int mmc_load_ext2()
{
	FILE *fd;
	char ch_txt[300];
	char buf[30];
	char buf1[30];
	int len;
	int mount_succ=0;
  int i;
  
  if(CIRCLE_MODE==0)
  	{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载EXT格式TF卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");
   }
  for(i=0;i<10;i++)
  {
	system("mount -t ext2 /dev/mmcblk0p1 /mnt/mmc 2> /mnt/a/userapp/mmc_info.txt");

	memset(ch_txt,0,300);
	if((fd=fopen("/mnt/a/userapp/mmc_info.txt","r+"))==NULL)
	{
		fclose(fd);
		return -1;
	}
	fgets(ch_txt,100,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);
	len = strlen(ch_txt);

	if (len != 0)
	{
		continue;
	}
	else
		{
		mount_succ=1;	
		break;
	  }
 }
 
 if(mount_succ==1)
 	{
 	mount_time++;
 	memset(buf,0,30);
 	memset(buf1,0,30); 	
 	sprintf(buf,"TF卡成功%d次",mount_time);
 	 	sprintf(buf1,"TF卡失败%d次",mount_error);	
  TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,buf);
	TERM_gotoxy(0,3);
	TERM_display(0,buf1);
	system("/mnt/a/mc2006/driverscript/mmc_stop");
	return 0;
  }
  else
  	{
  mount_error++;
 	memset(buf,0,30);
 	memset(buf1,0,30); 	
 	sprintf(buf,"TF卡成功%d次",mount_time);
 	sprintf(buf1,"TF卡失败%d次",mount_error);		
  TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,buf);
	TERM_gotoxy(0,3);
	TERM_display(0,buf1);	
	system("/mnt/a/mc2006/driverscript/mmc_stop");
	return -1;  		
  		}
}

void mmc_load_fat()
{
	FILE *fd;
	char ch_txt[300];
	int len;

	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载FAT格式TF卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");

	system("mount -t vfat /dev/mmcblk0p1 /mnt/mmc 2> /mnt/a/userapp/mmc_info.txt");
	sleep(1);

	memset(ch_txt,0,300);
	if((fd=fopen("/mnt/a/userapp/mmc_info.txt","r+"))==NULL)
	{
		fclose(fd);
		return;
	}
	fgets(ch_txt,100,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);
	len = strlen(ch_txt);

	if (len != 0)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"加载失败,请重新加载");
		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return;
			}
		}
	}

	TERM_clearscreen();
	TERM_gotoxy(0,3);
	TERM_display(0,"加载FAT格式TF卡成功!");
	
	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{
			return;
		}
	}

	return;
}

int mmc_load_menu()
{
	unsigned char *sys_menu[]={"1:TF卡单次测试","2:TF卡10次测试","3:TF卡50次测试","4.按返回键返回"};	
	
	while(1)
	{
		switch(my_menu_scanbarcode(" ",sys_menu,4))
		{
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 10;
			case 3:
				return 50;
			default:
				continue;
		}
	}
}

int mmc_load()
{
	FILE *fd;
	char ch_txt[300];
	int len;

  if(CIRCLE_MODE==1)
  	{
 //	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在测试TF卡"); 		
  	}
  else
  	{	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载TF卡驱动");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");
}
	system("/mnt/a/mc2006/driverscript/mmc_start 2> /mnt/a/userapp/mmc_info.txt");

	memset(ch_txt,0,300);
	if((fd=fopen("/mnt/a/userapp/mmc_info.txt","r+"))==NULL)
	{
		fclose(fd);
		return -1;
	}
	fgets(ch_txt,100,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);
	len = strlen(ch_txt);

	if (len != 0)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,2);
		TERM_display(0,"加载驱动失败");
		sleep(1);

		return 1;
	}

if(CIRCLE_MODE==0)
	{
	TERM_clearscreen();
	TERM_gotoxy(0,3);
	TERM_display(0,"加载TF卡驱动成功!");
	usleep(10000);
  }
	return 0;
}

unsigned long Read_Icode2card_Block()
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
		TempStatus = l_wStatus=ISO15693_WriteBlock(0x22, TagInfo.UID, nBlock, 0x01, 0x04, SendData);
		if(TempStatus == MI_OK )
		{
			TempStatus = ISO15693_ReadBlock(0x22, TagInfo.UID, nBlock, 0x01, &RecLength, RecData);
			if(TempStatus == MI_OK )
			{
				printf("%c%c%c%c\n",RecData[1],RecData[2],RecData[3],RecData[4]);
				Beep_AC(500);
				memset(bcode,0,255);
				strcpy(bcode,"读写卡成功!\r\n");
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,bcode);
				return MI_OK;
			}
		}

		printf("Write Error: %04X\n", l_wStatus);
		Beep_AC(100);
		usleep(500);
		Beep_AC(100);
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"写卡错误");
		return -1;
	}

	return 1;
}

void Read_Icode2card_test()
{
	int time1=9;
	unsigned long TempStatus,card_read_ok;
	unsigned long rcode;
	int keych;
	char buffer[100];

re_readticard:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"请把卡放入读卡区");
	TERM_gotoxy(0,3);
	TERM_display(0,"按确认键读卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"按返回键返回上级");

	card_read_ok=0;
	time1=9;

	while (1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key == KEY_ENTER)
		{
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
					ISO15693_VCD_Off();
					return;
				}
				
				sprintf(buffer,"请刷卡.........%d",time1);	
				TERM_displayxy(1,2,NORMALDISPLAY,buffer);
				TempStatus = Read_Icode2card_Block();
				if(TempStatus == MI_OK )
				{
					card_read_ok = 1;
					Beep_AC(500);
					ISO15693_VCD_Off();
					goto re_readticard;
				}
				else if (TempStatus == -1)
				{
					card_read_ok = 1;
					Beep_AC(500);
					ISO15693_VCD_Off();
					sleep(1);
					goto re_readticard;
				}
				else if (TempStatus == 1)
				{
					if(card_read_ok==0)time1--;
					Beep_AC(100);
				}

				keych = TERM_getkey(10);
				switch(keych)
				{
					case KEY_ESC:
						ISO15693_VCD_Off();
						goto re_readticard;
				}
			}
		}
		else if (key == KEY_ESC)
		{
			ISO15693_VCD_Off();
			return;
		}
	}
	
	return;
}





int sub_scanbarcod_menu()
{
	unsigned char *sys_menu[]={"1:单次扫描","2:100次循环扫描","3:200次循环扫描"};	
	
	while(1)
	{   
		switch(my_menu_scanbarcode(" ",sys_menu,3))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 100;
			case 3:
				return 200;	
			default:
				continue;
		}
	}
}

void Scan_barcode_round(int k)
{
		FILE *fd;
	char tmpstr[255],ch_info[50];
	char ch_wr_txt[255],time_str[20];
	char buf[30];
  int i;
	int l_n=0;
	int ln_error=0;

  type_2num cc;
  type_2num dd;
  
re_scanbarcode:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"开始扫描条码...");
  usleep(100000);

	for(i=0;i<k;i++)
	{
		memset(tstr,0,255);
		memset(tmpstr,0,255);
		if (BARCODE_trigger(tstr))
		{
			ln_error++;
				TERM_clearscreen();
				memset(buf,0,30);
				memset(ch_info,0,50);
				sprintf(ch_info,"成功: %d 条",l_n);
				sprintf(buf,"失败: %d条",ln_error);
				ch_info[strlen(ch_info)] = '\0';
     
				TERM_gotoxy(0,1);
				TERM_display(0,ch_info);
				TERM_gotoxy(0,2);
				TERM_display(0,buf);
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描失败!");
				Beep_AC(1000);
		}
		else
		{
			sprintf(tmpstr, "%s", tstr);
			memset(bcode,0,20);
			memcpy(bcode,tmpstr + 3,strlen(tmpstr) - 3);
			bcode[strlen(bcode)] = '\0';
			if (strcmp(bcode,"NR")==0)
			{
				ln_error++;
				TERM_clearscreen();
				memset(buf,0,30);
				memset(ch_info,0,50);
				sprintf(ch_info,"成功: %d 条",l_n);
				sprintf(buf,"失败: %d条",ln_error);
				ch_info[strlen(ch_info)] = '\0';
     
				TERM_gotoxy(0,1);
				TERM_display(0,ch_info);
				TERM_gotoxy(0,2);
				TERM_display(0,buf);
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描失败!");
				Beep_AC(1000);
			}
			else
			{
				l_n++;
			
				TERM_clearscreen();
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描成功!");
				TERM_gotoxy(0,4);
				TERM_display(0,bcode);
			
			  memset(buf,0,30);
				memset(ch_info,0,50);
				sprintf(ch_info,"成功: %d 条",l_n);
				sprintf(buf,"失败: %d条",ln_error);
				ch_info[strlen(ch_info)] = '\0';
     
				TERM_gotoxy(0,1);
				TERM_display(0,ch_info);
				TERM_gotoxy(0,2);
				TERM_display(0,buf);
				Beep_AC(200);
			}
		}

		key =TERM_getkey(3);
		if (key == KEY_ESC)
		{
			return;
		}
	}
	if(k==200)
		{
	cc.byte_2_num=l_n;		
	dd.byte_2_num=k;
  fd=fopen("/mnt/a/userapp/record","ab+"); 
  usleep(200);
  fwrite(dd.buff, 2, 1,fd);
  usleep(200);
  fwrite(cc.buff, 2, 1,fd); 
  usleep(200);
  fwrite("\n", 1, 1,fd);
  fclose(fd);
  
			}
	      TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"扫描完成");
				TERM_gotoxy(0,2);
				TERM_display(0,ch_info);
				TERM_gotoxy(0,3);
				TERM_display(0,buf);
				TERM_gotoxy(0,4);
				TERM_display(0,"按任意键返回");				
	while (1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key >0)
		{
			return ;		
		}		
	}
	return;
}

void Scan_barcode_1()
{
	int lt;
	char tmpstr[255];
	char ch_wr_txt[255],time_str[20];

re_scan_barcode:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按扫描键扫描条码");

	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key == KEY_SCAN)
		{
			memset(tstr,0,255);
			memset(tmpstr,0,255);
			if (BARCODE_trigger(tstr))
			{
				TERM_clearscreen();
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描失败!");
				TERM_gotoxy(0,4);
				TERM_display(0,"请重新扫描...");
				Beep_AC(1000);
				goto re_scan_barcode;
			}
			else
			{
				sprintf(tmpstr, "%s", tstr);
				memset(bcode,0,20);
				memcpy(bcode,tmpstr + 3,strlen(tmpstr) - 3);
				bcode[strlen(bcode)] = '\0';
				if (strcmp(bcode,"NR")==0)
				{
					TERM_clearscreen();
					TERM_gotoxy(0,3);
					TERM_display(0,"条码扫描失败!");
					TERM_gotoxy(0,4);
					TERM_display(0,"请重新扫描...");
					Beep_AC(1000);
					goto re_scan_barcode;
				}
				
				TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"条码扫描成功!");
				TERM_gotoxy(0,2);
				TERM_display(0,bcode);
				Beep_AC(1000);
				break;

			}
		}
	}
	while(1)
	{
	TERM_gotoxy(0,4);
	TERM_display(0,"按返回键返回上级");
	key =TERM_getkey(KEEPWAITING);
		if (key == KEY_ESC)
		{
      BARCODE_poweroff();			
			return;
		}
	return;
	}
}


void Scan_barcode_1_W()
{
	int lt;
	char tmpstr[255];
	char ch_wr_txt[255],time_str[20];

re_scan_barcode:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按扫描键扫描条码");
	
	TERM_gotoxy(0,4);
	TERM_display(0,"按返回退出");
	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key == KEY_SCAN)
		{
			memset(tstr,0,255);
			memset(tmpstr,0,255);
			if (BARCODE_trigger(tstr))
			{
				TERM_clearscreen();
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描失败!");
				TERM_gotoxy(0,4);
				TERM_display(0,"请重新扫描...");
				Beep_AC(1000);
				goto re_scan_barcode;
			}
			else
			{
				sprintf(tmpstr, "%s", tstr);
				memset(bcode,0,20);
				memcpy(bcode,tmpstr + 3,strlen(tmpstr) - 3);
				bcode[strlen(bcode)] = '\0';
				if (strcmp(bcode,"NR")==0)
				{
					TERM_clearscreen();
					TERM_gotoxy(0,3);
					TERM_display(0,"条码扫描失败!");
					TERM_gotoxy(0,4);
					TERM_display(0,"请重新扫描...");
					Beep_AC(1000);
					goto re_scan_barcode;
				}				
				TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"条码扫描成功!");
				TERM_gotoxy(0,2);
				TERM_display(0,bcode);
				Beep_AC(1000);
				TERM_gotoxy(0,4);
				TERM_display(0,"按返回键返回上级");

				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_ESC)
				{		
					goto re_scan_barcode;
				}
			}
		}
		else if(key == KEY_ESC)
		{
			BARCODE_poweroff();	
			return ;
		}
	}
	return;
}

void itoa(char *str,int a)//数字转字符串
{
	sprintf(str,"%d",a);
}


void Scan_barcode_loop_W()
{
	int lt;
	char tmpstr[255];
	char ch_wr_txt[255],time_str[20];
	char str[5];
	int count = 0,count1 = 0;

re_scan_barcode:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按扫描键开始");
	
	TERM_gotoxy(0,4);
	TERM_display(0,"按返回退出");
	key =TERM_getkey(KEEPWAITING);
	if (key == KEY_SCAN)
	{
		while(1)
		{
			memset(tstr,0,255);
			memset(tmpstr,0,255);
			if (BARCODE_trigger(tstr))
			{
				TERM_clearscreen();
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描失败!");
				Beep_AC(1000);
				count++;
			}
			else
			{
				sprintf(tmpstr, "%s", tstr);
				memset(bcode,0,20);
				memcpy(bcode,tmpstr + 3,strlen(tmpstr) - 3);
				bcode[strlen(bcode)] = '\0';
				if (strcmp(bcode,"NR")==0)
				{
					TERM_clearscreen();
					TERM_gotoxy(0,3);
					TERM_display(0,"条码扫描失败!");
					Beep_AC(1000);
					count++;
				}
				else
				{
					TERM_clearscreen();
					TERM_gotoxy(0,1);
					TERM_display(0,"条码扫描成功!");
					TERM_gotoxy(0,2);
					TERM_display(0,bcode);
					Beep_AC(1000);
					count1++;
				}
				if((count + count1) == 10)
				{
					TERM_clearscreen();
					itoa(str,count1);
					TERM_gotoxy(0,1);
					TERM_display(0,"扫描成功次数!");
					TERM_gotoxy(0,2);
					TERM_display(0,str);
					itoa(str,count);
					TERM_gotoxy(0,3);
					TERM_display(0,"扫描失败次数!");
					TERM_gotoxy(0,4);
					TERM_display(0,str);
					break;
				}
			}
			sleep(1);
		}
		key =TERM_getkey(KEEPWAITING);
		if(key > 0)
		{
			goto re_scan_barcode;
		}
	}
	else if(key == KEY_ESC)
	{
		BARCODE_poweroff();	
		return ;
	}
	return;
}


int Barcode_poweron()
{
	int ret;
	int time_re=0;
	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"条码正在上电");
	TERM_gotoxy(0,2);
	TERM_display(0,"请等待...");
	

Poweron_again:
	if ((ret=BARCODE_poweron())!=BC_ERROR_NONE)
	{
		time_re++;
		if(time_re<3)
		{	
		BARCODE_poweroff();
		goto Poweron_again;
	  }
	  BARCODE_poweroff();
		TERM_clearscreen();
		TERM_gotoxy(0,1);
		TERM_display(0,"条码上电失败!");
		TERM_gotoxy(0,2);
		TERM_display(0,"请联系质检员");
		TERM_gotoxy(0,3);
		TERM_display(0,"程序测试终止");
		while(1);
	}

	TERM_gotoxy(0,4);
	TERM_display(0,"条码上电成功!");
	Beep_AC(200);
//	sleep(1);

	return 0;
}

void Scan_barcode_test()
{
	int ld,lt;

	ld = Barcode_poweron();
	if (ld == 0)
	{
re_sub_scanbarcod_menu:
		lt = sub_scanbarcod_menu();
		if (lt == 0)
		{
			return;
		}
		
			Scan_barcode_round(lt);
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					goto re_sub_scanbarcod_menu;
				}
			}

	}

	return;
}



void Eth0_kzw_test_open()
{
	unsigned char buf[16],IP_addr[16],i;
	unsigned int iTmp[4];
	unsigned char ch_txt[50];
	char ch_buf[300];
	FILE *fd;
	
	
IP_AGAIN:
	TERM_clearscreen();
	memset(IP_addr,0,16);
	TERM_gotoxy(0,1);
	TERM_display(0,"请输入IP地址:");
	memcpy(IP_addr, "   .   .   .   ", 15);
	IP_addr[15] = '\0';
	if (ReadNumeric_(IP_addr, 15, 0, 2) < 0) return;
	
	for(i = 0; i < 4; i++)
	{
		memcpy(buf, IP_addr + i * 4, 3);
		buf[3] = '\0';
		iTmp[i] = atoi(buf);
		if(atoi(buf) > 255)
		{
			TERM_gotoxy(0,4);
			TERM_display(0,"IP地址输入有误!");
			
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);

			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					TERM_clearscreen();
					goto IP_AGAIN;
				}
			}
			goto IP_AGAIN;
		}
	}

	memset(IP_addr, '\0', 16);
	sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);
	IP_addr[strlen(IP_addr)] = '\0';
	
	memset(ch_txt,0,50);
	strcpy(ch_txt,"/mnt/a/mc2006/driverscript/usbh2eth_up ");
	strcat(ch_txt,IP_addr);
	ch_txt[strlen(ch_txt)] = '\0';

	system("/mnt/a/mc2006/driverscript/usbh2eth_down");
	sleep(1);
	system(ch_txt);

	memset(ch_buf,0,300);
	if((fd=fopen("/proc/net/dev","r"))==NULL)
	{
		printf(0,"打不开文件/proc/net/dev");
		return;
	}
	while(!feof(fd))
	{
		fgets(ch_buf,250,fd);
		printf(ch_buf);
		printf("\r\naaaaaaa\r\n");
		if (strstr(ch_buf,"eth0:")!=NULL)
		{
			fclose(fd);
			TERM_gotoxy(0,4);
			TERM_display(0,"网络打开成功!");
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					return;
				}
			}
			return;
		}
	}
	fclose(fd);

	TERM_gotoxy(0,4);
	TERM_display(0,"网络打开失败!");

	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{
			return;
		}
	}
	return;
}

int my_menu_menulabel_2(unsigned char *funcstr,unsigned char**menustr,unsigned char n)
{      
	unsigned char number,i;
	unsigned char num,num_b;
	num=0;
	num_b=0;
	number=n;   
	
	TERM_clearscreen();
	if(number>4)
	{      
		//显示4个内容
		TERM_gotoxy(1,1);
		TERM_display(1,menustr[0]);
		TERM_gotoxy(1,2);
		TERM_display(0,menustr[1]);  
		TERM_gotoxy(1,3);
		TERM_display(0,menustr[2]);  
		TERM_gotoxy(1,4);
		TERM_display(0,menustr[3]);  
	 	    
		while(1)
		{
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case KEY_ENTER:
					return  num+1;
				case KEY_DOWN:
					if(num_b==3)
					{   
						if(num==(number-1))   
						{     
							num=number-1;
							continue;
						}    
						else if(num<(number-1))
						{       
							num++;
							num_b=3;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(0,menustr[num-3]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num-2]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num-1]);
							TERM_gotoxy(1,4);
							TERM_display(1,menustr[num]);       
						}
					} 
					if(num_b<3)
					{     
						num_b++;
						num++;
						TERM_gotoxy(1,num_b+1);
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b));
						TERM_display(0,menustr[num-1]); 
					}   
						continue;
				case KEY_UP:
					if(num_b==0)
					{
						if(num==0)
						{	  
							num=0;
							num_b=0;
							continue;
						}   
						else if(num>0)
						{   
							num--;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(1,menustr[num]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num+1]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num+2]);
							TERM_gotoxy(1,4);
							TERM_display(0,menustr[num+3]);	
						}    
					}        
					else if((num_b>0)&&(num_b<=3))        
					{  
						num--;
						num_b--;        
						TERM_gotoxy(1,(num_b+1));
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b+2));
						TERM_display(0,menustr[num+1]);			     
					}
						continue;
				default:
					continue;
			}//end switch
		}//end while
	}   //end if	
	else 
	{      
		for(i=0;i<number;i++)
		{
			TERM_gotoxy(1,i+1);
			if(i==0)
			{
			TERM_display(1,menustr[i]);
			}
			else
			TERM_display(0,menustr[i]); 
		}   		  	
		while(1)
		{    
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case KEY_ENTER:
					return num+1;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case KEY_DOWN:
					if(num==number-1)   
						continue;		 	   
					else
					{    
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num++;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]);  	
					}			
						continue;
				case KEY_UP:
					if(num==0)
						continue;			
					else
					{   
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num--;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]); 
					}		
						continue;
				default:
					continue;
			}  //end switch
		} //end while
	} //end if
}

int my_menu_menulabel_1(unsigned char *funcstr,unsigned char**menustr,unsigned char n)
{      
	unsigned char number,i;
	unsigned char num,num_b;
	num=0;
	num_b=0;
	number=n;   
	
	TERM_clearscreen();
	if(number>4)
	{      
		//显示4个内容
		TERM_gotoxy(1,1);
		TERM_display(1,menustr[0]);
		TERM_gotoxy(1,2);
		TERM_display(0,menustr[1]);  
		TERM_gotoxy(1,3);
		TERM_display(0,menustr[2]);  
		TERM_gotoxy(1,4);
		TERM_display(0,menustr[3]);  
	 	    
		while(1)
		{
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case 0x35:
					return 5;
				case KEY_ENTER:
					return  num+1;
				case KEY_DOWN:
					if(num_b==3)
					{   
						if(num==(number-1))   
						{     
							num=number-1;
							continue;
						}    
						else if(num<(number-1))
						{       
							num++;
							num_b=3;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(0,menustr[num-3]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num-2]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num-1]);
							TERM_gotoxy(1,4);
							TERM_display(1,menustr[num]);       
						}
					} 
					if(num_b<3)
					{     
						num_b++;
						num++;
						TERM_gotoxy(1,num_b+1);
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b));
						TERM_display(0,menustr[num-1]); 
					}   
						continue;
				case KEY_UP:
					if(num_b==0)
					{
						if(num==0)
						{	  
							num=0;
							num_b=0;
							continue;
						}   
						else if(num>0)
						{   
							num--;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(1,menustr[num]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num+1]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num+2]);
							TERM_gotoxy(1,4);
							TERM_display(0,menustr[num+3]);	
						}    
					}        
					else if((num_b>0)&&(num_b<=3))        
					{  
						num--;
						num_b--;        
						TERM_gotoxy(1,(num_b+1));
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b+2));
						TERM_display(0,menustr[num+1]);			     
					}
						continue;
				default:
					continue;
			}//end switch
		}//end while
	}   //end if	
	else 
	{      
		for(i=0;i<number;i++)
		{
			TERM_gotoxy(1,i+1);
			if(i==0)
			{
			TERM_display(1,menustr[i]);
			}
			else
			TERM_display(0,menustr[i]); 
		}   		  	
		while(1)
		{    
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case KEY_ENTER:
					return num+1;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case 0x35:
					return 5;
				case KEY_DOWN:
					if(num==number-1)   
						continue;		 	   
					else
					{    
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num++;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]);  	
					}			
						continue;
				case KEY_UP:
					if(num==0)
						continue;			
					else
					{   
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num--;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]); 
					}		
						continue;
				default:
					continue;
			}  //end switch
		} //end while
	} //end if
}

int my_menu_sub(unsigned char *funcstr,unsigned char**menustr,unsigned char n)
{      
	unsigned char number,i;
	unsigned char num,num_b;
	num=0;
	num_b=0;
	number=n;   
	
	TERM_clearscreen();
	if(number>4)
	{      
		//显示4个内容
		TERM_gotoxy(1,1);
		TERM_display(1,menustr[0]);
		TERM_gotoxy(1,2);
		TERM_display(0,menustr[1]);  
		TERM_gotoxy(1,3);
		TERM_display(0,menustr[2]);  
		TERM_gotoxy(1,4);
		TERM_display(0,menustr[3]);  
	 	    
		while(1)
		{
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case 0x35:
					return 5;
				case KEY_ENTER:
					return  num+1;
				case KEY_DOWN:
					if(num_b==3)
					{   
						if(num==(number-1))   
						{     
							num=number-1;
							continue;
						}    
						else if(num<(number-1))
						{       
							num++;
							num_b=3;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(0,menustr[num-3]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num-2]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num-1]);
							TERM_gotoxy(1,4);
							TERM_display(1,menustr[num]);       
						}
					} 
					if(num_b<3)
					{     
						num_b++;
						num++;
						TERM_gotoxy(1,num_b+1);
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b));
						TERM_display(0,menustr[num-1]); 
					}   
						continue;
				case KEY_UP:
					if(num_b==0)
					{
						if(num==0)
						{	  
							num=0;
							num_b=0;
							continue;
						}   
						else if(num>0)
						{   
							num--;
							TERM_clearscreen();     //清屏
							TERM_gotoxy(1,1);
							TERM_display(1,menustr[num]);
							TERM_gotoxy(1,2);
							TERM_display(0,menustr[num+1]);
							TERM_gotoxy(1,3);
							TERM_display(0,menustr[num+2]);
							TERM_gotoxy(1,4);
							TERM_display(0,menustr[num+3]);	
						}    
					}        
					else if((num_b>0)&&(num_b<=3))        
					{  
						num--;
						num_b--;        
						TERM_gotoxy(1,(num_b+1));
						TERM_display(1,menustr[num]);
						TERM_gotoxy(1,(num_b+2));
						TERM_display(0,menustr[num+1]);			     
					}
						continue;
				default:
					continue;
			}//end switch
		}//end while
	}   //end if	
	else 
	{      
		for(i=0;i<number;i++)
		{
			TERM_gotoxy(1,i+1);
			if(i==0)
			{
			TERM_display(1,menustr[i]);
			}
			else
			TERM_display(0,menustr[i]); 
		}   		  	
		while(1)
		{    
			switch(TERM_getkey(KEEPWAITING))
			{
				case KEY_ESC:
					return  0;
				case KEY_ENTER:
					return num+1;
				case 0x31:
					return 1;
				case 0x32:
					return 2;
				case 0x33:
					return 3;
				case 0x34:
					return 4;
				case 0x35:
					return 5;
				case KEY_DOWN:
					if(num==number-1)   
						continue;		 	   
					else
					{    
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num++;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]);  	
					}			
						continue;
				case KEY_UP:
					if(num==0)
						continue;			
					else
					{   
						TERM_gotoxy(1,(num+1));
						TERM_display(0,menustr[num]); 
						num--;
						TERM_gotoxy(1,(num+1));
						TERM_display(1,menustr[num]); 
					}		
						continue;
				default:
					continue;
			}  //end switch
		} //end while
	} //end if
}

int sub_main_menulabel_6()
{
	unsigned char *sys_menu[]={"1:上电","2:拨号","3:联网","4:传输数据","5:断网并下电"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,5))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			default:
				continue;
		}
	}
}

int sub_main_menulabel_5()
{
	unsigned char *sys_menu[]={"1:上电","2:拨号","3:联网","4:传输数据","5:断网并下电"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,5))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			default:
				continue;
		}
	}
}

int sub_main_menulabel_4()
{
	unsigned char *sys_menu[]={"1:打开以太网","2:关闭以太网","3:打开USB","4:关闭USB"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_2(" ",sys_menu,4))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			default:
				continue;
		}
	}
}

int sub_main_menulabel_3()
{
	unsigned char *sys_menu[]={"1:基本参数设置","2:红外测试","3:WIFI测试","4:GPRS测试","5:CDMA测试"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,5))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			default:
				continue;
		}
	}
}

int sub_main_menulabel_2()
{
	unsigned char *sys_menu[]={"1:到位开关测试","2:TI卡测试","3:CPU卡测试","4:SAM卡测试","5:TF卡测试","6:Icode2测试"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,6))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			case 6:
				return 6;
			default:
				continue;
		}
	}
}

int sub_main_menulabel_1()
{
	unsigned char *sys_menu[]={"1:按键测试","2:时钟测试","3:背光测试","4:电量测试","5:蜂鸣器测试"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,5))
		{		       
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			default:
				continue;
		}
	}
}

int sub_main()
{
	unsigned char *sys_menu[]={"1:基本功能测试","2:卡片功能测试","3:条码功能测试","4:无线功能测试","5:扩展坞测试"};	
	
	while(1)
	{   
		switch(my_menu_sub(" ",sys_menu,5))
		{	
			case 0:
				return 0;	       
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			case 5:
				return 5;
			default:
				continue;
		}
	}
}

void sub_main_welcome()
{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"  ESCAN2009测试");
	TERM_gotoxy(0,3);
	TERM_display(0,"   20161207");

	if( mkdir("/mnt/a/userapp/sys_txt",00700)== -1)
 
//	set_sleep_level_LCDSLEEP();

	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{
			return;
		}
	}

	return;
}

int main(int argc, char *argv[])
{
	int key;
  int i;

	TERM_console_open();
	TERM_clearscreen();


	sub_main_welcome();
  while(1)
  {
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次抽检测试");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.低温老化测试"); 	
	TERM_gotoxy(0,3);
	TERM_display(0,"3.高温老化测试"); 
	TERM_gotoxy(0,4);
	TERM_display(0,"4.功能全检测试"); 	
		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key ==0x31)
		{
			one_time_test();
			break;
		}
		else if(key==0x32)
			{
				zero_temp_test();//低温老化测试
				break;
				}
		else if(key==0x33)
			{
			high_temp_test();//高温老化测试
			break;
				}
		else if(key==0x34)
			{
				circle_test();//功能全检
				break;
				}				
		else if(key==0x35)
			{
				Check_record();//查看结果
				break;
				}	
		else if(key==0x36)
			{		
				admin_check();	
				break;
			}
	}
	 	
  	}

}


int one_time_test()
{
	
		TERM_clearscreen();
		TERM_displayxy(0, 1, NORMALDISPLAY, "是否确认进入单次");
	  TERM_displayxy(0, 2, NORMALDISPLAY, "抽检测试");	
		if(get_confirm_stat("确认","退出")==1)
			 {
		     TERM_clearscreen();
		     TERM_displayxy(3, 1, NORMALDISPLAY, "进入单次抽检测试");		         
	     }
	       else
	       	return 0; 
	       	
	mount_time=0;
	mount_error=0;
				
	Presskey_test(1);
	DateTime_Setting();
	Backlight_test();
	Beep_test();
	test_switch_cup();
	cpucard_test_once();
	samcard_test_once();	
	if(mmc_load()==0)
		{
			if(mmc_load_ext2()==0)
				{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡功能正常");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "准备进入条码测试");
		      Barcode_poweron();
		      Scan_barcode_1();
					}
					else
						{
							TERM_clearscreen();
		          TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡功能不正常");
		          TERM_displayxy(0, 2, NORMALDISPLAY, "请联系质检员");
              TERM_displayxy(0, 3, NORMALDISPLAY, "测试程序终止");		   	
		          while(1);  
							}
			}
		else
			{
				TERM_clearscreen();
		    TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡驱动不成功");
		    TERM_displayxy(0, 2, NORMALDISPLAY, "请联系质检员");
        TERM_displayxy(0, 3, NORMALDISPLAY, "测试程序终止");		   	
		    while(1);  
			}		
	return 0;
	}
	

int zero_temp_test()//低温检测
{
	FILE *fd;
	char ch_txt[20];
	
  				mount_time=0;
				mount_error=0;
				
	fd=fopen("/mnt/a/userapp/zero_record","ab+");
	fgets(ch_txt,20,fd);
	fclose(fd);
	if(memcmp(ch_txt,"zero_test ok",12)==0)
		{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "已进行过低温测试");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "是否继续进入测试");	
		      if(get_confirm_stat("进入","返回")==1)
			       {
		         TERM_clearscreen();
		         TERM_displayxy(3, 1, NORMALDISPLAY, "进入低温老化测试");
		         	system("rm /mnt/a/userapp/zero_record"); 
	           }
	       else
	       	return 0;    
	   }
	 else
	 	{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "是否确认进入低温");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "老化测试");	
		      if(get_confirm_stat("确认","退出")==1)
			       {
		         TERM_clearscreen();
		         TERM_displayxy(3, 1, NORMALDISPLAY, "进入低温老化测试");
		         
	           }
	       else
	       	return 0; 	 		
	 		}  
	Presskey_test(1);
	DateTime_Setting();
	Backlight_test();
	Beep_test();
	test_switch_cup();
	cpucard_test_once();
	samcard_test_once();	
re_mmc:	
	if(mmc_load()==0)
		{
			if(mmc_load_ext2()==0)
				{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡功能正常");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "准备进入条码测试");
		      Barcode_poweron();
		      Scan_barcode_1();
					}
					else
						{
					TERM_gotoxy(0,1);
					TERM_display(0,"TF卡测试失败");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
		   				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_mmc;
		             }
	             } 
							}
			}
		else
			{
				TERM_clearscreen();
		    TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡驱动不成功");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
		   				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_mmc;
		             }
	             }
			}
   fd=fopen("/mnt/a/userapp/zero_record","ab+");
   usleep(2000);
	 fwrite("zero_test ok", 12, 1,fd);
   usleep(2000);
   fclose(fd);
	return 0;
	}


int high_temp_test()//高温检测
{
	FILE *fd;
	char ch_txt[20];
	int key=0;
	
				mount_time=0;
				mount_error=0;
	
	fd=fopen("/mnt/a/userapp/zero_record","ab+");
	fgets(ch_txt,20,fd);
	fclose(fd);
	
	if(memcmp(ch_txt,"zero_test ok",12)!=0)
		{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "没有进行低温测试");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "请先低温功能测试");	
		      while(1)
	          {
		         key =TERM_getkey(KEEPWAITING);
		          if (key >0)
		           {
		          	break;
		          } 		
			    }
			    return 0;
	   }
	 
	memset(ch_txt,0,20); 
	fd=fopen("/mnt/a/userapp/hightemp_record","ab+");
	fgets(ch_txt,20,fd);
	fclose(fd);	
		if(memcmp(ch_txt,"hightemp_test ok",16)==0)
		{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "已进行过高温测试");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "是否继续进入测试");	
		      if(get_confirm_stat("进入","返回")==1)
			       {
		         TERM_clearscreen();
		         TERM_displayxy(3, 1, NORMALDISPLAY, "进入高温老化测试");
		         	system("rm /mnt/a/userapp/zero_record"); 
	           }
	       else
	       	return 0;    
	   }
	 else
	 	{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "是否确认进入高温");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "老化测试");	
		      if(get_confirm_stat("确认","退出")==1)
			       {
		         TERM_clearscreen();
		         TERM_displayxy(3, 1, NORMALDISPLAY, "进入高温老化测试");
		         
	           }
	       else
	       	return 0; 	 		
	 		}  
	HIGH_TEST_MODE=1;	  
	 
	read_rtc_time();	
	Presskey_test(1);
	Backlight_test();
	Beep_test();
	test_switch_cup();
	cpucard_test_once();
	samcard_test_once();	
re_mmc:	
	if(mmc_load()==0)
		{
			if(mmc_load_ext2()==0)
				{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡功能正常");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "准备进入条码测试");
		      Barcode_poweron();
		      Scan_barcode_1();
					}
					else
						{
					TERM_gotoxy(0,1);
					TERM_display(0,"TF卡测试失败");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
		   				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_mmc;
		             }
	             } 
							}
			}
		else
			{
				TERM_clearscreen();
		    TERM_displayxy(0, 1, NORMALDISPLAY, "TF卡驱动不成功");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
		   				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_mmc;
		             }
	             }
			}
			
   fd=fopen("/mnt/a/userapp/hightemp_record","ab+");
   usleep(2000);
	 fwrite("hightemp_test ok", 16, 1,fd);
   usleep(2000);
   fclose(fd);				
	return 0;
	}	
	
	
int circle_test()
{
 FILE *fd;
 int key=0;
 char ch_txt[20];
 
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "是否确认进入功能");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "全检测试");	
		      if(get_confirm_stat("确认","退出")==1)
			       {
		         TERM_clearscreen();
		         TERM_displayxy(3, 1, NORMALDISPLAY, "进入功能测试");
		         
	           }
	       else
	       	return 0; 
 
 CIRCLE_MODE=1;
 mount_time=0;
 
	fd=fopen("/mnt/a/userapp/hightemp_record","ab+");
	fgets(ch_txt,20,fd);
	fclose(fd);
	
	if(memcmp(ch_txt,"hightemp_test ok",16)!=0)
		{
					TERM_clearscreen();
		      TERM_displayxy(0, 1, NORMALDISPLAY, "没有进行高温测试");
		      TERM_displayxy(0, 2, NORMALDISPLAY, "请先高温功能测试");	
		      while(1)
	          {
		         key =TERM_getkey(KEEPWAITING);
		          if (key >0)
		           {
		          	break;
		          } 		
			    }
			    return 0;
	   }	
	system("rm /mnt/a/userapp/record"); 
	usleep(2000);
	read_rtc_time();
  Presskey_test(2);
  test_switch_cup();
  Read_cpucard_test(50) ;
  Read_samcard_test(50);
  mmc_test_circle(50);
  Barcode_poweron();
 	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"将条码放至10CM处");
	TERM_gotoxy(0,2);
	TERM_display(0,"按任意键开始");  	
		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{			
			break;
		} 
	}
	
 	 Scan_barcode_round(200);
 	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"将条码放至15CM处");
	TERM_gotoxy(0,2);
	TERM_display(0,"按任意键开始");  	
		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{			
			break;
		} 
	}	
  Scan_barcode_round(200);  
  BARCODE_poweroff();
  CIRCLE_MODE=0;
  return 0;
	}
	
int Check_record()
{
	FILE *fd;
	int i=0;
	int j;
	char Check_type[20];
	char ch_txt[10];
	char time_test_buf[6],time_ok_buf[6];
	char show_buf1[30],show_buf2[30];
	int key=0;
	int time_test=0;
	int time_ok=0;
	int flag=0;
	
  type_2num cc;
  type_2num dd;
  	
	memset(ch_txt,0,10);
   fd=fopen("/mnt/a/userapp/record","ab+");

	while(fgets(ch_txt,10,fd)!=NULL)
   {
 
   	memset(show_buf1,0,30);
   	memset(show_buf2,0,30);
   	memset(Check_type,0,20);
   	memset(time_test_buf,0,6);
   	memset(time_ok_buf,0,6);
   	
   	i++;
   	if(i==1)
   		memcpy(Check_type,"CPU卡测试",12);
   	else if(i==2)
   		memcpy(Check_type,"SAM卡测试",12);
   	else if(i==3)	
   	  memcpy(Check_type,"TF卡测试",11);   		
   	else if(i==4)	
   	  memcpy(Check_type,"10CM条码测试",14);
   	else if(i==5)	
   	  memcpy(Check_type,"15CM条码测试",14);   	  
   	
   	for(j=0;j<5;j++)
   	{
   		printf("%02x",ch_txt[j]);
   	}
   	printf("\n");
   	
   	memcpy(dd.buff,ch_txt,2); //测试总次数
   	memcpy(cc.buff,ch_txt+2,2); //成功总次数 
   	
   	time_test=dd.byte_2_num;
   	time_ok=cc.byte_2_num;
   	
   	sprintf(time_test_buf,"%d",time_test);
    sprintf(time_ok_buf,"%d",time_ok);
    
   	memcpy(show_buf1,"测试次数",12);
   	memcpy(show_buf1+12,time_test_buf,6);
   	memcpy(show_buf2,"成功次数",12);
   	memcpy(show_buf2+12,time_ok_buf,6);   	
 	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,Check_type);   
	TERM_gotoxy(0,2);
	TERM_display(0,show_buf1); 		
	TERM_gotoxy(0,3);
	TERM_display(0,show_buf2);	
 		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key ==KEY_DOWN)
		{			
			break;
		} 
		else if(key== KEY_ESC)
			{
				fclose(fd);
			 return 0;
	  	}
	}	 	 		 		
	}
	fclose(fd);
	if(i==0)
		{
 	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"没有相关测试记录");	
	TERM_gotoxy(0,2);
	TERM_display(0,"按返回键返回");		
}
 		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
   if(key== KEY_ESC)
			{
			 return 0;
	  	}
	}	
	
}
