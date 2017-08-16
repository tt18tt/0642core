#include <sys/socket.h> 
#include <netdb.h>
#include <dlfcn.h>

#define BAUDRATE B115200
#define TIME_OUT_TIME 20

char *RS232_DEVICE = "/dev/s3c2410_serial2";
char *RS232_DEVICE0 = "/dev/s3c2410_serial0";

void Mobile_call_phone()
{
	char ch_number[20],ch_send[50];
	char buf[1024];

re_call:
	TERM_clearscreen();
	memset(ch_number,0,20);

	TERM_gotoxy(0,1);
	TERM_display(0,"请输入电话号码:");

	if (ReadString_term(ch_number,16,0,2,0) < 0) return;

	ch_number[strlen(ch_number)] = '\0';
	printf("\r\naa\r\n");
	printf(ch_number);
	printf("aa\r\n");

	if (l_mode == 1)		//CDMA
	{
		if (pp_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块已经联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先断网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块没有上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先上电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块开始拨号");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");

		memset(ch_send,0,50);
		memcpy(ch_send,"AT+CDV",6);
		memcpy(ch_send + 6,ch_number,strlen(ch_number));
		memcpy(ch_send + 6 + strlen(ch_number),"\r",1);
		ch_send[strlen(ch_send)]='\0';

		tcgetattr(0,&savedtio);
	
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			return;
		}
		
		set_serial_raw_1(fd232, 1);
		
		memset(buf,0,1024);
		SendCommand_1(fd232,ch_send,buf);                           //check gprs card present
		if  (strstr(buf,"OK"))
		{
			printf("\r\naaaa\r\n");
			printf(buf);

			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块拨号成功");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
			while (1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					call_phone_down();
					TERM_clearscreen();
					TERM_displayxy(0,4,NORMALDISPLAY,"CDMA模块断开成功");
					sleep(1);
					goto re_call;
				}
			}
		}
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块拨号失败");
		TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		if (pp_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块已经联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先断网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块没有上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先上电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块开始拨号");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");

		memset(ch_send,0,50);
		memcpy(ch_send,"ATD",3);
		memcpy(ch_send + 3,ch_number,strlen(ch_number));
		memcpy(ch_send + 3 + strlen(ch_number),";\r",2);
		ch_send[strlen(ch_send)]='\0';

		tcgetattr(0,&savedtio);
	
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			return;
		}
		
		set_serial_raw_1(fd232, 1);
		
		memset(buf,0,1024);
		SendCommand_1(fd232,ch_send,buf);                           //check gprs card present
		if  (strstr(buf,"OK"))
		{
			printf("\r\naaaa\r\n");
			printf(buf);

			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块拨号成功");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
			while (1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					call_phone_down();
					TERM_clearscreen();
					TERM_displayxy(0,4,NORMALDISPLAY,"GPRS模块断开成功");
					sleep(1);
					goto re_call;
				}
			}
		}
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块拨号失败");
		TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

	return;
}

void Mobile_poweroff_pppoff()
{
	if (l_mode == 1)		//CDMA
	{
		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块已经下电");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		if (pp_on == 1)			//已联网ppp0
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块正在下电断网");
			TERM_displayxy(0,3,NORMALDISPLAY,"请等待...");
			system("/mnt/a/mc2006/driverscript/mobile_pppoff");
			sleep(1);
			system("killall /mnt/a/bin/pppd");
			sleep(1);
			system("/mnt/a/mc2006/driverscript/mobile_poweroff");
			sleep(1);
			p_on = 0;
			pp_on = 0;
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块下电断网成功");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块正在下电");
		TERM_displayxy(0,3,NORMALDISPLAY,"请等待...");
		system("/mnt/a/mc2006/driverscript/mobile_poweroff");
		sleep(1);
		p_on = 0;
		pp_on = 0;
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块下电成功");
		TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块已经下电");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		if (pp_on == 1)			//已联网ppp0
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块正在下电断网");
			TERM_displayxy(0,3,NORMALDISPLAY,"请等待...");
			system("/mnt/a/mc2006/driverscript/mobile_pppoff");
			sleep(1);
			system("killall /mnt/a/bin/pppd");
			sleep(1);
			system("/mnt/a/mc2006/driverscript/mobile_poweroff");
			sleep(1);
			p_on = 0;
			pp_on = 0;
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块下电断网成功");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块正在下电");
		TERM_displayxy(0,3,NORMALDISPLAY,"请等待...");
		system("/mnt/a/mc2006/driverscript/mobile_poweroff");
		sleep(1);
		p_on = 0;
		pp_on = 0;
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块下电成功");
		TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

