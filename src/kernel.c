/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:29:20 by rbourgea          #+#    #+#             */
/*   Updated: 2022/06/22 17:03:00 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

/* ************************************************************************** */
/* Globals                                                                    */
/* ************************************************************************** */

struct IDT_entry IDT[IDT_SIZE]; // This is our entire IDT. Room for 256 interrupts

const size_t	VGA_WIDTH = 80;
const size_t	VGA_HEIGHT = 25;
 
size_t		tty_row;
size_t		tty_column;
uint8_t		tty_color;
uint16_t*	terminal_buffer;

char*		prompt_buffer;
int		tty_nb;
int		tty_pos;
int		tty_prompt_pos;
char		ttys[10][256][256];
size_t		ttys_row[10];
size_t		ttys_column[10];

void set_cursor_position(uint16_t position)
{
	ioport_out(0x3D4, 14);
	ioport_out(0x3D5, (position >> 8));
	ioport_out(0x3D4, 15);
	ioport_out(0x3D5, position);
}

void switch_screen(int nb)
{
	ttys_row[tty_nb] = tty_row;
	ttys_column[tty_nb] = tty_column;
	tty_nb = nb;
	if (ttys_row[tty_nb] != -1)
	{
		terminal_initialize(nb);
		tty_row = ttys_row[tty_nb];
		tty_column = ttys_column[tty_nb];
		// kputchar('\n');
		kprompt(0);
	}
	else			// tty not exist
	{
		terminal_initialize(-1);
		khello();
		kprompt(0);
	}
}

void kprompt(char c)
{
	char *tmp;

	tmp[0] = c;
	if (c == '\n')			// Enter
		kputchar('\n');
	else if (c == '\b')		// Delete
	{
		if (tty_column == 0)
			return;
		tty_column--;
		kputchar(' ');
		tty_column--;
		tty_pos = tty_column + tty_row * VGA_WIDTH;
		set_cursor_position((uint16_t)(tty_pos));
		return;
	}
	else if (c == -11)		// left Ctrl
	{
		// kputstr("left Ctrl");
		return;
	}
	else if (c == -12)		// left Shift
	{
		// kputstr("left Shift");
		return;
	}
	else if (c == -13)		// left arrow
	{
		// if (tty_prompt_pos <= tty_pos)
		// 	return;
		tty_column--;
		tty_pos = tty_column + tty_row * VGA_WIDTH;
		set_cursor_position((uint16_t)(tty_pos));
		return;
	}
	else if (c == -14)		// right arrow
	{
		tty_column++;
		tty_pos = tty_column + tty_row * VGA_WIDTH;
		set_cursor_position((uint16_t)(tty_pos));
		return;
	}
	else if (c < 0 && c > -11)	// F1 to F10
	{
		switch_screen(c + (-c * 2) - 1); // shortcut
		return;
	}
	if (c == '\n' || c == 0)
	{
		if (c == 0)
			kputchar('\n');
		kcolor(VGA_COLOR_RED);
		kputstr("[@rbourgea] \7 ");
		tty_prompt_pos = tty_pos;
		kcolor(VGA_COLOR_LIGHT_GREY);
	}
	if (c != '\n')
		kputchar((const char)c);
	tty_pos = tty_column + tty_row * VGA_WIDTH;
	set_cursor_position((uint16_t)(tty_pos));
}

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(int init)
{
	tty_row = 0;
	tty_column = 0;
	tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			if (init == -1) {
				terminal_buffer[index] = vga_entry(' ', tty_color);
				ttys[tty_nb][x][y] = ' ';
			}
			else {
				terminal_buffer[index] = vga_entry(ttys[tty_nb][x][y], tty_color);
			}
		}
	}
}

