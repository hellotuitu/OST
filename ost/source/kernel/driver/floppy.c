#include "floppy.h"
#include "o_stdlib.h"
#include "o_stdio.h"
#include "ostio.h"

// ---------------------------------------------

// 软盘驱动

// 该模块导出： floppy_api(sectno, buffer, type)


// 该模块导入：
//   无


// -------------------------------------------
static struct floppy_request REQUEST;
static floppy_struct floppy_type = {2880,18,2,80,0,0x1B,0x00,0xCF }; /* 1.44MB diskette */


void (* do_floppy)(void);
static void output_byte(char byte);
static void do_floppy_request();

static unsigned int head, track, block, sector, seek_track;
static unsigned char tmp_floppy_area[1024] = {0};
static unsigned int current_dev = 0;
static int recalibrate = 1; // 软盘校正标志
static int reset = 1; // 重启标志
static int seek = 1; // 寻道标志

static int finished = 0;

#define MAX_ERRORS 8
#define MAX_REPLIES 7
static unsigned char reply_buffer[MAX_REPLIES];
#define ST0 (reply_buffer[0])
#define ST1 (reply_buffer[1])
#define ST2 (reply_buffer[2])
#define ST3 (reply_buffer[3])

#define copy_buffer(from,to) do{ \
	memcpy(to, from, 1024); \
}while(0)

static void output_byte(char byte){
  int counter;
  unsigned char status;

  for(counter = 0 ; counter < 10000 ; counter++) {
    status = inbyte(FD_STATUS) & (STATUS_READY | STATUS_DIR);
    if (status == STATUS_READY) {
      outbyte(FD_DATA, byte);
      return;
    }
  }
  printf("Unable to send byte to FDC\r");
}

static void setup_DMA(unsigned char command){
  long addr = (long) REQUEST.buffer;

  cli();
  if (addr >= 0x100000) {
    addr = (long) tmp_floppy_area;
  if (command == FD_WRITE)
    copy_buffer(REQUEST.buffer, tmp_floppy_area);
  }
  /* mask DMA 2 */
  immoutb_p(4|2,10);

  unsigned char val = (command == FD_READ)?DMA_READ:DMA_WRITE;
  outbyte(val, 12);
  outbyte(val, 11);

  immoutb_p(addr,4);
  addr >>= 8;
  /* bits 8-15 of addr */
  immoutb_p(addr,4);
  addr >>= 8;
  /* bits 16-19 of addr */
  immoutb_p(addr,0x81);
  /* low 8 bits of count-1 (1024-1=0x3ff) */
  immoutb_p(0xff,5);
  /* high 8 bits of count-1 */
  immoutb_p(3,5);
  /* activate DMA 2 */
  immoutb_p(0|2,10);
  sti();
}
static int result(void){
  // 读取中断状态结果
  int i = 0, counter, status;

  for (counter = 0 ; counter < 10000 ; counter++) {
    status = inbyte(FD_STATUS)&(STATUS_DIR|STATUS_READY|STATUS_BUSY);
    if (status == STATUS_READY)
      return i;
    if (status == (STATUS_DIR|STATUS_READY|STATUS_BUSY)) {
      if (i >= MAX_REPLIES)
        break;
      reply_buffer[i++] = inbyte(FD_DATA);
    }
  }
  printf("Getstatus times out\n\r");
  return -1;
}
static void reset_interrupt(){
  // 启动软盘控制器后的中断
  output_byte(FD_SENSEI); // 检测中断状态命令
  result();
  outbyte(FD_DCR, 0); // 500kpbs
  output_byte(FD_SPECIFY); // 发送设定软驱命令参数
  output_byte(0xCF); // 马达步进速度
  output_byte(6);
  do_floppy_request();
}

static void reset_floppy(){
  cli();
  reset = 0;
  do_floppy = reset_interrupt;
  outbyte(FLOPPY_REG_DOR, 0x08); // 重启

  for (int i=0 ; i<100 ; i++)
    __asm__("nop"); // 延时，保证重启完成

  outbyte(FLOPPY_REG_DOR, 0xc); // 选择DMA模式，选择软驱A 启动软盘控制器
  // 第一次中断
  sti();
}

static void recal_interrupt(){
  // 第二次中断 重新校正完成
  // printf("recal_interrupt");
  output_byte(FD_SENSEI);
	if (result()!=2 || (ST0 & 0xE0) == 0x60){
    printf("fail to recal");
  }
	else
		recalibrate = 0;
	do_floppy_request();
}
static void recalibrate_floppy(){
  recalibrate = 0;
  do_floppy = recal_interrupt;
  output_byte(FD_RECALIBRATE);
  output_byte(0); // 磁头号 和 驱动器号
}

static void seek_interrupt(){
  output_byte(FD_SENSEI);
	if (result() != 2 || (ST0 & 0xF8) != 0x20 || ST1 != seek_track) {
		printf("fail to seek");
		return;
	}
  do_floppy_request();
}
static void seek_floppy(){
  seek = 0;
  do_floppy = seek_interrupt;
  output_byte(FD_SEEK);
  output_byte(head<<2 | current_dev);
  output_byte(seek_track);
}

static void rw_interrupt(){
  if (result() != 7 || (ST0 & 0xf8) || (ST1 & 0xbf) || (ST2 & 0x73)) {
		printf("fail");
		return;
	}
  if ((long)REQUEST.buffer >= 0x100000) {
    if (REQUEST.type == 0)
      copy_buffer(tmp_floppy_area, REQUEST.buffer);
  }

  finished = 1;
}
static void setup_rw_floppy(){
  do_floppy = rw_interrupt;

  output_byte(FD_READ); /* command */
  output_byte(current_dev); /* driver no. */
  output_byte(track); /* track no. */
  output_byte(head); /* head */
  output_byte(sector); /* start sector */
  output_byte(2); /* sector size = 512 */
  output_byte(floppy_type.sect); /* Max sector */
  output_byte(floppy_type.gap); /* sector gap */
  output_byte(0xFF); /* sector size (0xff when n!=0 ?) */
}
static void do_floppy_request(){
  int sectNo = REQUEST.sectNo;
  char* buf = REQUEST.buffer;
  if (NULL == buf){
      printf("FloppyReadSector para error.\r");
      return;
  }
  if (sectNo >= (floppy_type.head * floppy_type.track * floppy_type.sect)){
      printf("FloppyReadSector sectNo error: %x.\r", sectNo);
      return;
  }
  sector = sectNo % floppy_type.sect + 1;
  block = sectNo / floppy_type.sect;
  track = block / floppy_type.head;
  head = block % floppy_type.head;
  seek_track = track << floppy_type.stretch;

  if(reset){
    reset_floppy();
    return;
  }
  if(recalibrate){
    recalibrate_floppy();
    return;
  }
  if(seek){
    seek_floppy();
    return;
  }

  setup_DMA(FD_READ);
  setup_rw_floppy();
}
void floppy_api(unsigned int sectNo, unsigned char *buf, int type){
  seek = 1;
  reset = 1;
  recalibrate = 1;
  finished = 0;

  REQUEST.type = type; // 读
  REQUEST.sectNo = sectNo;
  REQUEST.buffer = buf;

  memset(tmp_floppy_area, 0, 1024);

  do_floppy_request();

  while(!finished){}
}

void floppy_handler(){
  do_floppy();
}
