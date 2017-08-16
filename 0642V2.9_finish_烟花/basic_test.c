#include "libdriver2006.h"
#include "libtty2006.h"

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

void Backlight_test()
{
	int keyread,rValue;

	rValue = 1;

	while (1) 
	{
		TERM_clearscreen();
		TERM_displayxy(3, 1, NORMALDISPLAY, "背光功能测试");
		if (rValue > 0)
		{
			TERM_displayxy(4, 3, REVERSEDISPLAY, "打开背光");
			TERM_displayxy(4, 4, NORMALDISPLAY, "关闭背光");
			rValue = 1;
		}
		else
		{
			TERM_displayxy(4, 3, NORMALDISPLAY, "打开背光");
			TERM_displayxy(4, 4, REVERSEDISPLAY, "关闭背光");
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
					usleep(100000);
					Run("sh","-c","/mnt/a/mc2006/driverscript/backlight_on",NULL);
					rValue = -rValue;
				}
				else
				{
					printf("\r\nclose");
					usleep(100000);
					Run("sh","-c","/mnt/a/mc2006/driverscript/backlight_off",NULL);
					rValue = -rValue;
				}
				break;
			}
			else if (keyread == KEY_ESC)
				return;
			
		}while (1);
		if (keyread == KEY_ESC)
			return;
	}
}

void Beep_test()
{
	int keyread,rValue;

	rValue = 1;

	while (1) 
	{
		TERM_clearscreen();
		TERM_displayxy(2, 1, NORMALDISPLAY, "蜂鸣器功能测试");
		if (rValue > 0)
		{
			TERM_displayxy(4, 3, REVERSEDISPLAY, "蜂鸣器鸣响");
			TERM_displayxy(4, 4, NORMALDISPLAY, "蜂鸣器鸣停");
			rValue = 1;
		}
		else
		{
			TERM_displayxy(4, 3, NORMALDISPLAY, "蜂鸣器鸣响");
			TERM_displayxy(4, 4, REVERSEDISPLAY, "蜂鸣器鸣停");
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
				sleep(1);
				if (rValue > 0)
				{
					printf("\r\nopen");
					//sleep(1);
					usleep(100000);
					system("setio 200000 200000");
					rValue = -rValue;
				}
				else
				{
					printf("\r\nclose");
					//sleep(1);
					usleep(100000);
					system("setio 200000 0");
					rValue = -rValue;
				}
				break;
			}
			else if (keyread == KEY_ESC)
				return;
			
		}while (1);
		if (keyread == KEY_ESC)
			return;
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

void Presskey_test()
{
	int key;

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
																															
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"按键测试完成");
	TERM_gotoxy(3,4);
	TERM_display(0,"按任意键退出");
			while(1)
			{
				key =TERM_getkey(KEEPWAITING);
				if (key > 0)
				{
					break;
				}
			}
			
}