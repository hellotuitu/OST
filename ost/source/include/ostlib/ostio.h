//底层实现相关的io函数
#ifndef _OSTIO_H_

#define _OSTIO_H_

#ifndef _TYPE_H_
#include "../type.h"
#endif

void ost_puts(char*); // 输出字符串到显示器
void ost_putchar(char); // 输出字符到显示器
void clean_screen(void); // 清屏
void outbyte_d(unsigned short, unsigned char); // 写端口， 带延迟
void outword(unsigned short port, unsigned short value);
char inbyte(unsigned short port);
char inbyte_d(unsigned short port);
unsigned short inword(unsigned short port);
void outbyte(unsigned short, unsigned char);
void ost_colorful_putchar(char, char);
void ost_colorful_puts(char* str, char color);


#define cli()  asm("cli")
#define sti()  asm("sti")

#define NULL 0

#define	BLACK	0x0	/* 0000 */
#define	WHITE	0x7	/* 0111 */
#define	RED	0x4	/* 0100 */
#define	GREEN	0x2	/* 0010 */
#define	BLUE	0x1	/* 0001 */
#define	FLASH	0x80	/* 1000 0000 */
#define	BRIGHT	0x08	/* 0000 1000 */
#define	make_color(x,y)	((x<<4) | y)	/* MAKE_COLOR(Background,Foreground) */

#endif
