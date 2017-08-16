#include "mc2005_icc.h"

#define BARCODE_DEVICE "/dev/s3c2410_serial1"
#define ARRAY_SIZE(a) (sizeof(a))/(sizeof(a[0]))
int fd232;
#define __log printf
#define BAUDRATE1 B9600
char upda_buf[] = {0x00,0x0A,0x80,0x04,0x00,0x00,0x06,0x20,0x00,0xFF,0xFF,0xFD,0x4E};
char upda_buf1[] = {0x00,0x0A,0x80,0x04,0x00,0x00,0x06,0x02,0x00,0x4E,0x24,0xFE,0xF8};

int mount_time=0;
int mount_error=0;

typedef union 
{
	unsigned char buff[2];
	int byte_2_num;
}type_2num;

int sub_main_menulabel_7()
{
	unsigned char *sys_menu[]={"1:扫码测试","2:版本测试"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,2))
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
}

int sub_main_menulabel_8()
{
	unsigned char *sys_menu[]={"1:单次扫码测试","2:10次扫码测试"};	
	
	while(1)
	{   
		switch(my_menu_menulabel_1(" ",sys_menu,2))
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
	
	system("setio 8 8");// 8 8 ′??? 9600 
	fd232 = open(BARCODE_DEVICE, O_RDWR | O_NOCTTY); 

	if (fd232 <0) 
	{
		__log("Poweron is FAIL\n");
		return -1;
	}
  
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE1 | CS8 | CLOCAL | CREAD ;
	
	tcflush(fd232, TCIFLUSH);
	tcsetattr(fd232,TCSANOW,&newtio);
  __log("Poweron is Success\n");

RESTART:
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按确认键检测");
	TERM_gotoxy(0,2);
	TERM_display(0,"按返回退出");
	
	

	while(1)
	{
		keych = TERM_getkey(KEEPWAITING);
		__log("__%x__\n",keych);
		if(keych == 0x0D)
		{
			__log("send buf :");
			for(i = 0;i<ARRAY_SIZE(upda_buf);i++)
			{
				__log("%02x ",upda_buf[i]);
			}
			__log("\n");
			
			write(fd232, upda_buf,ARRAY_SIZE(upda_buf));
			sleep(1);
			tcflush(fd232,TCIFLUSH);//清空缓冲区 不需要第一个返回
			
			__log("send buf1 :");
			for(i = 0;i<ARRAY_SIZE(upda_buf1);i++)//?o?12+1 **
			{
				__log("%02x ",upda_buf1[i]);
			}
			__log("\n");
			
			write(fd232,upda_buf1,ARRAY_SIZE(upda_buf1));
			sleep(1);
			count = read(fd232, rev_buf, 50); 
			__log("read BUF1\n");
			for(i = 0;i < count;i++)
			{
				__log("%02x ",rev_buf[i]);
			}
			__log("\n");
			
			memcpy(version,rev_buf + 16,8);			
			version[9] = "\0";
			__log("version == %s__\n",version);
			
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
			close(fd232);
			TERM_clearscreen();
			return -1;
		}
	}
}

int Read_samcard_test(int flag)
{
	FILE *fd;
	int ret;
	char ch_txt[200];
	unsigned char abuf[64], len;
	char buff1[30],buff2[30];
  int i,j;
  int time_test=0;
  int succ_flag=0;
  int time_ok=0,time_error=0;
  int k;
 
  type_2num cc;
  type_2num dd;
  
  time_test=flag;
  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"请确认已插入SAM卡");
	TERM_gotoxy(0,2);
	TERM_display(0,"任意键开始测试");

	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
			break;
		  } 
	}  