int getmobileinfo()
{
	char buf[1024],ch_csq[20],ch_view[50];
	
	if (l_mode == 1)		//CDMA
	{
		tcgetattr(0,&savedtio);
		
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			return -1;
		}
		
		set_serial_raw_1(fd232,1);

		SendCommand_1(fd232,"AT+CSQ?\r",buf);                             //check gprs card present
		if  (strstr(buf,"OK"))
		{
			printf("\r\naaaa\r\n");
			printf(buf);
			
			memset(ch_csq,0,20);
			memcpy(ch_csq,buf + 16,2);
			ch_csq[2]='\0';
			
			memset(ch_view,0,50);
			sprintf(ch_view,"当前CDMA信号强度为:%s",ch_csq);
			ch_view[strlen(ch_view)] = '\0';

			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,ch_view);

			return 0;
		}
		return -1;
	}
	else //GPRS
	{
		tcgetattr(0,&savedtio);
		
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			sleep(10);		
			return -1;
		}
		
		set_serial_raw_1(fd232,1);
		
		TERM_displayxy(0,3,NORMALDISPLAY,"准备发送信号指令");
		sleep(2);
		SendCommand_1(fd232,"AT+CSQ\r",buf);                             //check gprs card present
		if  (strstr(buf,"OK"))
		{
			printf("\r\naaaa\r\n");
			printf(buf);
			
			memset(ch_csq,0,20);
			memcpy(ch_csq,buf + 15,2);
			ch_csq[2]='\0';
			
			memset(ch_view,0,50);
			sprintf(ch_view,"当前GPRS信号强度为:%s",ch_csq);
			ch_view[strlen(ch_view)] = '\0';

			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,ch_view);
			return 0;
		}
		return -1;

	}
}


int getmobileinfo_1()
{
	char buf[1024],ch_csq[20],ch_view[50];
	int i=0;
	
	if (l_mode == 1)		//CDMA
	{
		tcgetattr(0,&savedtio);
		
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			TERM_displayxy(0,4,NORMALDISPLAY,"串口打开失败");
			return -1;
		}
		
		set_serial_raw_1(fd232,1);

		SendCommand_1(fd232,"AT+CSQ?\r",buf);                             //check gprs card present
		if  (strstr(buf,"OK"))
		{
			printf("\r\naaaa\r\n");
			printf(buf);
			
			memset(ch_csq,0,20);
			memcpy(ch_csq,buf + 16,2);
			ch_csq[2]='\0';
			
			memset(ch_view,0,50);
			sprintf(ch_view,"当前CDMA信号强度为:%s",ch_csq);
			ch_view[strlen(ch_view)] = '\0';

			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,ch_view);

			return 0;
		}
		TERM_displayxy(0,4,NORMALDISPLAY,"串口2打开失败");
		return -1;
	}
	else //GPRS
	{
		tcgetattr(0,&savedtio);
		
		fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
		if (fd232 <0) 
		{
			perror(RS232_DEVICE0);      
			tcsetattr(0,TCSANOW,&savedtio);
			TERM_displayxy(0,1,NORMALDISPLAY,"串口3打开失败");
			sleep(10);		
			return -1;
		}
		
		set_serial_raw_1(fd232,1);
	
		TERM_displayxy(0,3,NORMALDISPLAY,"AT指令测试");
		sleep(2);
Reget:			
		SendCommand_1(fd232,"AT\r",buf);                             //check gprs card present
		if  (strstr(buf,"OK"))
		{
//			printf("\r\naaaa\r\n");
//			printf(buf);
//			
//			memset(ch_csq,0,20);
//			memcpy(ch_csq,buf + 15,2);
//			ch_csq[2]='\0';
//			
//			memset(ch_view,0,50);
//			sprintf(ch_view,"当前GPRS信号强度为:%s",ch_csq);
//			ch_view[strlen(ch_view)] = '\0';
//
//			TERM_clearscreen();
//			TERM_displayxy(0,1,NORMALDISPLAY,ch_view);
       TERM_displayxy(0,4,NORMALDISPLAY,"模块上电成功");
			return 0;
		}
		else
			{
				if(i<3)
					{
		    TERM_displayxy(0,3,NORMALDISPLAY,"AT测试失败");	
		    TERM_displayxy(0,4,NORMALDISPLAY,"等待重新测试");
		    sleep(5);
		    i++;
		    goto Reget;
		//return -1;
	   }
		 return -1;
	}
	}
}

