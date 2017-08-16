#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h> 
#include <netinet/in.h>
#include <time.h>

#include "libdriver2006.h"


#include "wifi.c"
#include "TI_card_test.c"
#include "M1card_test.c"
#include "basic_test.c"
#include "cpucard_test.c"
#include "gprs_test.c"
#include "udisk_test.c"


#define FALSE 0
#define TRUE 1


#define MTD_DEV "/dev/mtd0"
int mount_time=0;
int mount_error=0;
char upda_buf[] = {0x00,0x0A,0x80,0x04,0x00,0x00,0x06,0x20,0x00,0xFF,0xFF,0xFD,0x4E};
char upda_buf1[] = {0x00,0x0A,0x80,0x04,0x00,0x00,0x06,0x02,0x00,0x4E,0x24,0xFE,0xF8};
#define ARRAY_SIZE(a) (sizeof(a))/(sizeof(a[0]))
#define BARCODE_DEVICE "/dev/s3c2410_serial1"
#define BAUDRATE1 B9600
int fd232_1;



int Barcode_poweron_version()
{
	int ret;
	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"条码重新上电");
	TERM_gotoxy(0,2);
	TERM_display(0,"请等待...");
	
	BARCODE_poweroff();

	if ((ret=BARCODE_poweron())!=BC_ERROR_NONE)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,3);
		TERM_display(0,"条码上电失败!");
		TERM_gotoxy(0,4);
		TERM_display(0,"按任意键返回....");

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return -1;
			}
		}
		return -1;
	}

	TERM_gotoxy(0,4);
	TERM_display(0,"重新上电成功!");
	Beep_AC(1000);
	sleep(1);

	return 0;
}


int barcod_version()
{
	int keych;
	char rev_buf[100] = {0};
	int i,count = 0;
	char version[9];
	struct termios newtio;
	TERM_console_open();
	TERM_clearscreen();
	//barcod_poweron();
	
	//system("setio 8 8");// 8 8 ′??? 9600 
	fd232_1 = open(BARCODE_DEVICE, O_RDWR | O_NOCTTY); 

	if (fd232_1 <0) 
	{
		printf("Poweron is FAIL\n");
		return -1;
	}
  
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE1 | CS8 | CLOCAL | CREAD ;
	
	tcflush(fd232_1, TCIFLUSH);
	tcsetattr(fd232_1,TCSANOW,&newtio);
  printf("Poweron is Success\n");

RESTART:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按确认键检测");
	TERM_gotoxy(0,2);
	TERM_display(0,"按返回退出");
	
	

	while(1)
	{
		keych = TERM_getkey(KEEPWAITING);
		printf("__%x__\n",keych);
		if(keych == 0x0D)
		{
			printf("send buf :");
			for(i = 0;i<ARRAY_SIZE(upda_buf);i++)
			{
				printf("%02x ",upda_buf[i]);
			}
			printf("\n");
			
			write(fd232_1, upda_buf,ARRAY_SIZE(upda_buf));
			sleep(1);
			tcflush(fd232_1,TCIFLUSH);//清空缓冲区 不需要第一个返回
			
			printf("send buf1 :");
			for(i = 0;i<ARRAY_SIZE(upda_buf1);i++)//?o?12+1 **
			{
				printf("%02x ",upda_buf1[i]);
			}
			printf("\n");
			
			write(fd232_1,upda_buf1,ARRAY_SIZE(upda_buf1));
			sleep(1);
			count = read(fd232_1, rev_buf, 50); 
			printf("read BUF1\n");
			for(i = 0;i < count;i++)
			{
				printf("%02x ",rev_buf[i]);
			}
			printf("\n");
			
			memcpy(version,rev_buf + 16,8);			
			version[9] = "\0";
			printf("version == %s__\n",version);
			
				TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"版本号:");
				TERM_gotoxy(8,1);
				TERM_display(0,version);	
						
			keych = TERM_getkey(KEEPWAITING);
			if(keych > 0)
			{
				goto RESTART;
			}
		}
		else if(keych == 0x1b)
		{
			close(fd232_1);
			Barcode_poweron_version();
			return -1;
		}
	}
}
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




typedef union 
{
	unsigned char buff[2];
	 int byte_2_num;
}type_2num;


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