TERM_clearscreen();		
for(k=0;k<time_test;k++)
{		
re_readcpucard:
	  succ_flag=0;	
			if (ICC_interface_power(ICC_POWER_ON))
			{
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"SAM卡初始化失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			puts("ICC_interface_power OK");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(1)) != 0)
			{
       usleep(200);
			}
			else
			{
				printf("ICC_select_sock OK\n");
				usleep(10000);
				
				for(i=0;i<10;i++)
				{
				if ((ret = ICC_cpucard_ATR(abuf, &len,ICC_CLOCK_4M,ICC_VOLTAGE_5,0x11,ICC_CPUC_ATR_FLAG_EMV)) != 0) 
				{					
					usleep(100);
				}
				else 
				{
					memset(ch_txt,0,200);
					for (ret = 0; ret < len; ret++)
						printf("%02X ", abuf[ret]);
					if ( EMV_ISO_PBOC_atr_check( abuf, len, 0) )
						puts("is EMV_ISO_PBOC card");
					else
						puts("NONE ISO card");
					{
						typ_cpucard_param tcp;
						ICC_cpucard_get_param(&tcp);
						sprintf(ch_txt,"\natr ok:\n isT1:%d\nisConverted:%d\nVccOn:%d\nVolIndex:%d\n"
						"ClkIndex:%d\nisSyncCard:%d\nPSC32th:%d\nCKU:%d\n"
						"Fi:%d\nDi:%d\nD:%d\nDividor:%d\nPI1:%d\nII:%d\n"
						"N:%d\nPI2:%d\nWI:%d\nIFSI:%d\nCWI:%d\nBWI:%d\n"
						"BWT:%d\nCWT:%d\nWWT:%d\n",
						tcp.isT1, tcp.isConverted, tcp.VccOn, tcp.VolIndex,
						tcp.ClkIndex, tcp.isSyncCard, tcp.PSC32th, tcp.CKU,
						tcp.Fi, tcp.Di, tcp.D, tcp.Dividor, tcp.PI1, tcp.II,
						tcp.N, tcp.PI2, tcp.WI, tcp.IFSI, tcp.CWI, tcp.BWI,
						tcp.BWT, tcp.CWT, tcp.WWT);
						printf(ch_txt);
					}
					time_ok++;
					memset(buff1,0,30);
					memset(buff2,0,30);
          sprintf(buff1,"SAM卡成功%d次",time_ok);
          sprintf(buff2,"SAM卡失败%d次",time_error);
					TERM_gotoxy(0,1);
					TERM_display(0,buff1);
					TERM_gotoxy(0,2);
					TERM_display(0,buff2);
					ICC_interface_power(ICC_POWER_OFF);
					usleep(100000);
					succ_flag=1;
					break;				
				}
			}
			    if(succ_flag==0)
			    	{
			    time_error++;
					memset(buff1,0,30);
					memset(buff2,0,30);
          sprintf(buff1,"SAM卡成功%d次",time_ok);
          sprintf(buff2,"SAM卡失败%d次",time_error);
					TERM_gotoxy(0,1);
					TERM_display(0,buff1);
					TERM_gotoxy(0,2);
					TERM_display(0,buff2);
					ICC_interface_power(ICC_POWER_OFF);
		      } 
			}
    }	
  	usleep(100000);
}
  if(flag==50)
  {	
  dd.byte_2_num=k;
  cc.byte_2_num=time_ok;
  fd=fopen("/mnt/a/userapp/record","ab+"); 
  usleep(200);
  fwrite(dd.buff, 2, 1,fd);
  usleep(200);
  fwrite(cc.buff, 2, 1,fd); 
  usleep(200);
  fwrite("\n", 1, 1,fd);
  fclose(fd);   
}
  TERM_gotoxy(0,3);
	TERM_display(0,"按任意键进行TF卡测试");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				return 0;
		  }  
  }
}