void Mobile_poweron_csq()
{
	int ld=0;
	
	if (l_mode == 1)		//CDMA
	{
		if (p_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块已经上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先下电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块正在上电");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");
		system("/mnt/a/mc2006/driverscript/mobile_poweron");
		sleep(10);
		ld = getmobileinfo();
		if (ld == -1)
		{
			sleep(1);
			system("/mnt/a/mc2006/driverscript/mobile_poweroff");
			TERM_displayxy(0,4,NORMALDISPLAY,"CDMA上电失败");
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
		p_on = 1;
re_search_csq:
		TERM_displayxy(0,3,NORMALDISPLAY,"按确认键查询信号");
		TERM_displayxy(0,4,NORMALDISPLAY,"按返回键返回");
		while (1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key == KEY_ENTER)
			{
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,"正在查询信号");
				TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");
				ld = getmobileinfo();
				if (ld == -1)
				{
					TERM_clearscreen();
					TERM_displayxy(0,1,NORMALDISPLAY,"查询信号失败");
					TERM_displayxy(0,2,NORMALDISPLAY,"请重新查询...");
					sleep(1);
					return;
				}
				goto re_search_csq;
			}
			else if (key == KEY_ESC)
			{
				return;
			}
		}
	}
	else
	{
		if (p_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块已经上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先下电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块正在上电");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");
		system("/mnt/a/mc2006/driverscript/mobile_poweron");
		//PressAnyKey();
		sleep(10);
		ld = getmobileinfo_1();
		if (ld == -1)
		{
			sleep(1);
			system("/mnt/a/mc2006/driverscript/mobile_poweroff");
			TERM_displayxy(0,4,NORMALDISPLAY,"GPRS上电失败");
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
		p_on = 1;

re_search_csq_1:
		TERM_displayxy(0,3,NORMALDISPLAY,"按确认键查询信号");
		TERM_displayxy(0,4,NORMALDISPLAY,"按返回键返回");
		while (1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key == KEY_ENTER)
			{
				TERM_clearscreen();
				TERM_displayxy(0,1,NORMALDISPLAY,"正在查询信号");
				TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");
				ld = getmobileinfo();
				if (ld == -1)
				{
					TERM_clearscreen();
					TERM_displayxy(0,1,NORMALDISPLAY,"查询信号失败");
					TERM_displayxy(0,2,NORMALDISPLAY,"请重新查询...");
					sleep(1);
					return;
				}
				goto re_search_csq_1;
			}
			else if (key == KEY_ESC)
			{
				return;
			}
		}
	}

	return;
}

int get_host_ip()//获取网站IP地址
{
	FILE *fd;
	struct sockaddr_in si_me;
	char ch_txt[50];
	int len;
	char hostip[20];
	
	TERM_clearscreen();
	TERM_displayxy(0,1,NORMALDISPLAY,"正在进行域名解析");
	memset((char *) &si_me, 0, sizeof(struct sockaddr_in));
	memset(ch_txt,0,50);

	struct hostent* hostInfo = gethostbyname("uniriho.f3322.org");
	if (hostInfo != NULL)
		{
      memcpy((char *)&si_me.sin_addr.s_addr, hostInfo->h_addr, hostInfo->h_length);
      printf("address:%s\n",inet_ntoa(si_me.sin_addr));
      memcpy(ip_address,inet_ntoa(si_me.sin_addr),20);
      puts(ip_address);
			return 0;
		}
	else 
		{
			printf("\n gethostbyname error\n");
			return -1;
		}
	}

int gprs_rec_send(char *send_string)
{	
	char rcv[1024];
	
	if(send(sockfd,send_string,strlen(send_string),0)==-1)
	{
		close(sockfd);
		return -1;
	}
	
	memset(rcv,0,1024);
	if((nbytes=recv(sockfd,rcv,1023,0))==-1)
	{
		close(sockfd);
		return -1;	
	}
	rcv[nbytes]='\0';
	
	display_string("recv success");
	close(sockfd);
	return 1;
}



