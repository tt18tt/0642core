#include "libdriver2006.h"
#include "libtty2006.h"

int HIGH_TEST_MODE=0;
int CIRCLE_MODE=0;
void Voltage_test()
{
	int keyread,ln=0;
	unsigned long mv;
	float v;
	char ch_vol[50],ch_ln[10];

	TERM_clearscreen();
	TERM_displayxy(3, 1, NORMALDISPLAY, "电池电量测试");
	TERM_displayxy(2, 2, NORMALDISPLAY, "每5秒更新一次");

	getBatVoltage(1, &mv);
	v=mv/1000.0;

	memset(ch_vol,0,50);
	sprintf(ch_vol,"当前电压:%.2fV",v);
	ch_vol[strlen(ch_vol)] = '\0';
	TERM_displayxy(2, 4, NORMALDISPLAY, ch_vol);

	do 
	{
		ln++;
		memset(ch_ln,0,10);
		sprintf(ch_ln,"--%d--",ln);
		ch_ln[strlen(ch_ln)] = '\0';
		TERM_displayxy(6, 3, NORMALDISPLAY, ch_ln);
		if (ln == 5)
		{
			getBatVoltage(1, &mv);
			v=mv/1000.0;
			memset(ch_vol,0,50);
			sprintf(ch_vol,"当前电压:%.2fV",v);
			ch_vol[strlen(ch_vol)] = '\0';
			TERM_displayxy(2, 4, NORMALDISPLAY, ch_vol);
			ln = 0;
			printf("\ngetBatVoltage\n");
		}
		sleep(1);
		
		keyread = TERM_getkey(3);
		if(keyread==KEY_ESC)      //esc
		{
			return;
		}
	}
	while (1);

	return;
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

int read_rtc_time()
{
	//char year[2],month[2],day[2],hour[2],minute[2],second[2];
	int key;
	char time[30];
	char show_buf1[10];
  char show_buf2[5];
  char year[4];
  char year_num;

restar:  
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"当前时间为:");
		
	gettime(time);
	
	memcpy(show_buf1,time,4);
	memcpy(show_buf1+4,"-",1);
	memcpy(show_buf1+5,time+4,2);
	memcpy(show_buf1+7,"-",1);
	memcpy(show_buf1+8,time+6,2);
  show_buf1[10]='\0';

	memcpy(show_buf2,time+8,2);
	memcpy(show_buf2+2,":",1);
	memcpy(show_buf2+3,time+10,2);
  show_buf2[5]='\0';
	
  TERM_gotoxy(0,2);
	TERM_display(0,show_buf1);
	TERM_gotoxy(12,2);
	TERM_display(0,show_buf2);
	TERM_displayxy(0, 3, NORMALDISPLAY, "时间日期是否正确");	
		if(get_confirm_stat("正确","不正确")==1)
			{
		TERM_clearscreen();
		TERM_displayxy(3, 1, NORMALDISPLAY, "进入按键测试");
		return 0;
	   }
	  else
	  	{
	  		//system("setio 200000 0");
		   TERM_clearscreen();
		   TERM_displayxy(0, 1, NORMALDISPLAY, "RTC功能不正常");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            goto restar;
		             }
	             }
         		
	  		}	

		
	}

	int get_confirm_stat(char *p1,char *p2)
{
	int i=1;
	int j=0;
	int key=0;
	
	while(1)
	{
		TERM_gotoxy(0,4);
		TERM_display(i%2,p1);	
		TERM_gotoxy(10,4);
		TERM_display(j%2,p2);
		key=TERM_getkey(KEEPWAITING);
		if(key==KEY_DOWN)
			{
				i++;
				j++;
				}
		else if(key==KEY_ENTER)
			{
				return i%2;
				}								
	}
	}
	

