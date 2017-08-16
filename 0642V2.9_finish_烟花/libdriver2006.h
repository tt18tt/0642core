#ifndef _LIBDRIVER2006
#define _LIBDRIVER2006

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include<unistd.h>
#include <stdarg.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <linux/rtc.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <time.h>

#include <sys/time.h>


#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define PORT_FILE    "/sys/class/MC2006/misc_io/miscio"
//extended io is valid for LXR0680 only!
#define EXTPORT_FILE "/sys/class/MC2006/misc_io/extendedio"

#define FALSE 0
#define TRUE 1

//global var for print debug info , declared in barcode.c .add in 08/06/2012 
extern unsigned int InDebuggingMode;

//
//Keyboard and backlight auto control For QT systems:
//
//send message to QT key monitor (sidetoolbar) to display or hide the software keyboard:
#define	openkb_upper()      { QCopChannel::send( "/tmp/channel", "openkb_upper"); }       
#define	openkb_lower()      { QCopChannel::send( "/tmp/channel", "openkb_lower"); }    
#define	openkb_sym ()       { QCopChannel::send( "/tmp/channel", "openkb_sym"); }  
#define openkb_py()         { QCopChannel::send( "/tmp/channel", "openkb_py"); } 
#define closekb()           { QCopChannel::send( "/tmp/channel", "closekb"); } 

//send message to QT key monitor (sidetoolbar) to enter or exit standby mode:
#define standby_enter()     { QCopChannel::send( "/tmp/channel", "standby_enter"); } 
#define	standby_exit()      { QCopChannel::send( "/tmp/channel", "standby_exit" ); }

//send message to QT key monitor (sidetoolbar) to set timeout to enter standby mode, PARAM is unsigned integer, 0-65535. unit is second. 0:never standby.
#define standby_settime(PARAM) { QByteArray  a(sizeof(unsigned short)); *((unsigned short*)a.data())= (PARAM); QCopChannel::send( "/tmp/channel", "standby_settime", a); }


//
//Keyboard and backlight auto control For Non-QT systems:
//
//send IPC signal to background key monitor (icon_km for LXR0642)
int settime_ELdelaysec(int sec);
int settime_AutoSleepsec(int sec);


//
//low level APIs to set system flag to enable/disable background program to enter Suspend or LCDoff mode:
//for all LXR06XX excepting the ESCAN20007 OEM model which uses console.a to handle power saving.
//
int disable_suspend();
int enable_suspend();
int disable_off_LCD();
int enable_off_LCD();

//high level APIs to se system sleep level:
//at NOSLEEP level, suspend and LCD off are all disabled:
#define set_sleep_level_NOSLEEP()     { disable_off_LCD(); disable_suspend(); }
//at LCDSLEEP or LOWSLEEP level, no suspend is allowed but LCD can be turned off:
#define set_sleep_level_LCDSLEEP()    { enable_off_LCD();  disable_suspend(); }
//at DEEPSLEEP level, suspend and LCD off are all allowed:
#define set_sleep_level_DEEPSLEEP()   { enable_off_LCD();  enable_suspend(); }


//
//IO, Time, MISC control for All LXR06X0:
//
int set_mio(unsigned long mask, unsigned long value);                //set io
int get_mio( unsigned long *value);                                  //get io  

//extended io is for LXR0680 ONLY:
int set_extmio(unsigned long mask, unsigned long value);         //set io
int get_extmio( unsigned long *value);                           //get io  

int readlinefromfile(char *filename, char *retstr);                  //readln from a file
int writelinetofile(char *filename, char *putstr);
 
//for DC type beeper (LXR0630, 0640):
void Beep(int ms);                                                   
//for AC type beeper (LXR0642):
void Beep_AC(int ms);                                                   

//API to turn on/off LCD backlight, valid input is 0:off, 1:on;
int backlight_power(int on_off);

//returns UID of NAND flash as handheld's ID
int getnanduid(char *nanduid);
int setnanduid(char *argv);
int getnanduid_flashid(char *nanduid);

//returns contents of /mnt/a/etc/mc2006MAC
int getetherMAC(char *etherMAC);
//returns battery's voltage in mV. input singleLi: 1 for one cell or 0 for two cells.
int getBatVoltage(int singleLi, unsigned long *pv);
//get machine's type
unsigned long get_mach_type(void);
//get ADC channel's status
int getADC(int ChannelNum, unsigned long *ADCResult_mV);
//returns battery's charge status,1:on 0:off
int getBatChgStatus();

int TERM_setSYStimefromRTC();                                         //set system time (UTC) from rtc time (Local time)
int TERM_setRTCandSYStime(int y,int mo,int d,int h,int mi,int s);     //set rtc time (local time) and system time (UTC)
int TERM_getRTCtime(struct tm *ptm);                                  //get trc time (local time)
int TERM_getSYStime(struct tm *ptm);                                  //get system time, (LOCAL time format!!)


