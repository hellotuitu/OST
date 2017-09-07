#ifndef _TIME_H
#define _TIME_H

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#define CLOCKS_PER_SEC 100

typedef long clock_t;

typedef struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} Time;

void time_init();
long kernel_mktime(struct tm * tm);
void get_time(Time*);
void sysbeepstop(void);
void sysbeep(void);

#define sleep(x) do{ \
  int a = get_ticks(); \
  while((get_ticks() - a)  < x * 100); \
}while(0)

#endif