int Backlight_test()
{
	int keyread,rValue;

	rValue = 1;

	while (1) 
	{
		TERM_clearscreen();
		TERM_displayxy(0, 1, NORMALDISPLAY, "正在进行背光测试");

		Run("sh","-c","/mnt/a/mc2006/driverscript/backlight_on",NULL);
		TERM_displayxy(0, 2, NORMALDISPLAY, "背光是否正常亮起");	
		TERM_displayxy(0, 3, NORMALDISPLAY, "请务必确认");	
		if(get_confirm_stat("正常","不正常")==1)
			{
		Run("sh","-c","/mnt/a/mc2006/driverscript/backlight_off",NULL);
		TERM_clearscreen();
		TERM_displayxy(3, 1, NORMALDISPLAY, "进入蜂鸣器测试");
		return 0;
	   }
	  else
	  	{
	  		Run("sh","-c","/mnt/a/mc2006/driverscript/backlight_off",NULL);
		   TERM_clearscreen();
		   TERM_displayxy(0, 1, NORMALDISPLAY, "背光功能不正常");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            break;
		             }
	             }  		
	  		} 
	}
}

void Beep_test()
{
	int keyread,rValue;


	while (1) 
	{
		TERM_clearscreen();
		TERM_displayxy(2, 1, NORMALDISPLAY, "蜂鸣器功能测试");
				sleep(1);
				Beep_AC(2000);
				//	system("setio 200000 200000");
			TERM_displayxy(0, 2, NORMALDISPLAY, "蜂鸣器是否正常");	
		TERM_displayxy(0, 3, NORMALDISPLAY, "请务必确认");	
		if(get_confirm_stat("正常","不正常")==1)
			{
		//system("setio 200000 0");
		TERM_clearscreen();
		TERM_displayxy(3, 1, NORMALDISPLAY, "进入CPU大卡测试");
		return 0;
	   }
	  else
	  	{
	  		//system("setio 200000 0");
		   TERM_clearscreen();
		   TERM_displayxy(0, 1, NORMALDISPLAY, "蜂鸣器功能不正常");
		   TERM_displayxy(0, 4, NORMALDISPLAY, "按任意键进行重测");
		   TERM_displayxy(0, 2, NORMALDISPLAY, "超过3次请联系质");
		   TERM_displayxy(0, 3, NORMALDISPLAY, "检员");
				   	while(1)
                 	{
	           	key =TERM_getkey(KEEPWAITING);
	          	if (key > 0)
          		   {
			            break;
		             }
	             } 		
	  		} 								

	}
}


