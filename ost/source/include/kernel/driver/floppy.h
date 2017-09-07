#ifndef _FLOPPY_H_

#define _FLOPPY_H_

void floppy_api(unsigned int sector, unsigned char* buffer, int type);

#define DMA_READ 0x46

#define DMA_WRITE 0x4A

#define FD_STATUS	0x3f4
#define FD_DATA		0x3f5
#define FD_DOR		0x3f2		/* Digital Output Register */
#define FD_DIR		0x3f7		/* Digital Input Register (read) */
#define FD_DCR		0x3f7		/* Diskette Control Register (write)*/

#define FLOPPY_REG_DOR 0x3f2

#define FD_RECALIBRATE	0x07		/* move to track 0 */
#define FD_SEEK		0x0F		/* seek track */
#define FD_READ		0xE6		/* read with MT, MFM, SKip deleted */
#define FD_WRITE	0xC5		/* write with MT, MFM */
#define FD_SENSEI	0x08		/* Sense Interrupt Status */
#define FD_SPECIFY	0x03		/* specify HUT etc */

#define STATUS_BUSYMASK	0x0F		/* drive busy mask */
#define STATUS_BUSY	0x10		/* FDC busy */
#define STATUS_DMA	0x20		/* 0- DMA mode */
#define STATUS_DIR	0x40		/* 0- cpu->fdc */
#define STATUS_READY	0x80		/* Data reg ready */

#define immoutb_p(val,port) outbyte_d(port, val)

typedef struct {

       unsigned int size, sect, head, track, stretch;

       unsigned char gap,rate,spec1;

}floppy_struct;

struct floppy_request{
  int type; // 读或写
  char* buffer;
  int sectNo;
};


#endif