void kcolor(uint8_t color)
{
	tty_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

static int count_digits(int n)
{
	int digits;

	if (n == 0)
		return (1);
	digits = n < 0 ? 1 : 0;
	while (n != 0)
	{
		n = n / 10;
		digits++;
	}
	return (digits);
}

void khello(void)
{
	kcolor(VGA_COLOR_LIGHT_GREEN);
	kputstr("\n 42\\   42\\  424242\\        42\\        424242\\           \n");
	kputstr(" 42 |  42 |42  __42\\       42 |      42  __42\\          \n");
	kputstr(" 42 |  42 |\\__/  42 |      42 |  42\\ 42 /  \\__|424242=\\ \n");
	kputstr(" 42424242 | 424242  |      42 | 42  |4242\\    42  _____|\n");
	kputstr(" \\_____42 |42  ____/       424242  / 42  _|   \\424242\\  \n");
	kputstr("       42 |42 |            42  _42<  42 |      \\____42\\ \n");
	kputstr("       42 |42424242\\       42 | \\42\\ 42 |     424242/  |\n");
	kputstr("       \\__|\\________|      \\__|  \\__|\\__|     \\_______/ \n\n");
	kputstr("                                           by rbourgea \2\n");
	kcolor(VGA_COLOR_LIGHT_MAGENTA);
	kputstr(" \4 Login on ttys");
	kputchar((char)(tty_nb + '0'));
	kputstr("\n");
	kcolor(VGA_COLOR_WHITE);
}

void printk(char *str, ...)
{
	int		*nb_args;
	char	*args;
	int		i;
	char	tmp_addr[9];
	uint8_t	zero_padding;

	nb_args = (int *)(&str);
	args = (char *)(*nb_args++);
	i = 0;
	while (args[i]) {
		zero_padding = 0;
		if (args[i] == '%') {
			i++;
			if (args[i] == '0') {
				i++;
				zero_padding = args[i] - '0';
				i++;
			}
			if (args[i] == 'c')
				kputchar(*nb_args++);
			else if (args[i] == 's')
				kputstr(*((char **)nb_args++));
			else if (args[i] == 'x') {
				kmemset(tmp_addr, 0, sizeof(tmp_addr));
				hex_to_str(*nb_args++, tmp_addr, sizeof(tmp_addr));
				kputstr(tmp_addr);
			}
			else if (args[i] == 'p') {
				kmemset(tmp_addr, 0, sizeof(tmp_addr));
				hex_to_str(*nb_args++, tmp_addr, sizeof(tmp_addr));
				kputstr("0x");
				kputstr(tmp_addr);
			}
			else if (args[i] == 'd') {
				if (zero_padding > 0) {
					while (zero_padding - kintlen(*nb_args)) {
						kputchar('0');
						zero_padding--;
					}
				}
				kputnbr(*nb_args++);
			}
			else {
				kputchar('%');
				kputchar(args[i]);
			}
		}
		else
			kputchar(args[i]);
		i++;
	}
}

void colrow_init(void)
{
	for (int i = 0; i < 10; i++)
	{
		ttys_column[i] = -1;
		ttys_row[i] = -1;
	}
}

#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
 
#define GDT_CODE_PL0	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(0)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL0	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(0)     | SEG_DATA_RDWR

#define GDT_STACK_PL0	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(0)     | SEG_DATA_RDWREXPD
 
#define GDT_CODE_PL3	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(3)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL3	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(3)     | SEG_DATA_RDWR

#define GDT_STACK_PL3	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
			SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
			SEG_PRIV(3)     | SEG_DATA_RDWREXPD

#define GDT_ADDRESS	0x00000800
#define GDT_ENTRIES	7
#define FLAG_D_32	0xCF

typedef struct  __attribute__((packed)) gdt_entry
{
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  attributes;
   uint8_t  base_high;           // The last 8 bits of the base.
}  t_gdt_entry;

typedef struct  __attribute__((packed)) gdt_ptr
{
   uint16_t limit;               // The upper 16 bits of all selector limits.
   uint32_t base;                // The address of the first gdt_entry_t struct.
} t_gdt_ptr;

extern void load_gdt(uint32_t gdt_ptr);

void init_gdt();

t_gdt_entry     gdt_entries[GDT_ENTRIES];
t_gdt_ptr       *gdt_ptr = (t_gdt_ptr *)GDT_ADDRESS;

static void create_descriptor(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entries[num].limit_low = (limit & 0xFFFF);
	gdt_entries[num].base_low = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].access = access;
	gdt_entries[num].attributes = (limit >> 16) & 0x0F;
	gdt_entries[num].attributes |= ((flags << 4) & 0xF0);
	gdt_entries[num].base_high = (base >> 24) & 0xFF;
}

