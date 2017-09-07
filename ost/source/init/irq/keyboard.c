#include "../../include/headers.h"

// ---------------------------------------------

// 键盘中断处理

// 该模块导出：
//   keyboard_read 函数 和相应的 API
//   键盘中断处理 keyboard_handler 函数
//   键盘初始化函数 init_keyboard

// 该模块导入:
//   无

// -------------------------------------------


PRIVATE	KB_INPUT	kb_in;
PRIVATE	t_bool		code_with_E0	= FALSE;
PRIVATE	t_bool		shift_l;		/* l shift state	*/
PRIVATE	t_bool		shift_r;		/* r shift state	*/
PRIVATE	t_bool		alt_l;			/* l alt state		*/
PRIVATE	t_bool		alt_r;			/* r left state		*/
PRIVATE	t_bool		ctrl_l;			/* l ctrl state		*/
PRIVATE	t_bool		ctrl_r;			/* l ctrl state		*/
PRIVATE	t_bool		caps_lock;		/* Caps Lock		*/
PRIVATE	t_bool		num_lock;		/* Num Lock		*/
PRIVATE	t_bool		scroll_lock;		/* Scroll Lock		*/
PRIVATE	int		column		= 0;

// 键处理函数 将按键转换成ansi
static char key_process(t_32 key);

void init_keyboard(){
  kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;
}

PRIVATE t_8 get_byte_from_kb_buf()	/* 从键盘缓冲区中读取下一个字节 */
{
	t_8	scan_code;

	while (kb_in.count <= 0) {}	/* 等待下一个字节到来 */

	// disable_int();
  asm("cli");

	scan_code = *(kb_in.p_tail);
	kb_in.p_tail++;
	if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES) {
		kb_in.p_tail = kb_in.buf;
	}
	kb_in.count--;
	// enable_int();
  asm("sti");

	return scan_code;
}
char keyboard_read(){
  t_8	scan_code;
	t_bool	make;	/* TRUE : make  */
			/* FALSE: break */
	t_32	key = 0;/* 用一个整型来表示一个键。 */
			/* 比如，如果 Home 被按下，则 key 值将为定义在 keyboard.h 中的 'HOME'。*/
	t_32*	keyrow;	/* 指向 keymap[] 的某一行 */

	if(kb_in.count > 0){
		code_with_E0 = FALSE;

		scan_code = get_byte_from_kb_buf();

		/* 下面开始解析扫描码 */
		if (scan_code == 0xE1) {
			int i;
			t_8 pausebreak_scan_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
			t_bool is_pausebreak = TRUE;
			for(i=1;i<6;i++){
				if (get_byte_from_kb_buf() != pausebreak_scan_code[i]) {
					is_pausebreak = FALSE;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0) {
			scan_code = get_byte_from_kb_buf();

			/* PrintScreen 被按下 */
			if (scan_code == 0x2A) {
				if (get_byte_from_kb_buf() == 0xE0) {
					if (get_byte_from_kb_buf() == 0x37) {
						key = PRINTSCREEN;
						make = TRUE;
					}
				}
			}

			/* PrintScreen 被释放 */
			if (scan_code == 0xB7) {
				if (get_byte_from_kb_buf() == 0xE0) {
					if (get_byte_from_kb_buf() == 0xAA) {
						key = PRINTSCREEN;
						make = FALSE;
					}
				}
			}

			/* 不是 PrintScreen。此时 scan_code 为 0xE0 紧跟的那个值。 */
			if (key == 0) {
				code_with_E0 = TRUE;
			}
		}
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
			/* 首先判断Make Code 还是 Break Code */
			make = (scan_code & FLAG_BREAK ? FALSE : TRUE);

			/* 先定位到 keymap 中的行 */
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

			column = 0;

			if (shift_l || shift_r) {
				column = 1;
			}

			if (code_with_E0) {
				column = 2;
			}

			key = keyrow[column];

			switch(key) {
			case SHIFT_L:
				shift_l	= make;
				break;
			case SHIFT_R:
				shift_r	= make;
				break;
			case CTRL_L:
				ctrl_l	= make;
				break;
			case CTRL_R:
				ctrl_r	= make;
				break;
			case ALT_L:
				alt_l	= make;
				break;
			case ALT_R:
				alt_l	= make;
				break;
			default:
				break;
			}
		}

		if(make){ /* 忽略 Break Code */
			key |= shift_l	? FLAG_SHIFT_L	: 0;
			key |= shift_r	? FLAG_SHIFT_R	: 0;
			key |= ctrl_l	? FLAG_CTRL_L	: 0;
			key |= ctrl_r	? FLAG_CTRL_R	: 0;
			key |= alt_l	? FLAG_ALT_L	: 0;
			key |= alt_r	? FLAG_ALT_R	: 0;

			return key_process(key);
		}
	}

  //其他所有都返回零
  return 0;
}

static char key_process(t_32 key){
  //处理按键
  //将按键转换成ansi返回
	if (!(key & FLAG_EXT)){
    //可打印字符
    // char	output = 0;
    //
		// output = key & 0xFF;
		// ost_colorful_putchar(output, make_color(BLACK, RED));
    return key & 0xFF;
	}
  else {
		int raw_code = key & MASK_RAW;
		switch(raw_code) {
		case UP:
			break;
		case DOWN:
			break;
    case BACKSPACE:
      // ost_putchar(40);
      return 40;
      break;
    case ENTER:
      // ost_putchar('\n');
      return '\n';
      break;
		default:
      return 0;
			break;
		}
	}
}

void keyboard_handler(){
  t_8 scan_code = inbyte(KB_READ_PORT) & 0xFF;

	if (kb_in.count < KB_IN_BYTES) {
		*(kb_in.p_head) = scan_code;
		kb_in.p_head++;
		if (kb_in.p_head == kb_in.buf + KB_IN_BYTES) {
			kb_in.p_head = kb_in.buf;
		}
		kb_in.count++;
	}
}