int samcard_test_once()
{
	int ret;
	char ch_txt[200];
	unsigned char abuf[64], len;
	char buff1[30],buff2[30];
  int i,j;
  int time_test=0;
  int succ_flag=0;
  int time_ok=0,time_error=0;
  int k;
  
re_readcpucard:
  TERM_clearscreen();
  TERM_gotoxy(0,1);
	TERM_display(0,"正在测试SAM卡");				
	  succ_flag=0;	
			if (ICC_interface_power(ICC_POWER_ON))
			{
				printf("ICC_interface_power error\n.");
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"SAM卡初始化失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			printf("ICC_interface_power ok \n");
			
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(1)) != 0)
			{
       usleep(200);
			}
			else
			{
				printf("ICC_select_sock 1 OK\n");
				usleep(10000);
				
				for(i=0;i<10;i++)
				{
				if ((ret = ICC_cpucard_ATR(abuf, &len,ICC_CLOCK_4M,ICC_VOLTAGE_5,0x11,ICC_CPUC_ATR_FLAG_EMV)) != 0) 
				{	
					printf("%08X\n",ret);				
					usleep(100);
				}
				else 
				{
					memset(ch_txt,0,200);
					for (ret = 0; ret < len; ret++)
						printf("%02X ", abuf[ret]);
					if ( EMV_ISO_PBOC_atr_check( abuf, len, 0) )
						puts("is EMV_ISO_PBOC card");
					else
						puts("NONE ISO card");
					{
						typ_cpucard_param tcp;
						ICC_cpucard_get_param(&tcp);
						sprintf(ch_txt,"\natr ok:\n isT1:%d\nisConverted:%d\nVccOn:%d\nVolIndex:%d\n"
						"ClkIndex:%d\nisSyncCard:%d\nPSC32th:%d\nCKU:%d\n"
						"Fi:%d\nDi:%d\nD:%d\nDividor:%d\nPI1:%d\nII:%d\n"
						"N:%d\nPI2:%d\nWI:%d\nIFSI:%d\nCWI:%d\nBWI:%d\n"
						"BWT:%d\nCWT:%d\nWWT:%d\n",
						tcp.isT1, tcp.isConverted, tcp.VccOn, tcp.VolIndex,
						tcp.ClkIndex, tcp.isSyncCard, tcp.PSC32th, tcp.CKU,
						tcp.Fi, tcp.Di, tcp.D, tcp.Dividor, tcp.PI1, tcp.II,
						tcp.N, tcp.PI2, tcp.WI, tcp.IFSI, tcp.CWI, tcp.BWI,
						tcp.BWT, tcp.CWT, tcp.WWT);
						printf(ch_txt);
					}
					time_ok++;
					memset(buff1,0,30);
					memset(buff2,0,30);
          sprintf(buff1,"SAM卡成功%d次",time_ok);
          sprintf(buff2,"SAM卡失败%d次",time_error);
					TERM_gotoxy(0,1);
					TERM_display(0,"SAM卡测试成功");
					TERM_gotoxy(0,2);
					TERM_display(0,"准备测试TF卡");
					TERM_gotoxy(0,3);
					TERM_display(0,"按任意键进行");					
					ICC_interface_power(ICC_POWER_OFF);
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            return 0;
		             }
	             }		
				}
			}
         
		      } 
			}
			
								ICC_interface_power(ICC_POWER_OFF);
					TERM_gotoxy(0,1);
					TERM_display(0,"SAM卡测试失败");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_readcpucard;
		             }
	             }


}


int Read_cpucard_test(int flag)
{
	FILE *fd;
	int ret;
	char ch_txt[200];
	unsigned char abuf[64], len;
	char buff1[30],buff2[30];
  int i,j;
  int time_test=0;
  int succ_flag=0;
  int time_ok=0,time_error=0;
  int k;
  
  type_2num cc;
  type_2num dd;
  
  time_test=flag;
  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"请插入大CPU卡");
	TERM_gotoxy(0,2);
	TERM_display(0,"任意键开始测试");

	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
			break;
		  } 
		}  