//
//Barcode part:
//
#define BC_ERROR_NONE       0
#define BC_ERROR_TXTIMEOUT  1
#define BC_ERROR_RXTIMEOUT  2
#define BC_ERROR_BADCHKSUM  3
#define BC_ERROR_NACK       4
#define BC_ERROR_NOTOPEN    5
#define BC_ERROR_SETPARAM   6
#define BC_ERROR_OPENFAIL   7
#define BC_ERROR_REOPEN     8
#define BC_ERROR_ERROPEN    9

int BARCODE2D_poweron(void);                    //add for barcode 2D 2012/06/07
int BARCODE2D_trigger( unsigned char * pbarcode);

int BARCODE_poweron(void);
int BARCODE_poweroff(void);
//sets 1 byte param_value to given position at param_num.
int BARCODE_set_param( unsigned char param_num, unsigned char param_value);
//The decoder sends its revision string to the host in the following format:
// S/W_REVISION <space> BOARD_TYPE <space> SCANNER_ID <space> PGM_CHKSUM
//   where:
// S/W_REVISION is the release name of the software
// BOARD_TYPE is N for non-flash decoder board, F for flash
// SCANNER_ID indicates the type of scan engine paired with the decoder
// PGM_CHKSUM is the two-byte checksum of the program code.
int BARCODE_get_revision( unsigned char * rstr);
//output pbarcode string format: "TT:nnnnn....", 
//  where TT is barcode type #, nnnn... is barcode or "NR" for no barcode.
int BARCODE_trigger(unsigned char * pbarcode) ;


//
//for LXR0640(Data Terminal) only, read getmobileinfo output files:
//
int getownnum(char *ownnum);                                         
int getimsi(char *imsi);                                            
int getmodel(char *model);
int gettype(char *type);
int getcard(char *card);


//
//casio APIs:
//
#define MULTIDROP_OK             0
#define MULTIDROP_ERR_TIMEOUT    1
#define MULTIDROP_ERR_WRITE      2
#define MULTIDROP_ERR_READ       3
#define MULTIDROP_ERR_OPENFAIL   4
#define MULTIDROP_ERR_FILENAME   5
#define MULTIDROP_ERR_FILEOPEN   6

#define DT_MODE_ALL 0
#define DT_MODE_SCANONLY 1
#define DT_MODE_KEYONLY  2

//opens the comm for multidrop software,
//CommMode: 0 : UART2-IRDA    1 : UART2
int MULTIDROP_opencomm(int CommMode);     
int MULTIDROP_closecomm();

//handheld requires to download batch files from PC
//set IsEXE to download exe files or reset to download data files
//target directory is /mnt/a/multidrop/exe or /mnt/a/multidrop/down
int MULTIDROP_downloadfiles(int IsEXE,int TimeOutSec);

//handheld requires to upload ONE file to PC
//the filename contains NO directory information, should be 8+3 DOS formart.
//the file stores at /mnt/a/multidrop/up dir.
int MULTIDROP_uploadfile( char *filename,int TimeOutSec);

//emulates casio DT900lib readstr():
int DT_read_bc_str(int x,int y,int mode, unsigned char *strbuf,int MaxStrLen);


//
//RS-485 for LXR0680:
//

#define PARITY_NONE 0
#define PARITY_EVEN 1
#define PARITY_ODD  2
#define PARITY_MARK 3
#define PARITY_SPACE 4

#define RS485_ERROR_NONE    0
#define RS485_ERROR_TIMEOUT 2

int  RS485_poweron(int baudrate,int charsize);
int  RS485_poweroff(int fd485);
void RS485_enable_driver(int fd485);
void RS485_disable_driver(int fd485);
//void RS485_set_addressmode(int fd485, int is_addr);
void RS485_set_parity(int fd485, int parity);

int RS485_send(int fd485, unsigned char * poutdata, int datalen );
int RS485_receive(int fd485,  unsigned char * ptrdata, int expdatalen, int *retdatalen,int timeout);

//RS485 Psedo Multi Drop communication mode using parity check bit to distinguish address and data.
int RS485_PMD_send_addr_data_pack(int fd485, unsigned char * paddress, int addrlen,
unsigned char * poutdata, int datalen);

int  RS485_muxon();
int  RS485_muxoff();


//
//MISC tools:
//
#define RunScript(PARAM)    { char *arglist[4];   arglist[0] = "sh";   arglist[1] = "-c"; arglist[2] = PARAM;	 arglist[3] = 0; return execute(arglist); }



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LIBDRIVER2006*/
