#include "../include/headers.h"

// ---------------------------------------------

// 有关时间的代码
// NOTE: copyed from linux maic.c

// 该模块导出：
//   time_init()

// 该模块导入：
//   set_irq set_syscall_env

// -------------------------------------------

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

#define CMOS_READ(addr) ({ \
outbyte_d(0x70, 0x80|addr); \
inbyte_d(0x71); \
})

#define outb_p(x, y) outbyte_d(y, x)
#define outb(x, y) outbyte(y, x)
#define inb_p(x) inbyte_d(x)

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

static int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

unsigned long startup_time = 0;

void time_init(void){
	struct tm time;

	do {
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year = CMOS_READ(9);
	} while (time.tm_sec != CMOS_READ(0));
	BCD_TO_BIN(time.tm_sec);
	BCD_TO_BIN(time.tm_min);
	BCD_TO_BIN(time.tm_hour);
	BCD_TO_BIN(time.tm_mday);
	BCD_TO_BIN(time.tm_mon);
	BCD_TO_BIN(time.tm_year);
	time.tm_mon--;

	startup_time = kernel_mktime(&time);
}

void get_time(Time* time){
	do {
		time -> tm_sec = CMOS_READ(0);
		time -> tm_min = CMOS_READ(2);
		time -> tm_hour = CMOS_READ(4);
		time -> tm_mday = CMOS_READ(7);
		time -> tm_mon = CMOS_READ(8);
		time -> tm_year = CMOS_READ(9);
	} while (time -> tm_sec != CMOS_READ(0));
	BCD_TO_BIN(time -> tm_sec);
	BCD_TO_BIN(time -> tm_min);
	BCD_TO_BIN(time -> tm_hour);
	BCD_TO_BIN(time -> tm_mday);
	BCD_TO_BIN(time -> tm_mon);
	BCD_TO_BIN(time -> tm_year);
}

long kernel_mktime(struct tm * tm)
{
	long res;
	int year;

	year = tm->tm_year - 70;
/* magic offsets (y+1) needed to get leapyears right.*/
	res = YEAR*year + DAY*((year+1)/4);
	res += month[tm->tm_mon];
/* and (y+2) here. If it wasn't a leap-year, we have to adjust */
	if (tm->tm_mon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(tm->tm_mday-1);
	res += HOUR*tm->tm_hour;
	res += MINUTE*tm->tm_min;
	res += tm->tm_sec;
	return res;
}

void sysbeepstop(void){
	/* disable counter 2 */
	outb(inb_p(0x61)&0xFC, 0x61);
}

int beepcount = 0;

void sysbeep(void){
	/* enable counter 2 */
	outb_p(inb_p(0x61)|3, 0x61);
	/* set command for counter 2, 2 byte write */
	outb_p(0xB6, 0x43);
	/* send 0x637 for 750 HZ */
	outb_p(0x37, 0x42);
	outb(0x06, 0x42);
	/* 1/8 second */
	beepcount = 100/8;
}