TERM_clearscreen();		
for(k=0;k<time_test;k++)
{		
re_readcpucard:
	  succ_flag=0;	
			if (ICC_interface_power(ICC_POWER_ON))
			{
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"CPU卡上电失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			puts("ICC_interface_power OK");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(0)) != 0)
			{
       usleep(200);
			}
			else
			{
				printf("select_sock OK\n");
				usleep(1000);
				for(i=0;i<10;i++)
				{
				if ((ret = ICC_cpucard_ATR(abuf, &len,ICC_CLOCK_4M,ICC_VOLTAGE_5,0x11,ICC_CPUC_ATR_FLAG_EMV)) != 0) 
				{	
					memset(ch_txt,0,200);
				  sprintf(ch_txt,"error-\n%08X\n", ret);
				  ch_txt[strlen(ch_txt)] = '\0';
					puts(ch_txt);					
					usleep(100);
				}
				else 
				{
					memset(ch_txt,0,200);
					for (ret = 0; ret < len; ret++)
						printf("%02X ", abuf[ret]);
					if ( EMV_ISO_PBOC_atr_check( abuf, len, 0) )
						puts("is EMV_ISO_PBOC card");
					else
						puts("NONE ISO card");
					{
						typ_cpucard_param tcp;
						ICC_cpucard_get_param(&tcp);
						sprintf(ch_txt,"\natr ok:\n isT1:%d\nisConverted:%d\nVccOn:%d\nVolIndex:%d\n"
						"ClkIndex:%d\nisSyncCard:%d\nPSC32th:%d\nCKU:%d\n"
						"Fi:%d\nDi:%d\nD:%d\nDividor:%d\nPI1:%d\nII:%d\n"
						"N:%d\nPI2:%d\nWI:%d\nIFSI:%d\nCWI:%d\nBWI:%d\n"
						"BWT:%d\nCWT:%d\nWWT:%d\n",
						tcp.isT1, tcp.isConverted, tcp.VccOn, tcp.VolIndex,
						tcp.ClkIndex, tcp.isSyncCard, tcp.PSC32th, tcp.CKU,
						tcp.Fi, tcp.Di, tcp.D, tcp.Dividor, tcp.PI1, tcp.II,
						tcp.N, tcp.PI2, tcp.WI, tcp.IFSI, tcp.CWI, tcp.BWI,
						tcp.BWT, tcp.CWT, tcp.WWT);
						printf(ch_txt);
					}
					time_ok++;
					memset(buff1,0,30);
					memset(buff2,0,30);
          sprintf(buff1,"cpu卡成功%d次",time_ok);
          sprintf(buff2,"cpu卡失败%d次",time_error);
					TERM_gotoxy(0,1);
					TERM_display(0,buff1);
					TERM_gotoxy(0,2);
					TERM_display(0,buff2);
					ICC_interface_power(ICC_POWER_OFF);
					usleep(10000);
					succ_flag=1;
					break;				
				}
			}
			    if(succ_flag==0)
			    	{
			    time_error++;
					memset(buff1,0,30);
					memset(buff2,0,30);
          sprintf(buff1,"cpu卡成功%d次",time_ok);
          sprintf(buff2,"cpu卡失败%d次",time_error);
					TERM_gotoxy(0,1);
					TERM_display(0,buff1);
					TERM_gotoxy(0,2);
					TERM_display(0,buff2);
					ICC_interface_power(ICC_POWER_OFF);
		      } 
			}
			usleep(100000);
    }	
  
}
  if(flag==50)
  {	
  dd.byte_2_num=k;
  cc.byte_2_num=time_ok;
  fd=fopen("/mnt/a/userapp/record","ab+"); 
  usleep(200);
  fwrite(dd.buff, 2, 1,fd);
  usleep(200);
  fwrite(cc.buff, 2, 1,fd); 
  usleep(200);
  fwrite("\n", 1, 1,fd);
  fclose(fd);   
}
  TERM_gotoxy(0,3);
	TERM_display(0,"按任意键进行SAM卡测试");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				return 0;
		  }  
  }
}


int cpucard_test_once()
{
	int ret;
	char ch_txt[200];
	unsigned char abuf[64], len;
	char buff1[30],buff2[30];
  int i,j;
  int time_test=0;
  int succ_flag=0;
  int time_ok=0,time_error=0;
  int k;
	
	
	  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"请插入大CPU卡");
	TERM_gotoxy(0,2);
	TERM_display(0,"任意键开始测试");

	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
			break;
		  } 
		} 
		
