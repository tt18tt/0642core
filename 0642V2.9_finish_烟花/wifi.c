#include "libdriver2006.h"
#include "libtty2006.h"

int key;
int ret;
int fd232;

int l_mmc=0;
unsigned char tstr[255],bcode[20];
char all[10];

char ip_address[20];
int ip_port;

int sockfd,nbytes,ret;
int l_mode=0,p_on=0,pp_on=0;

static struct termios savedtio;

int Link_wifi()
{
	FILE *fd;
	char ch_txt[150],ch_ip[20];
	char ch_info[500];
	int ln,li;
	int key;
	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在连接无线网...");

	//system("/mnt/a/userapp/sys_txt/wlanup_info");
  system("/mnt/a/mc2006/driverscript/wlanup > /mnt/a/userapp/sys_txt/wifilist.txt");
	memset(ch_info,0,500);
	strcpy(ch_info,"ifconfig mwlan0 > /mnt/a/userapp/mmc_info.txt");
	ch_info[strlen(ch_info)] = '\0';

	system(ch_info);

	if((fd=fopen("/mnt/a/userapp/mmc_info.txt","r+"))==NULL) 
	{
		fclose(fd);
		return -1;
	}
	memset(ch_txt,0,150);
	fgets(ch_txt,128,fd);
	ch_txt[strlen(ch_txt)] = '\0';
	fclose(fd);
	if (strstr(ch_txt,"mwlan0") == NULL)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"连接无线网失败!");

		Beep_AC(200);
		usleep(500);
		Beep_AC(200);
		system("/mnt/a/userapp/sys_txt/wlandown");

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return 0;
			}
		}
		return 0;
	}
	
	li = 0;
	memset(ch_txt,0,150);
	if((fd=fopen("/mnt/a/userapp/sys_txt/wifilist.txt","r+"))==NULL) 
	{
		fclose(fd);
		return -1;
	}
	while(!feof(fd))
	{
		memset(ch_txt,0,150);
		fgets(ch_txt,128,fd);
		ch_txt[strlen(ch_txt)] = '\0';
		if (strstr(ch_txt,"select for")!=NULL)
		{
			ln = strlen(ch_txt);
			memset(ch_ip,0,20);
			memcpy(ch_ip,ch_txt + 26,ln-26-4);
			ch_ip[strlen(ch_ip)] = '\0';
			printf(ch_ip);
			break;
		}
		else if (strstr(ch_txt,"Sending discover...")!=NULL)
		{
			li++;
			if (li==3)
			{
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"连接无线网失败!");

				Beep_AC(200);
				usleep(500);
				Beep_AC(200);
				system("/mnt/a/userapp/sys_txt/wlandown");

				while(1)
				{
					key =TERM_getkey(KEEPWAITING);
					if (key > 0)
					{
						return 0;
					}
				}
				return 0;
			}
		}
	}
	fclose(fd);

	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"当前自动获得IP为:");
	TERM_gotoxy(0,3);
	TERM_display(0,ch_ip);
	TERM_gotoxy(0,4);
	TERM_display(0,"无线网连接成功!");
	
	Beep_AC(1000);

	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{
			return 1;
		}
	}
	return 1;
}

int Change_wpa_essid()
{
	FILE *fp;
	int len,i,k,lk,lk1;
	char ch_essid[20],ch_txt[20],ch_b_essid[20];
	char ch_shell[100];
	int key;
	
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在寻找并配置网络...");
	
	memset(ch_essid,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/essid","a+"))==NULL)
	{
		fclose(fp);
		return -1;
	}
	fgets(ch_essid,20,fp);
	fclose(fp);
	ch_essid[strlen(ch_essid)-1] = '\0';

	len = strlen(ch_essid);
	if (len == 0)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,3);
		TERM_display(0,"系统中无无线设备名称");

		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				return 0;
			}
		}
		return 0;
	}
re_view_essid_before:
	