int gettime(char * dest)
{
	char year[30];
	char month[10];
	char  day[10];
	char  hour[10];
	char minute[10];
	char  second[10];

	time_t now;
	struct tm *time_strt;
	if((now=time(NULL))==-1)
	{
		printf("time error\n");
		return -1;
	}	
	
	time_strt=localtime(&now);
	
	if(time_strt==NULL)
	{
		printf("time error\n");
		return -1;
	}
	sprintf(year,"%4d",time_strt->tm_year+1900);
	sprintf(month,"%02d",time_strt->tm_mon+1);
	sprintf(day,"%02d",time_strt->tm_mday);
	sprintf(hour,"%02d",time_strt->tm_hour);
	sprintf(minute,"%02d",time_strt->tm_min);
	sprintf(second,"%02d",time_strt->tm_sec);
	
	strcat(year,month);  
	strcat(year,day);
	strcat(year,hour);
	strcat(year,minute);
	strcat(year,second);

	strcpy(dest,year);
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
		return 0;
	}		
	printf("\r\n232 error_11\r\n");
	return -1;
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

int mmc_load_ext2()
{
	FILE *fd;
	char ch_txt[300];
	char buf[30];
	char buf1[30];
	int len;
	int mount_succ=0;
  int i;
  
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载EXT格式TF卡");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");

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
	unsigned char *sys_menu[]={"1:TF卡单次测试","2:TF卡10次测试","3:TF卡100次测试","4.按返回键返回"};	
	
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
				return 100;
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

	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载TF卡驱动");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");
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
		TERM_gotoxy(0,4);
		TERM_display(0,"加载失败,请重新加载");
		sleep(1);

		return 0;
	}

	TERM_clearscreen();
	TERM_gotoxy(0,3);
	TERM_display(0,"加载TF卡驱动成功!");
	usleep(10000);

	return 1;
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
	unsigned char *sys_menu[]={"1:单次扫描","2:10次循环扫描","3:500次循环扫描","4.版本测试"};	
	
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
				return 500;
			case 4:
				return 4;
			default:
				continue;
		}
	}
}

void Scan_barcode_round(int k)
{
	char tmpstr[255],ch_info[50];
	char ch_wr_txt[255],time_str[20];
	char buf[30];
  int i;
	int l_n=0;
	int ln_error=0;

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
			printf("\n************\n");
			puts(tstr);
			sprintf(tmpstr, "%s", tstr);
			memset(bcode,0,20);
			memcpy(bcode,tmpstr,strlen(tmpstr) );
			bcode[strlen(bcode)] = '\0';
			printf("\n************\n");
			puts(bcode);
			printf("\n************\n");
			if (memcmp(bcode+3,"NR",2)==0)
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
	      		TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"扫描完成");
				TERM_gotoxy(0,2);
				TERM_display(0,ch_info);
				TERM_gotoxy(0,3);
				TERM_display(0,buf);
				TERM_gotoxy(0,4);
				TERM_display(0,"按返回退出");
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
	TERM_gotoxy(0,4);
	TERM_display(0,"按返回键返回上级");

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
				memcpy(bcode,tmpstr ,strlen(tmpstr));
				bcode[strlen(bcode)] = '\0';
				printf("\n*********\n");
				puts(bcode);
				printf("\n*********\n");
				if (strstr(bcode,"NR")==0)
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
				TERM_gotoxy(0,3);
				TERM_display(0,"条码扫描成功!");
				TERM_gotoxy(0,4);
				TERM_display(0,bcode);
				Beep_AC(1000);
				goto re_scan_barcode;

			}
		}
		else if (key == KEY_ESC)
		{
			return;
		}
	}

	return;
}

