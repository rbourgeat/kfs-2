/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:29:20 by rbourgea          #+#    #+#             */
/*   Updated: 2022/06/21 14:37:18 by rbourgea         ###   ########.fr       */
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

void kmain()
{
	colrow_init();
	terminal_initialize(-1);
	khello();
	// printk(" \11 %s, %d%c\n\n", "Hello", 42, '!');

	init_idt();
	kb_init();
	enable_interrupts();

	kprompt(0);
	while(42);
}