//	system("/mnt/a/userapp/sys_txt/view_essid_before");
//	sleep(1);

	memset(ch_txt,0,20);
	if((fp=fopen("/mnt/a/mc2006/etc/wpa-sup.conf","r"))==NULL)
	{
		fclose(fp);
		return -1;
	}
	fgets(ch_txt,20,fp);
	fclose(fp);
	ch_txt[strlen(ch_txt)] = '\0';
	len = strlen(ch_txt);

	if (len == 0)
	{
		printf("\r\nwpa-sup.conf  len=0\r\n");
		goto re_view_essid_before;
	}
	printf("\r\nwpa-sup.conf  len!=0\r\n");
	
	sleep(1);
	memset(ch_txt,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/wpa-essid.txt","r"))==NULL)
	{
		fclose(fp);
		return -1;
	}
	fgets(ch_txt,18,fp);
	fclose(fp);
	ch_txt[strlen(ch_txt)] = '\0';
	len = strlen(ch_txt);

	k=0;
	lk=0;
	lk1=0;

	for (i=0;i<len;i++)
	{
		if(ch_txt[i]=='"')
		{
			if(k == 0)
			{
				k++;
				lk = i;
			}
			else if (k == 1)
			{
				k++;
				lk1 = i;
			}
		}
	}
	memset(ch_b_essid,0,20);
	memcpy(ch_b_essid,ch_txt + lk + 1,lk1-lk-1);
	ch_b_essid[strlen(ch_b_essid)] = '\0';

	printf("\r\naaaaa\r\n");
	printf(ch_b_essid);
	printf("bbbbb\r\n");
	printf(ch_essid);
	printf("bbbbb\r\n");
	
	memset(ch_shell,0,100);
	strcpy(ch_shell,"cat /mnt/a/mc2006/etc/wpa-sup.conf | sed 's/");
	strcat(ch_shell,ch_b_essid);
	strcat(ch_shell,"/");
	strcat(ch_shell,ch_essid);
	strcat(ch_shell,"/' > /mnt/a/mc2006/etc/wpa-sup.conf");
	ch_shell[strlen(ch_shell)] = '\0';

	printf(ch_shell);
	printf("aaa\r\n");
	
	Run("sh","-c",ch_shell,NULL);
	
	return 1;
}

void Essid_Setting_term()
{
	unsigned char buf[16], Essid[16],i, j;
	unsigned int retCode;
	int len,key;
	FILE *fp,*stream;

	TERM_clearscreen();
	memset(Essid,0,16);
	if((fp=fopen("/mnt/a/userapp/sys_txt/essid","a+"))==NULL) 
	{
		fclose(fp);
		return;
	}
	fgets(Essid,16,fp);
	fclose(fp);
	Essid[strlen(Essid)] = '\0';
	len = strlen(Essid);
	if (len == 0)
	{
ESSID_AGAIN:
		TERM_clearscreen();
		memset(Essid,0,16);

		TERM_gotoxy(0,1);
		TERM_display(0,"请输入ESSID名:");
	
		if (ReadString_term(Essid,16,0,2,0) < 0) return;

		Essid[strlen(Essid)] = '\0';
		printf("\r\naa\r\n");
		printf(Essid);
		printf("aa\r\n");
		Run("rm","/mnt/a/userapp/sys_txt/essid",NULL,NULL);
	
		if((stream=fopen("/mnt/a/userapp/sys_txt/essid","at+"))==NULL)
		{
			fclose(stream);
			return;
		}
		fputs(Essid,stream);
		fsync(fileno(stream));
		rewind(stream);
		fclose(stream);

		TERM_gotoxy(0,4);
		TERM_display(0,"ESSID保存完毕");
		
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
		TERM_gotoxy(0,1);
		TERM_display(0,"无线设备名称:");
		TERM_gotoxy(0,2);
		TERM_display(0,Essid);
		
		retCode=TERM_getkey(KEEPWAITING);
		if(retCode!=KEY_ENTER)
		{
			return;	
		}
		else
		{
			goto ESSID_AGAIN;
		}
	}
}