//********************************************************************************
//建立Socket连接
//********************************************************************************
int sockfd_create()
{
	char rcv[1024];
	
	struct sockaddr_in server_addr;
 	int portnumbers=ip_port;

  if(get_host_ip()==0)
  	{
  	
  		}	
  else
  	{
  		printf("\n gethostbyname error!!!\n");
  		TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"域名解析失败");
			sleep(2);
			return -1;
  		}		
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)	
	{
		printf("\nsocket error!\n");
		close(sockfd);
		return -1;
	}
	
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(portnumbers);
	server_addr.sin_addr.s_addr=inet_addr(ip_address);
	
	int error=-1, len;
	
	len = sizeof(int); 
	struct timeval tm;
	fd_set set;
	unsigned long ul = 1; 
	ioctl(sockfd, FIONBIO, &ul); //设置为非阻塞模式 
	int ret = 0;
	
	printf("\r\nbegin connect\r\n");
	
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	{
		printf("\r\nbegin connect time\r\n");
		tm.tv_sec = TIME_OUT_TIME; 
		tm.tv_usec = 0; 
		FD_ZERO(&set); 
		FD_SET(sockfd, &set); 
		
		if( select(sockfd+1, NULL, &set, NULL, &tm) > 0) 
		{ 
			getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len); 
			if(error == 0) 
			{
				ret = 1;
			}
			else 
			{
				ret = 0; 
			}
		} 
		else 
		{
			ret = 0;
		}
	}
	else 
	{
		ret = 1; 
	}
	ul = 0; 
	ioctl(sockfd, FIONBIO, &ul); //设置为阻塞模式 
	if(!ret) 
	{ 
		close( sockfd ); 
		printf("\nConnect error!\n");
		return -1; 
	}
	
	struct timeval timeout = {50,0};
	 
	printf("\nConnected!\n");
	
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	
	return 0;
}



int gprs_rec_send_1(char *send_string)
{	
	char rcv[1024];
	
	if(send(sockfd,send_string,strlen(send_string),0)==-1)
	{
		close(sockfd);
		return -1;
	}
	
	memset(rcv,0,1024);
	
	if((nbytes=recv(sockfd,rcv,1023,0))==-1)
	{
		close(sockfd);
		return -1;	
	}
	rcv[nbytes]='\0';

	display_string("begin receive...");
	display_string(rcv);
	
 // Run("echo","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);
//	TERM_displayxy(0,3,NORMALDISPLAY,rcv);
//			while(1)
//			{
//				key =TERM_getkey(KEEPWAITING);
//				if (key > 0)
//				{
//					return;
//				}
//			}	
	if (strstr(rcv,"ok"))
	{
		display_string("recv success");
		return 1;
	}
	else if (strstr(rcv,"ERROR"))
	{
		display_string("recv error");
		return -1;
	}

	return -1;
}

int gprs_rec_send_2(char *send_string)
{	
	char rcv[1024],sall[10];
	char ch_rcv_1[1024],ch_jyw_1[10];
	int ltr,lta;
	
	if(send(sockfd,send_string,strlen(send_string),0)==-1)
	{
		close(sockfd);
		return -1;
	}
	
	memset(rcv,0,1024);
	if((nbytes=recv(sockfd,rcv,1023,0))==-1)
	{
		close(sockfd);
		return -1;	
	}
	rcv[nbytes]='\0';

	display_string("begin receive...");
	display_string(rcv);

	memset(ch_rcv_1,0,1024);
	memcpy(ch_rcv_1,rcv,nbytes-4);
	ch_rcv_1[strlen(ch_rcv_1)] = '\0';

	display_string(ch_rcv_1);

	ltr = strtoint(ch_rcv_1);
	if (ltr == 0)
	{
		memset(sall,0,10);
		memcpy(sall,"0000",4);
		sall[4] = '\0';
		lta = strlen(all);
		if (lta < 4 )
		{
			memcpy(sall + (4 - lta),all,strlen(all));
			sall[4] = '\0';
		}
		else if (lta > 4)
		{
			memcpy(sall,all + (lta - 4),4);
			sall[4] = '\0';
		}
		else
		{
			memcpy(sall,all,4);
			sall[4] = '\0';
		}
	}

	display_string(sall);

	memset(ch_jyw_1,0,1024);
	memcpy(ch_jyw_1,rcv + nbytes-4,4);
	ch_jyw_1[strlen(ch_jyw_1)] = '\0';

	display_string(ch_jyw_1);

	if (str_n_cmp(ch_jyw_1,sall) == -1)
	{
		display_string("recv error");
		return -1;
	}
	else
	{
		display_string("recv success");
		return 1;
	}
	
	return 0;
}



