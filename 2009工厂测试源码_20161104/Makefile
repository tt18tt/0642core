CROSS_COMPILE 	= arm-linux-

#
# Include the make variables (CC, etc...)
#进行宏定义

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc -I/mc2005/include -L/mc2005/lib/.libs  
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
# Comment/uncomment the following line to enable/disable debugging

#DEBUG = y

OBJS= 0642-Full-function.o 

all: 0642-Full.x

.c.e:
	$(CC) -I/mc2005/include -L/mc2005/lib/.libs -lpthread -ltty2006 -ldriver2006 -lgcc_s -L/arm9-demo/0642-demo/0642-Full-function -E -o $@ $<

0642-Full.x: $(OBJS)
	$(CC) -I/mc2005/include -L/mc2005/lib/.libs -lpthread -ltty2006 -ldriver2006  -lgcc_s -L/arm9-demo/0642-demo/0642-Full-function -O -o $@ $(OBJS)libmc531.a libpn512.a iso15693.a cdcl_rc632.a libicc.a
	$(STRIP) $@
clean:
	rm -Rf *.o 0642-Full.x
	-rm -f *~ core   
	-rm -f *.bak core