re_readcpucard:
	  succ_flag=0;	
			if (ICC_interface_power(ICC_POWER_ON))
			{
				printf("ICC_interface_power error\n.");
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"CPU卡上电失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			printf("ICC_interface_power ok\n.");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(0)) != 0)
			{
       usleep(200);
       printf("ICC_select_sock 0 error\n");
			}
			else
			{
				printf("ICC_select_sock 0 OK\n");
				usleep(10000);
				for(i=0;i<10;i++)
				{
				if ((ret = ICC_cpucard_ATR(abuf, &len,ICC_CLOCK_4M,ICC_VOLTAGE_5,0x11,ICC_CPUC_ATR_FLAG_EMV)) != 0) 
				{					
					usleep(100);
					memset(ch_txt,0,200);
					sprintf(ch_txt,"%08X", ret);
					ch_txt[strlen(ch_txt)] = '\0';
					puts(ch_txt);
				}
				else 
				{
					memset(ch_txt,0,200);
					for (ret = 0; ret < len; ret++)
						printf("%02X ", abuf[ret]);
					if ( EMV_ISO_PBOC_atr_check( abuf, len, 0) )
						puts("is EMV_ISO_PBOC card");
					else
						puts("NONE ISO card");
					{
						typ_cpucard_param tcp;
						ICC_cpucard_get_param(&tcp);
						sprintf(ch_txt,"\natr ok:\n isT1:%d\nisConverted:%d\nVccOn:%d\nVolIndex:%d\n"
						"ClkIndex:%d\nisSyncCard:%d\nPSC32th:%d\nCKU:%d\n"
						"Fi:%d\nDi:%d\nD:%d\nDividor:%d\nPI1:%d\nII:%d\n"
						"N:%d\nPI2:%d\nWI:%d\nIFSI:%d\nCWI:%d\nBWI:%d\n"
						"BWT:%d\nCWT:%d\nWWT:%d\n",
						tcp.isT1, tcp.isConverted, tcp.VccOn, tcp.VolIndex,
						tcp.ClkIndex, tcp.isSyncCard, tcp.PSC32th, tcp.CKU,
						tcp.Fi, tcp.Di, tcp.D, tcp.Dividor, tcp.PI1, tcp.II,
						tcp.N, tcp.PI2, tcp.WI, tcp.IFSI, tcp.CWI, tcp.BWI,
						tcp.BWT, tcp.CWT, tcp.WWT);
						printf(ch_txt);
					}

					TERM_gotoxy(0,1);
					TERM_display(0,"CPU卡测试成功");
					TERM_gotoxy(0,2);
					TERM_display(0,"进入SAM卡测试");					
					ICC_interface_power(ICC_POWER_OFF);
					TERM_gotoxy(0,3);
					TERM_display(0,"按任意键进行");					
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            return 0;
		             }
	             }				
				}
			}
								ICC_interface_power(ICC_POWER_OFF);
					TERM_gotoxy(0,1);
					TERM_display(0,"CPU卡测试失败");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto re_readcpucard;
		             }
	             }
		     
			}
    }	
 
}