int get_sys_com_gprscdma()
{
	FILE *fp;
	int len;
	char com_id[20];
	
	memset(com_id,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/com_1","a+"))==NULL) 
	{
		fclose(fp);
		return -1;
	}
	fgets(com_id,20,fp);
	fclose(fp);
	com_id[strlen(com_id)] = '\0';
	len = strlen(com_id);
	if (len == 0)
	{
		return -1;
	}
	
	ip_port = atoi(com_id);
	
	return 1;
}

int get_sys_ip_gprscdma()
{
	FILE *fp;
	int len;
	
	memset(ip_address,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/ip_1","a+"))==NULL) 
	{
		fclose(fp);
		return -1;
	}
	fgets(ip_address,20,fp);
	fclose(fp);
	ip_address[strlen(ip_address)] = '\0';
	len = strlen(ip_address);
	if (len == 0)
	{
		return -1;
	}
	
	return 1;
}

void Mobile_down_info()
{
	FILE *fd;
	char ch_txt[1500],sall[10],ch_display[50];
	int len,pd,ln,ln_1;
	int ltr,lta;
	int Date_send=0;

	if (pp_on == 0)
	{
		if (l_mode == 1)		//CDMA
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块没有联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先联网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块没有联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先联网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

	memset(ch_txt,0,1500);
	if((fd=fopen("/mnt/a/userapp/b.txt","r+"))==NULL) 
	{
		fclose(fd);
		return;
	}
	fseek(fd,0,SEEK_END);
	len = ftell(fd);
	fclose(fd);

	if((fd=fopen("/mnt/a/userapp/b.txt","r+"))==NULL) 
	{
		fclose(fd);
		return;
	}
	fread(ch_txt,len,1,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);

	display_string(ch_txt);


	pd = get_sys_ip_gprscdma();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"无服务端IP!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		return;
	}
	
	pd = get_sys_com_gprscdma();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"无服务端端口!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		return;
	}

	pd = sockfd_create();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,2);
		TERM_display(0,"连接服务器失败,请查看服务端是否打开!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);
		return;
	}
	ln = 0;
	ln_1 = 0;

re_down_info:
		TERM_clearscreen();
	TERM_displayxy(0,1,NORMALDISPLAY,"正在下载数据...");
	for(Date_send=0;Date_send<100;Date_send++)
	{

	//TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");

	pd = gprs_rec_send_2(ch_txt);
	if (pd == 1)
	{
				if (l_mode == 1)		//CDMA
		{
			ln++;
			memset(ch_display,0,50);
			sprintf(ch_display,"下载成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"CDMA下载成功!");
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);
			sleep(1);
			//goto re_down_info;
		}
		else
		{

			ln++;
			memset(ch_display,0,50);
			sprintf(ch_display,"下载成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"GPRS下载成功!");
			Beep_AC(200);
			//goto re_down_info;
		}
	}
else
	{
		  if(l_mode==1)
		  	{
			ln_1++;
			memset(ch_display,0,50);
			sprintf(ch_display,"下载成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"CDMA下载失败!");
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);
			sleep(1);
			//goto re_down_info;
		}
		
		else
		{
			ln_1++;
			memset(ch_display,0,50);
			sprintf(ch_display,"下载成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"GPRS下载失败!");
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);
		}
	}
}	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					return;
				}
			}
}

