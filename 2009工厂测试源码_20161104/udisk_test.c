void Udisk_kzw_test_close()
{
	TERM_clearscreen();
	TERM_gotoxy(0,1);
	TERM_display(0,"���ڹر�USB...");
	system("/mnt/a/mc2006/driverscript/udisk_stop");
	sleep(1);
	TERM_gotoxy(0,4);
	TERM_display(0,"USB�رճɹ�!");
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
	TERM_display(0,"���ڼ���USB����");
	TERM_gotoxy(0,4);
	TERM_display(0,"���Ժ�...");

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
		TERM_display(0,"����ʧ��,�����¼���");
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
		TERM_display(0,"����ʧ��,�����¼���");
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
	TERM_display(0,"����U�̳ɹ�!");
	
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
	TERM_display(0,"���ڹر�����...");
	system("/mnt/a/mc2006/driverscript/usbh2eth_down");
	sleep(1);
	TERM_gotoxy(0,4);
	TERM_display(0,"����رճɹ�!");
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