int Barcode_poweron()
{
	int ret;
	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"条码正在上电");
	TERM_gotoxy(0,2);
	TERM_display(0,"请等待...");
	
	BARCODE_poweroff();

	if ((ret=BARCODE_poweron())!=BC_ERROR_NONE)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,3);
		TERM_display(0,"条码上电失败!");
		TERM_gotoxy(0,4);
		TERM_display(0,"按任意键返回....");

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return -1;
			}
		}
		return -1;
	}

	TERM_gotoxy(0,4);
	TERM_display(0,"条码上电成功!");
	Beep_AC(1000);
	sleep(1);

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
		else if(lt == 4)
		{
			barcod_version();
			goto re_sub_scanbarcod_menu;
		}
		
			Scan_barcode_round(lt);
			//goto re_sub_scanbarcod_menu;
			
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_ESC)
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
				case 0x36:
					return 6;	
				case 0x37:
					return 7;
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
				case 0x43:
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
				case 0x43:
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
	unsigned char *sys_menu[]={"1:M1卡测试","2:TI卡测试","3:CPU卡测试","4:SAM卡测试","5:TF卡测试","6:Icode2测试"};	
	
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
	unsigned char *sys_menu[]={"1:按键测试","2:时钟测试","3:背光测试","4:电量测试","5:蜂鸣器测试","6:读取PID","7:读取记录"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,7))
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
			case 7:
				return 7;	
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
	TERM_display(0,"0642Fullfunction");
	TERM_gotoxy(7,3);
	TERM_display(0,"V2.9");

	if( mkdir("/mnt/a/userapp/sys_txt",00700)== -1)
    		//puts("mkdir /mnt/a/userapp/sys_txt error.");

	//set_sleep_level_LCDSLEEP();

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



int get_id()
{
	
	FILE *fd;
		int i;
	char flash_buf[34];
	memset(flash_buf,0,34);
 // getnanduid(flash_buf);
  
  system(" cat /sys/class/MC2006/nanduid/nanduid > /mnt/a/id");
 	if((fd=fopen("/mnt/a/id","r+"))==NULL) 
	{
		fclose(fd);
		return -1;
	}
	memset(flash_buf,0,34);
	fgets(flash_buf,34,fd);
	//flash_buf[32] = '\0';
	fclose(fd); 
	puts(flash_buf);
  //getnanduid_flashid(flash_buf);
//    for(i=0;i<32;i++)
//   {
//   	printf("%02x",flash_buf[i]);
//   	}
//   	printf("\n");
 	TERM_clearscreen();
	TERM_gotoxy(0,0);
	TERM_display(0,flash_buf); 
		while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{
			return 0;
		}
	} 	 

	}


int read_pid()
 {
  unsigned char ibuf[1024];
  unsigned char buf[2];
  unsigned char sum[4];
  char pid_sc[32];
  long int total_num=0;
  FILE * fout;
  
  memset(buf,0,2);
  memset(sum,0,4);
  memset(pid_sc,0,32);
  
  if (!(fout = fopen(MTD_DEV, "rb"))) {
    fprintf(stderr, "unabled to open %s\n", MTD_DEV);
    return -1;
  }
  fseek(fout, 5119*1024UL, SEEK_SET);
  if (fread(ibuf, 512, 1, fout) != 1) {
    perror("reading "MTD_DEV);
  }
  {
    int i, blank=1;
    for (i = 0; i < 32; i++) {
      if (ibuf[i] != 0xFF) {
	    blank = 0;
      }
    }
    if (! blank) {
      for(i=16;i<32;i++)
       	 {
       	 	total_num+=ibuf[i];
       	 	}
 		     buf[1]=total_num>>8;
		     buf[0]=total_num&0x00ff;
		     sprintf(sum,"%02X%02X",buf[0],buf[1]);	        
		     if(memcmp(sum,ibuf+32,4)==0)  
		     	{	
           for (i = 0; i < 32; i++) {
	         printf("%c", isprint(ibuf[i])?ibuf[i]:'.');
          }
        printf("\n");
        memcpy(pid_sc,ibuf,32);
        pid_sc[32]='\0';
        fclose(fout);
        
  	   TERM_clearscreen();
	     TERM_gotoxy(0,0);
	     TERM_display(0,pid_sc); 
		  while(1)
	       {
		     key =TERM_getkey(KEEPWAITING);
		   if (key > 0)
		    {
			   return 0;
		    }
	      }       

       }
       else
       	{
       		printf("pid no written \n");
       TERM_clearscreen();
	     TERM_gotoxy(0,0);
	     TERM_display(0,"PID为空"); 
	     sleep(2);
       		  fclose(fout);
       		  return -1;
       		} 
    }

    else {
      fprintf(stderr, "PID blank\n");
       TERM_clearscreen();
	     TERM_gotoxy(0,0);
	     TERM_display(0,"PID为空"); 
	     sleep(2);      
        fclose(fout);
        return -1;
    }
  }

}