int DateTime_Setting()
{	
	unsigned char sBuf[20], sTmp[20];
	int year,month,day,hour,minute,second=30,retCode=0;
	int key;
	
re_set_datetime:
	TERM_clearscreen();

	TERM_displayxy(1, 1, NORMALDISPLAY, "请输入当前日期:");
	TERM_displayxy(1, 2,	NORMALDISPLAY, "    /  /  ");

	memcpy(sBuf, "    /  /  ", 10);
	sBuf[10] = '\0';

	if (ReadNumeric_(sBuf, 10, 0, 2) < 0) return -1;

	memcpy(sTmp, sBuf, 4);
	sTmp[4] = '\0';
	year = atoi(sTmp);
	memcpy(sTmp, sBuf + 5, 2);
	sTmp[2] = '\0';
	month = atoi(sTmp);
	memcpy(sTmp, sBuf + 8, 2);
	sTmp[2] = '\0';
	day = atoi(sTmp);
	retCode=sscanf(sBuf,"%d/%d/%d",&year,&month,&day);

	if ((year<1900)||(month>12)||(day>31))
	{	
		TERM_clearscreen();
		TERM_displayxy(1,1,NORMALDISPLAY,"时钟输入错误..");
		TERM_displayxy(1,4,NORMALDISPLAY,"按任意键返回!");
		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				goto re_set_datetime;
			}
		}
	}


	TERM_displayxy(1, 3, NORMALDISPLAY, "请输入当前时间:");
	TERM_displayxy(1, 4, NORMALDISPLAY, "  :  ");

	memcpy(sBuf, "  :  ", 5);
	sBuf[5] = '\0';

	if (ReadNumeric_(sBuf, 5, 0, 4) < 0) return -1;
		
	memcpy(sTmp, sBuf, 2);
	sTmp[2] = '\0';
	hour = atoi(sTmp);
	memcpy(sTmp, sBuf + 3, 2);
	sTmp[2] = '\0';
	minute = atoi(sTmp);
	
	retCode=sscanf(sBuf,"%d:%d",&hour,&minute);

	TERM_clearscreen();
	if ((hour>24)||(minute>=60)||(second>=60))
	{	
		TERM_clearscreen();
		TERM_displayxy(1,1,NORMALDISPLAY,"时钟输入错误..");
		TERM_displayxy(1,4,NORMALDISPLAY,"按任意键返回!");
		while(1)
		{
			key =TERM_getkey(KEEPWAITING);
			if (key > 0)
			{
				goto re_set_datetime;
			}
		}
	}
	else
	{
		if ( TERM_setRTCandSYStime(year,month,day,hour,minute,second))
		{
			TERM_clearscreen();
			TERM_displayxy(1,1,NORMALDISPLAY,"时钟设置失败..");
			TERM_displayxy(1,4,NORMALDISPLAY,"按任意键返回!");
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					goto re_set_datetime;
				}
			}
		}
		TERM_displayxy(1,1,NORMALDISPLAY,"时钟设置成功!");
		TERM_displayxy(1,4,NORMALDISPLAY,"按任意键返回!");
	}

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

void Presskey_test(int flag)
{
	int key;
 int time=0;
 
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"请按提示输入按键");
	TERM_gotoxy(3,4);
	TERM_display(0,"按任意键开始");
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					break;
				}
			}
Again:			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【上】方向键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_UP)
				{
					break;
				}
			}	
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【下】方向键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_DOWN)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【左】方向键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_LEFT)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【右】方向键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_RIGHT)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【扫描】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_SCAN)
				{
					break;
				}
			}		
	
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【确认】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_ENTER)
				{
					break;
				}
			}			
	
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【清除】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_C)
				{
					break;
				}
			}
			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【返回】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_ESC)
				{
					break;
				}
			}
		
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【1】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x31)
				{
					break;
				}
			}				
			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【2】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x32)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【3】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x33)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【4】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x34)
				{
					break;
				}
			}

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【5】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x35)
				{
					break;
				}
			}
			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【6】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x36)
				{
					break;
				}
			}
	
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【7】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x37)
				{
					break;
				}
			}
		TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【8】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x38)
				{
					break;
				}
			}	
			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【9】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x39)
				{
					break;
				}
			}	
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【0】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == 0x30)
				{
					break;
				}
			}			

	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【F】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_F1)
				{
					break;
				}
			}
			
	TERM_clearscreen();
	TERM_gotoxy(0,2);
	TERM_display(0,"请按【S】键");	
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key == KEY_F2)
				{
					break;
				}
			}
	if(flag==2)
		{
			time++;
			if(time==1)
			goto Again;
			}																														
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按键功能测试完成");
	if(flag==2)
		{
	  TERM_gotoxy(0,2);
	  TERM_display(0,"按任意键进入到位");
    TERM_gotoxy(0,3);
	  TERM_display(0,"开关测试");			
			}
	else
		{
			if(HIGH_TEST_MODE==1)
				{
					TERM_gotoxy(0,2);
  	      TERM_display(0,"按任意键进入背光");
          TERM_gotoxy(0,3);
        	TERM_display(0,"功能设置测试");	
        	HIGH_TEST_MODE=0;
					}
		else
			{				
	      TERM_gotoxy(0,2);
  	    TERM_display(0,"按任意键进入时间");
        TERM_gotoxy(0,3);
      	TERM_display(0,"功能设置测试");
     }
    }
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					break;
				}
			}
			
}