void Mobile_send_info()
{
	FILE *fd;
	char ch_txt[1500],sall[10],ch_display[50];
	int len,pd,ln,ln_1;
	int ltr,lta;
	int i;

	if (pp_on == 0)
	{
		if (l_mode == 1)		//CDMA
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块没有联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先联网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块没有联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先联网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

	memset(ch_txt,0,1500);
	if((fd=fopen("/mnt/a/userapp/a.txt","r+"))==NULL) 
	{
		fclose(fd);
		return;
	}
	fseek(fd,0,SEEK_END);
	len = ftell(fd);
	fclose(fd);

	if((fd=fopen("/mnt/a/userapp/a.txt","r+"))==NULL) 
	{
		fclose(fd);
		return;
	}
	fread(ch_txt,len,1,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);

	//display_string(ch_txt);//

	ltr = strtoint(ch_txt);
	if (ltr == 0)
	{
		memset(sall,0,10);
		memcpy(sall,"0000",4);
		sall[4] = '\0';
		lta = strlen(all);
		if (lta < 4 )
		{
			memcpy(sall + (4 - lta),all,strlen(all));
			sall[4] = '\0';
		}
		else if (lta > 4)
		{
			memcpy(sall,all + (lta - 4),4);
			sall[4] = '\0';
		}
		else
		{
			memcpy(sall,all,4);
			sall[4] = '\0';
		}
		
		memcpy(ch_txt + strlen(ch_txt),sall,4);
		ch_txt[strlen(ch_txt)] = '\0';
	}

//	display_string(ch_txt);

//	pd = get_sys_ip_gprscdma();
//	if (pd == -1)
//	{
//		TERM_clearscreen();
//		TERM_gotoxy(0,4);
//		TERM_display(0,"无服务端IP!");
//		Beep_AC(200);
//		usleep(500);
//		Beep_AC(200);
//
//		return;
//	}
	
	pd = get_sys_com_gprscdma();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"无服务端端口!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		return;
	}

	pd = sockfd_create();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,2);
		TERM_display(0,"连接服务器失败,请查看服务端是否打开!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);
		return;
	}
	ln = 0;
	ln_1 = 0;

re_send_info:
	TERM_clearscreen();
	TERM_displayxy(0,1,NORMALDISPLAY,"正在发送数据...");