int main(int argc, char *argv[])
{
	int mPtr,mPtr_1,mPtr_2,mPtr_3,mPtr_4;
  int i;
  char buf_flash_id[34];
  
  memset(buf_flash_id,0,34);
	TERM_console_open();
	TERM_clearscreen();

	sub_main_welcome();
  

sub_main_lable:
	mPtr = sub_main();
	if (mPtr == 1)
	{
sub_main_menulabel_1_label:		
		mPtr_1 = sub_main_menulabel_1();
		if (mPtr_1 == 0)
		{
			goto sub_main_lable;
		}
		else if (mPtr_1 == 1)
		{
			Presskey_test();
			goto sub_main_menulabel_1_label;
		}
		else if (mPtr_1 == 2)
		{
			DateTime_Setting();
			goto sub_main_menulabel_1_label;
		}
		else if (mPtr_1 == 3)
		{
			Backlight_test();
			goto sub_main_menulabel_1_label;
		}
		else if (mPtr_1 == 4)
		{
			Voltage_test();
			goto sub_main_menulabel_1_label;
		}
		else if (mPtr_1 == 5)
		{
			get_id();
			goto sub_main_menulabel_1_label;
		}
		else if (mPtr_1 == 6)
		{
			
					read_pid();
//			i=setnanduid("11SC140424200009");
//			printf("i=%+d\n",i);
			goto sub_main_menulabel_1_label;
		}	
		else if(mPtr_1==7)
			{
				Check_record();
				goto sub_main_menulabel_1_label;
				}
	}
	else if (mPtr == 2)
	{
sub_main_menulabel_2_label:			
		mPtr_1 = sub_main_menulabel_2();
		if (mPtr_1 == 0)
		{
			goto sub_main_lable;
		}
		else if (mPtr_1 == 1)
		{
			//Read_pn512();
			Read_M1card_test();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 2)
		{
			Read_Ticard_test();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 3)
		{
			Read_cpucard_test();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 4)
		{
			Read_samcard_test();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 5)
		{
			 mPtr_3 = mmc_load_menu();
			 if(mPtr_3==0)
			 		goto sub_main_menulabel_2_label;
			 else
			 	{		
			  for(i=0;i<mPtr_3;i++)
			  {
		    mmc_load();								
				mmc_load_ext2();
			  }
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);
			 while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
				mount_time=0;
				mount_error=0;
				 goto sub_main_menulabel_2_label;
				}
			}
			  
		   }
		}
		else if (mPtr_1 == 6)
		{
			Read_Icode2card_test();
			goto sub_main_menulabel_2_label;
		}
	}
	else if (mPtr == 3)
	{
		Scan_barcode_test();
		goto sub_main_lable;
	}
	else if (mPtr == 4)
	{
sub_main_menulabel_3_label:			
		mPtr_1 = sub_main_menulabel_3();
		if (mPtr_1 == 0)
		{
			goto sub_main_lable;
		}
		else if (mPtr_1 == 1)
		{
sub_system_menu_label:			
			mPtr_2 = sub_system_menu();
			if (mPtr_2 == 0)
			{
				goto sub_main_menulabel_3_label;
			}
			else if (mPtr_2 == 1)
			{
				Essid_Setting_term();
				goto sub_system_menu_label;
			}
			else if (mPtr_2 == 2)
			{
				Server_IP_Setting_term();
				Server_COM_Setting_term();
				goto sub_system_menu_label;
			}
			else if (mPtr_2 == 3)
			{
				Server_IP_Setting_term_1();
				Server_COM_Setting_term_1();
				goto sub_system_menu_label;
			}
		}
		else if (mPtr_1 == 2)
		{
			Open_Irda_com();
sub_irda_menu_label:			
			mPtr_2 = Irda_test();
			if (mPtr_2 == 0)
			{
				goto sub_main_menulabel_3_label;
			}
			else if (mPtr_2 == 1)
			{
				Send_Irda_info();
				goto sub_irda_menu_label;
			}
			else if (mPtr_2 == 2)
			{
				Receive_Irda_info();
				goto sub_irda_menu_label;
			}
		}
		else if (mPtr_1 == 3)
		{
//			mPtr_2 = Change_wpa_essid();
//			if (mPtr_2 == 0)
//			{
//				goto sub_main_menulabel_3_label;
//			}
//			else if (mPtr_2 == 1)
//			{
				mPtr_3 = Link_wifi();
				if (mPtr_3 == 0)
				{
					goto sub_main_menulabel_3_label;
				}
				else if (mPtr_3 == 1)
				{
					mPtr_4 = Send_wifi_menu();
					if (mPtr_4 == 1)
					{
						Send_wifi_info();
						goto sub_main_menulabel_3_label;
					}
					else
					{
						TERM_clearscreen();
						TERM_gotoxy(0,3);
						TERM_display(0,"正在断开无线网返回...");
						system("/mnt/a/userapp/sys_txt/wlandown");
						goto sub_main_menulabel_3_label;
					}
				}
			
		}
		else if (mPtr_1 == 4)
		{
sub_main_menulabel_6_label:
			l_mode = 0;//
			
			mPtr_2 = sub_main_menulabel_6();
			if (mPtr_2 == 0)
			{
				goto sub_main_menulabel_3_label;
			}
			else if (mPtr_2 == 1)
			{
				Mobile_poweron_csq();
				goto sub_main_menulabel_6_label;
			}
			else if (mPtr_2 == 2)
			{
				Mobile_call_phone();
				goto sub_main_menulabel_6_label;
			}
			else if (mPtr_2 == 3)
			{
				Mobile_pppon_pppoff();
				goto sub_main_menulabel_6_label;
			}
			else if (mPtr_2 == 4)
			{
re_send_down_menu_label:
				mPtr_3 = send_down_menu();
				if (mPtr_3 == 0)
				{
					goto sub_main_menulabel_6_label;
				}
				else if (mPtr_3 == 1)
				{
					Mobile_send_info();
					goto re_send_down_menu_label;
				}
				else if (mPtr_3 == 2)
				{
					Mobile_down_info();
					goto re_send_down_menu_label;
				}
			}
			else if (mPtr_2 == 5)
			{
				Mobile_poweroff_pppoff();
				goto sub_main_menulabel_6_label;
			}
		}
		else if (mPtr_1 == 5)
		{
sub_main_menulabel_5_label:
			l_mode = 1;//CDMA
			mPtr_2 = sub_main_menulabel_5();
			if (mPtr_2 == 0)
			{
				goto sub_main_menulabel_3_label;
			}
			else if (mPtr_2 == 1)
			{
				Mobile_poweron_csq();
				goto sub_main_menulabel_5_label;
			}
			else if (mPtr_2 == 2)
			{
				Mobile_call_phone();
				goto sub_main_menulabel_5_label;
			}
			else if (mPtr_2 == 3)
			{
				Mobile_pppon_pppoff();
				goto sub_main_menulabel_5_label;
			}
			else if (mPtr_2 == 4)
			{
re_send_down_menu_label_1:
				mPtr_3 = send_down_menu();
				if (mPtr_3 == 0)
				{
					goto sub_main_menulabel_5_label;
				}
				else if (mPtr_3 == 1)
				{
					Mobile_send_info();
					goto re_send_down_menu_label_1;
				}
				else if (mPtr_3 == 2)
				{
					Mobile_down_info();
					goto re_send_down_menu_label_1;
				}
			}
			else if (mPtr_2 == 5)
			{
				Mobile_poweroff_pppoff();
				goto sub_main_menulabel_5_label;
			}
		}
	}
	else if (mPtr == 5)
	{
sub_main_menulabel_4_label:			
		mPtr_1 = sub_main_menulabel_4();
		if (mPtr_1 == 0)
		{
			goto sub_main_lable;
		}
		else if (mPtr_1 == 1)
		{
			Eth0_kzw_test_open();
			goto sub_main_menulabel_4_label;
		}
		else if (mPtr_1 == 2)
		{
			Eth0_kzw_test_close();
			goto sub_main_menulabel_4_label;
		}
		else if (mPtr_1 == 3)
		{
			Udisk_kzw_test_open();
			goto sub_main_menulabel_4_label;
		}
		else if (mPtr_1 == 4)
		{
			Udisk_kzw_test_close();
			goto sub_main_menulabel_4_label;
		}
	}
}
