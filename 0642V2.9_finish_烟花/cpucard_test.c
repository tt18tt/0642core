#include "mc2005_icc.h"

#if 0
void Read_samcard_test()
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
  
Sam_menu:  
  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次SAM卡测试");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.10次SAM卡测试");
	TERM_gotoxy(0,3);
	TERM_display(0,"3.100次SAM卡测试");
	TERM_gotoxy(0,4);	
	TERM_display(0,"4.按返回键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key == 0x31)
			{
			time_test=1;
			break;
		  }
		else if(key == 0x32)
			{
				time_test=10;
				break;
				}	
		else if(key == 0x33)
			{
				time_test=100;
				break;
				}	
	  else if(key == KEY_ESC)
			{
				return 0;
				}							
		}
TERM_clearscreen();		
printf("time_test =========== %d\n",time_test);
	for(k=0;k<time_test;k++);
	{		
		re_readcpucard:
		  	succ_flag=0;
			printf("LINE== %d\n",__LINE__);
				if (ICC_interface_power(ICC_POWER_ON))
				{
					TERM_clearscreen();
					TERM_gotoxy(0,4);
					TERM_display(0,"SAM卡初始化失败!");
					Beep_AC(1000);
					goto re_readcpucard;
				}
				puts("mc2005 opened.");
	    for(j=0;j<10;j++)
	    {		
				if ((ret = ICC_select_sock(1)) != 0)
				{
	       			usleep(200);
				}
				else
				{
					printf("select_sock OK\n");
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
						printf("LINE== %d\n",__LINE__);
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
						printf("LINE== %d\n",__LINE__);
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
						printf("LINE== %d\n",__LINE__);
			      } 
				}
	    	
	  	usleep(100000);
		}
}
  time_ok=0;
  time_error=0;
  TERM_gotoxy(0,3);
	TERM_display(0,"按任意键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				goto Sam_menu;
		  }  
  }
}
#endif
void Read_samcard_test()
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
  
Sam_menu:  
  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次SAM卡测试");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.10次SAM卡测试");
	TERM_gotoxy(0,3);
	TERM_display(0,"3.100次SAM卡测试");
	TERM_gotoxy(0,4);	
	TERM_display(0,"4.按返回键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key == 0x31)
			{
			time_test=1;
			break;
		  }
		else if(key == 0x32)
			{
				time_test=10;
				break;
				}	
		else if(key == 0x33)
			{
				time_test=100;
				break;
				}	
	  else if(key == KEY_ESC)
			{
				return 0;
				}							
		}
TERM_clearscreen();		
printf("time_test =========== %d\n",time_test);
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
				puts("mc2005 opened.");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(1)) != 0)
			{
       usleep(200);
			}
			else
			{
//				TERM_clearscreen();
//				TERM_gotoxy(0,1);
//				TERM_display(0,"卡槽检测成功");
//				sleep(1);
				printf("select_sock OK\n");
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
			usleep(100000);
    }	
}
  time_ok=0;
  time_error=0;
  TERM_gotoxy(0,3);
	TERM_display(0,"按任意键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				goto Sam_menu;
		  }  
  }
}
void Read_cpucard_test()
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
  
Sam_menu:  
  TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"1.单次CPU卡测试");
	TERM_gotoxy(0,2);
	TERM_display(0,"2.10次CPU卡测试");
	TERM_gotoxy(0,3);
	TERM_display(0,"3.100次CPU卡测试");
	TERM_gotoxy(0,4);	
	TERM_display(0,"4.按返回键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key == 0x31)
			{
			time_test=1;
			break;
		  }
		else if(key == 0x32)
			{
				time_test=10;
				break;
				}	
		else if(key == 0x33)
			{
				time_test=100;
				break;
				}	
	  else if(key == KEY_ESC)
			{
				return 0;
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
				TERM_display(0,"CPU卡初始化失败!");
				Beep_AC(1000);
				goto re_readcpucard;
			}
			puts("mc2005 opened.");
    for(j=0;j<10;j++)
    {
			if ((ret = ICC_select_sock(0)) != 0)
			{
       usleep(200);
			}
			else
			{
//				TERM_clearscreen();
//				TERM_gotoxy(0,1);
//				TERM_display(0,"卡槽检测成功");
//				sleep(1);
				printf("select_sock OK\n");
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
          			sprintf(buff1,"cpu卡成功%d次",time_ok);
          			sprintf(buff2,"cpu卡失败%d次",time_error);
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
  time_ok=0;
  time_error=0;
  TERM_gotoxy(0,3);
	TERM_display(0,"按任意键返回");
	while(1)
	{
	key =TERM_getkey(KEEPWAITING);
		if (key >0)
			{
				goto Sam_menu;
		  }  
  }
}