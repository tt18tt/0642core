void Udisk_kzw_test_close()
{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在关闭USB...");
	system("/mnt/a/mc2006/driverscript/udisk_stop");
	sleep(1);
	TERM_gotoxy(0,4);
	TERM_display(0,"USB关闭成功!");
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

void Udisk_kzw_test_open()
{
	FILE *fd;
	char ch_txt[300];
	int len;

	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在加载USB驱动");
	TERM_gotoxy(0,4);
	TERM_display(0,"请稍后...");

	system("/mnt/a/mc2006/driverscript/udisk_stop");
	sleep(2);
	system("/mnt/a/mc2006/driverscript/udisk_start 2> /mnt/a/userapp/usb_info.txt");

	memset(ch_txt,0,300);
	if((fd=fopen("/mnt/a/userapp/usb_info.txt","r+"))==NULL)
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
		return;
	}

	system("mount -t vfat /dev/sda1 /mnt/udisk 2> /mnt/a/userapp/usb_info.txt");
	sleep(1);

	memset(ch_txt,0,300);
	if((fd=fopen("/mnt/a/userapp/usb_info.txt","r+"))==NULL)
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
	TERM_display(0,"加载U盘成功!");
	
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

void Eth0_kzw_test_close()
{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"正在关闭网络...");
	system("/mnt/a/mc2006/driverscript/usbh2eth_down");
	sleep(1);
	TERM_gotoxy(0,4);
	TERM_display(0,"网络关闭成功!");
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