void	init_gdt()
{
	gdt_ptr->limit = (sizeof(t_gdt_entry) * GDT_ENTRIES) - 1;
	gdt_ptr->base = ((uint32_t)(&gdt_entries));

	create_descriptor(0, 0, 0, 0, FLAG_D_32);			// null
	create_descriptor(1, 0, 0xFFFFF, (uint8_t)(GDT_CODE_PL0),  FLAG_D_32);	// kernel code
	create_descriptor(2, 0, 0xFFFFF, (uint8_t)(GDT_DATA_PL0),  FLAG_D_32);	// kernel data
	create_descriptor(3, 0, 0xFFFFF, (uint8_t)(GDT_STACK_PL0), FLAG_D_32);	// kernel stack
	create_descriptor(4, 0, 0xBFFFF, (uint8_t)(GDT_CODE_PL3),  FLAG_D_32);	// user code
	create_descriptor(5, 0, 0xBFFFF, (uint8_t)(GDT_DATA_PL3),  FLAG_D_32);	// user data
	create_descriptor(6, 0, 0xBFFFF, (uint8_t)(GDT_STACK_PL3), FLAG_D_32);	// user stack

	load_gdt(((uint32_t)gdt_ptr));
}

void	khexdump(uint32_t addr, int limit)
{
	char *c = (char *)addr;
	char str_addr[9];
	int i;
	uint32_t previous;

	if (limit <= 0)
		return;
	for (i = 0; i < limit; i++)
	{
		if ((i % 16) == 0) // 16 = size line
		{
			if (i != 0)
			{
				previous = addr - 16;
				while (previous < addr)
				{
					if (*(char *)previous <= 32)
					{
						kcolor(VGA_COLOR_RED);
						printk("%c", '.');
					}
					else
					{
						kcolor(VGA_COLOR_GREEN);
						printk("%c", *(char *)previous);
					}
					previous++;
				}
				printk("\n");
			}
			if ((uint32_t)0x00000800 == addr)
				kcolor(VGA_COLOR_CYAN);
			else
				kcolor(VGA_COLOR_LIGHT_BLUE);
			printk("%p: ", addr);
		}
		hex_to_str((uint32_t)c[i], str_addr, 3);
		if ((uint32_t)c[i] == 0) // == 00
			kcolor(VGA_COLOR_RED);
		else
			kcolor(VGA_COLOR_GREEN);
		printk("%s ", str_addr);
		kcolor(VGA_COLOR_WHITE);
		addr++;
	}
	for (i = 0; i < ((limit % 16) * 3); i++) // last line
		printk(" ");
	if ((limit % 16) == 0)
		previous = addr - 16;
	else
		previous = addr - (limit % 16);
	while (previous < addr)
	{
		if (*(char *)previous <= 32)
		{
			kcolor(VGA_COLOR_RED);
			printk("%c", '.');
		}
		else
		{
			kcolor(VGA_COLOR_GREEN);
			printk("%c", *(char *)previous);
		}
		previous++;
	}
	printk("\n");
	kcolor(VGA_COLOR_WHITE);
}

void	kmain()
{
	colrow_init();
	terminal_initialize(-1);
	// khexdump(0x00000800, 10);
	init_gdt();
	khello();
	// printk(" \11 %s, %d%c\n\n", "Hello", 42, '!');

	init_idt();
	kb_init();
	enable_interrupts();

	khexdump(0x000007c0, 100);

	kprompt(0);
	while(42);
}