int test_switch_cup()//检测到位开关
{
	int ret;
	char ch_txt[200];
	unsigned char abuf[64], len;
	char buff1[30],buff2[30];
  int i,j;
  int time_test=0;
  int succ_flag=0;
  int time_ok=0,time_error=0;
  int k;
	
Menu:
				TERM_clearscreen();
				TERM_gotoxy(0,1);
				TERM_display(0,"检测卡片到位开关");					
				TERM_gotoxy(0,2);
				TERM_display(0,"需将CPU大卡拔出!");				
				TERM_gotoxy(0,3);
				TERM_display(0,"请确认CPU卡已拔出");
				TERM_gotoxy(0,4);
				TERM_display(0,"按任意键开始测试");			
	while(1)
	{
		key =TERM_getkey(KEEPWAITING);
		if (key > 0)
		{			
			break;
		} 
	}
					
re_readcpucard:
	  succ_flag=0;	
			if (ICC_interface_power(ICC_POWER_ON))
			{
				printf("ICC_interface_power error\n.");
				TERM_clearscreen();
				TERM_gotoxy(0,4);
				TERM_display(0,"CPU卡上电失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			printf("ICC_interface_power ok\n.");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(0)) != 0)
			{
       usleep(200);
       printf("ICC_select_sock 0 error\n");
			}
			else
			{
				printf("ICC_select_sock 0 OK\n");
				usleep(10000);
				for(i=0;i<10;i++)
				{
				if ((ret = ICC_cpucard_ATR(abuf, &len,ICC_CLOCK_4M,ICC_VOLTAGE_5,0x11,ICC_CPUC_ATR_FLAG_EMV)) != 0) 
				{					
					usleep(100);
					memset(ch_txt,0,200);
					sprintf(ch_txt,"%08X", ret);
					ch_txt[strlen(ch_txt)] = '\0';
					puts(ch_txt);
					if(memcmp(ch_txt,"FFFFFE01",8)==0)
						{
							
						TERM_clearscreen();
				    TERM_gotoxy(0,1);
				    TERM_display(0,"到位开关检测成功");					
				    TERM_gotoxy(0,2);
				    TERM_display(0,"准备CPU卡测试");	
				    TERM_gotoxy(0,3);
				    TERM_display(0,"按任意键进入");					    
				    ICC_interface_power(ICC_POWER_OFF);
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
				 		ICC_interface_power(ICC_POWER_OFF);
				 		TERM_clearscreen();
				    TERM_gotoxy(0,1);
				    TERM_display(0,"到位开关检测失败");					
				    sleep(2);
				    goto Menu;
				 		}			
				}
				else 
				{
					ICC_interface_power(ICC_POWER_OFF);
				 		TERM_clearscreen();
				    TERM_gotoxy(0,1);
				    TERM_display(0,"到位开关检测失败");
				    TERM_gotoxy(0,2);
				    TERM_display(0,"检测到CPU卡已插入");				    					
				    sleep(2);
				    goto Menu;					
					}
			}
			 ICC_interface_power(ICC_POWER_OFF);
			 TERM_gotoxy(0,1);
			 TERM_display(0,"CPU卡测试失败");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
		   	while(1)
        {
         		key =TERM_getkey(KEEPWAITING);
        		if (key > 0)
      		  {
	            goto re_readcpucard;
            }
        }						     
			}
    }	
}


int admin_check()
{
	int mPtr,mPtr_1,mPtr_2,mPtr_3,mPtr_4;
  int i;
  
	TERM_clearscreen();
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
			Presskey_test(1);
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
			Beep_test();
			goto sub_main_menulabel_1_label;
		}
	}
	else if (mPtr == 0)
		return 0;
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
			test_switch_cup();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 2)
		{
			Read_Ticard_test();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 3)
		{
			cpucard_test_once();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 4)
		{
			samcard_test_once();
			goto sub_main_menulabel_2_label;
		}
		else if (mPtr_1 == 5)
		{	
	    	mmc_load();								
			mmc_load_ext2();
			  
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
		else if (mPtr_1 == 6)
		{
			Read_Icode2card_test();
			goto sub_main_menulabel_2_label;
		}
	}
	else if (mPtr == 3)
	{//11111
		mPtr_1 = sub_main_menulabel_7();
		if(mPtr_1 == 1)
		{
			Barcode_poweron();
			mPtr_2 = sub_main_menulabel_8();
			if(mPtr_2 == 1)
			{
				Scan_barcode_1_W();
			}
			else if(mPtr_2 == 2)
			{
				Scan_barcode_loop_W();
			}
		}
		else if (mPtr_1 == 2) 
		{
			barcod_version();
		}
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