//	TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");
for(i=1;i<101;i++)
{
	pd = gprs_rec_send_1(ch_txt);
	if (pd == 1)
	{
			TERM_displayxy(0,2,NORMALDISPLAY,"数据发送成功");
		if (l_mode == 1)		//CDMA
		{
			ln++;
			memset(ch_display,0,50);
			sprintf(ch_display,"发送成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"CDMA发送成功!");
			Beep_AC(200);
//			usleep(500);
//			Beep_AC(200);
//			sleep(1);
		//	goto re_send_info;
		}
		else
		{

			ln++;
			memset(ch_display,0,50);
			sprintf(ch_display,"发送成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"GPRS发送成功!");
			Beep_AC(200);
//			usleep(500);
//			Beep_AC(200);
//			sleep(1);
		}
	}
	else if(pd == -1)
	{
		//sleep(1);
		if (l_mode == 1)		//CDMA
		{
			ln_1++;
			memset(ch_display,0,50);
			sprintf(ch_display,"发送成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"CDMA发送失败!");
			Beep_AC(200);
			usleep(500);
			Beep_AC(200);
			sleep(1);
			//goto re_send_info;
		}
		else
		{
			ln_1++;
			memset(ch_display,0,50);
			sprintf(ch_display,"发送成功 %d 次,失败 %d 次",ln,ln_1);
			ch_display[strlen(ch_display)] = '\0';
			TERM_clearscreen();
			TERM_gotoxy(0,1);
			TERM_display(0,ch_display);
			TERM_gotoxy(0,3);
			TERM_display(0,"GPRS发送失败!");
			Beep_AC(200);
		}
	 }

}	
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

int send_down_menu()
{
	unsigned char *sys_menu[]={"1:上传数据","2:下载数据"};	
	
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


int get_ppp()
{
	char ch_buf[300];
	FILE *fd;

	memset(ch_buf,0,300);
	if((fd=fopen("/proc/net/dev","r"))==NULL)
	{
		printf(0,"打不开文件/proc/net/dev");
		return 0;
	}
	while(!feof(fd))
	{
		fgets(ch_buf,250,fd);
		printf(ch_buf);
		printf("\r\naaaaaaa\r\n");
		if (strstr(ch_buf,"ppp0:")!=NULL)
		{
			fclose(fd);
			return 1;
		}
	}
	fclose(fd);

	return 0;
}

void Mobile_pppon_pppoff()
{
	int lu;
	FILE *fd;
	char ch_txt1[150],ch_ip[20];
	char *p1,*p2;

	if (l_mode == 1)		//CDMA
	{
		if (pp_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块已经联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先断网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块没有上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先上电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"CDMA模块开始联网");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");

		sleep(1);
		system("/mnt/a/mc2006/driverscript/mobile_pppon");
		sleep(12);

		lu = get_ppp();
		if (lu == 1)
		{
			printf("\r\naaaa1\r\n");
			system("ifconfig ppp0 > /mnt/a/userapp/ppp0.txt");
			sleep(1);

			if((fd=fopen("/mnt/a/userapp/ppp0.txt","r+"))==NULL) 
			{
				fclose(fd);
				return;
			}
			while(!feof(fd))
			{
				memset(ch_txt1,0,150);
				fgets(ch_txt1,128,fd);
				ch_txt1[strlen(ch_txt1)] = '\0';
				if (strstr(ch_txt1,"inet addr:")!=NULL)
				{
					p1=strstr(ch_txt1,"inet addr:");
					p1=p1+10;

					p2=strchr(p1,' ');
					*p2=0;
					break;
				}
			}
			fclose(fd);

			TERM_clearscreen();
			TERM_displayxy(0,4,NORMALDISPLAY,"CDMA连接成功");	
			TERM_displayxy(0,3,NORMALDISPLAY,"当前网络IP为:");		
			TERM_displayxy(0,4,NORMALDISPLAY,p1);
			pp_on = 1;
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
			printf("\r\naaaa2\r\n");
			system("/mnt/a/mc2006/driverscript/mobile_pppoff");
			sleep(1);
			TERM_clearscreen();
			TERM_displayxy(0,4,NORMALDISPLAY,"CDMA连接失败");
			pp_on = 0;
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
	else
	{
		if (pp_on == 1)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块已经联网");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先断网...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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

		if (p_on == 0)
		{
			TERM_clearscreen();
			TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块没有上电");
			TERM_displayxy(0,2,NORMALDISPLAY,"请先上电...");
			TERM_displayxy(0,4,NORMALDISPLAY,"按任意键返回");
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
		
		TERM_clearscreen();
		TERM_displayxy(0,1,NORMALDISPLAY,"GPRS模块开始联网");
		TERM_displayxy(0,2,NORMALDISPLAY,"请等待...");

		sleep(1);
		system("/mnt/a/mc2006/driverscript/mobile_pppon");
		sleep(12);

		lu = get_ppp();
		if (lu == 1)
		{
			printf("\r\naaaa1\r\n");
			system("ifconfig ppp0 > /mnt/a/userapp/ppp0.txt");
			sleep(1);

			if((fd=fopen("/mnt/a/userapp/ppp0.txt","r+"))==NULL) 
			{
				fclose(fd);
				return;
			}
			while(!feof(fd))
			{
				memset(ch_txt1,0,150);
				fgets(ch_txt1,128,fd);
				ch_txt1[strlen(ch_txt1)] = '\0';
				if (strstr(ch_txt1,"inet addr:")!=NULL)
				{
					p1=strstr(ch_txt1,"inet addr:");
					p1=p1+10;

					p2=strchr(p1,' ');
					*p2=0;
					break;
				}
			}
			fclose(fd);

			TERM_clearscreen();
			TERM_displayxy(0,4,NORMALDISPLAY,"GPRS连接成功");	
			TERM_displayxy(0,3,NORMALDISPLAY,"当前网络IP为:");		
			TERM_displayxy(0,4,NORMALDISPLAY,p1);
			pp_on = 1;
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
			printf("\r\naaaa2\r\n");
			system("/mnt/a/mc2006/driverscript/mobile_pppoff");
			sleep(1);
			TERM_clearscreen();
			TERM_displayxy(0,4,NORMALDISPLAY,"GPRS连接失败");
			pp_on = 0;
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

	return;
}

void call_phone_down()
{
	char buf[1024];
	
	tcgetattr(0,&savedtio);
	
	fd232 = open(RS232_DEVICE0, O_RDWR | O_NOCTTY ); 
	if (fd232 <0) 
	{
		perror(RS232_DEVICE0);      
		tcsetattr(0,TCSANOW,&savedtio);
		return;
	}
	
	set_serial_raw_1(fd232, 1);
	
	if (l_mode == 1)
	{
		SendCommand_1(fd232,"AT+CHV\r",buf);
	}
	else
	{
		SendCommand_1(fd232,"ATH\r",buf);
	}
	
	if  (strstr(buf,"OK"))
	{
		printf("\r\naaaa\r\n");
		printf(buf);		
		return;
	}
	return;
}