void Send_wifi_info()
{
	int pd;
	char to_send[1024],ch_barcode[50],time_str[20];

	pd = get_sys_ip_wifi();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"无服务端IP!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		Run("sh","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);

		sleep(1);
		return;
	}
	
	pd = get_sys_com_wifi();
	if (pd == -1)
	{
		TERM_clearscreen();
		TERM_gotoxy(0,4);
		TERM_display(0,"无服务端端口!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		Run("sh","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);

		sleep(1);
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

		Run("sh","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);

		sleep(1);
		return;
	}

	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在发送数据到服务器...");
	
	memset(time_str,0,20);
	gettime(time_str);
	time_str[14] = '\0';

	memset(ch_barcode,0,50);
	strcpy(ch_barcode,"1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	ch_barcode[strlen(ch_barcode)] = '\0';
	
	memset(to_send,0,1024);
	memcpy(to_send,"&&",2);
	memcpy(to_send + 2,ch_barcode,strlen(ch_barcode));
	memcpy(to_send + 2 + strlen(ch_barcode),"|",1);
	memcpy(to_send + 3 + strlen(ch_barcode),time_str,14);
	memcpy(to_send + 17 + strlen(ch_barcode),"$$",2);
	to_send[strlen(to_send)] = '\0';

	pd = gprs_rec_send(to_send);
	if (pd == 1)
	{
		sleep(1);
		TERM_clearscreen();
		TERM_gotoxy(0,3);
		TERM_display(0,"无线WIFI发送成功!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		Run("sh","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);

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
	else if(pd == -1)
	{
		sleep(1);
		TERM_clearscreen();
		TERM_gotoxy(0,3);
		TERM_display(0,"无线WIFI发送失败!");
		Beep_AC(200);
		usleep(500);
		Beep_AC(200);

		Run("sh","-c","/mnt/a/userapp/sys_txt/wlandown",NULL);

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

int Send_wifi_menu()
{
	int keyread,rValue;

	rValue = 1;

	while (1) 
	{
		TERM_clearscreen();
		TERM_displayxy(2, 1, NORMALDISPLAY, "是否发送数据?");
		if (rValue > 0)
		{
			TERM_displayxy(6, 3, REVERSEDISPLAY, "发送");
			TERM_displayxy(6, 4, NORMALDISPLAY, "返回");
			rValue = 1;
		}
		else
		{
			TERM_displayxy(6, 3, NORMALDISPLAY, "发送");
			TERM_displayxy(6, 4, REVERSEDISPLAY, "返回");
			rValue = -1;
		}

		do 
		{
			keyread = TERM_getkey(KEEPWAITING);
			if (keyread == KEY_UP) 
			{
				rValue = -rValue;
				if (rValue > 0)
				{
					rValue = 1;
				}
				else
				{
					rValue = -1;
				}
				break;
			}
			else if (keyread == KEY_DOWN) 
			{
				rValue = -rValue;
				if (rValue > 0)
				{
					rValue = 1;
				}
				else
				{
					rValue = -1;
				}
				break;
			}
			else if (keyread == KEY_ENTER)
			{
				if (rValue > 0)
				{
					printf("\r\nopen");
					rValue = -rValue;
					return 1;
				}
				else
				{
					printf("\r\nclose");
					rValue = -rValue;
					return -1;
				}
			}
			else if (keyread == KEY_ESC)
				return 0;
			
		}while (1);
		if (keyread == KEY_ESC)
			return 0;
	}

	return 0;
}

int get_sys_com_wifi()
{
	FILE *fp;
	int len;
	char com_id[20];
	
	memset(com_id,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/com","a+"))==NULL) 
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

int get_sys_ip_wifi()
{
	FILE *fp;
	int len;
	
	memset(ip_address,0,20);
	if((fp=fopen("/mnt/a/userapp/sys_txt/ip","a+"))==NULL) 
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