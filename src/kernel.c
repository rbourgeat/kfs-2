/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:29:20 by rbourgea          #+#    #+#             */
/*   Updated: 2022/06/22 17:42:11 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "gdt.h"

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

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void	terminal_initialize(int init)
{
	tty_row = 0;
	tty_column = 0;
	tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			if (init == -1) 
			{
				terminal_buffer[index] = vga_entry(' ', tty_color);
				ttys[tty_nb][x][y] = ' ';
			}
			else
				terminal_buffer[index] = vga_entry(ttys[tty_nb][x][y], tty_color);
		}
	}
}

void	kcolor(uint8_t color)
{
	tty_color = color;
}

void	terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
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

void	khello(void)
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

void colrow_init(void)
{
	for (int i = 0; i < 10; i++)
	{
		ttys_column[i] = -1;
		ttys_row[i] = -1;
	}
}

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

	create_descriptor(0, 0, 0, 0, FLAG_D_32);				// null
	create_descriptor(1, 0, 0xFFFFF, (uint8_t)(GDT_CODE_PL0),  FLAG_D_32);	// kernel code
	create_descriptor(2, 0, 0xFFFFF, (uint8_t)(GDT_DATA_PL0),  FLAG_D_32);	// kernel data
	create_descriptor(3, 0, 0xFFFFF, (uint8_t)(GDT_STACK_PL0), FLAG_D_32);	// kernel stack
	create_descriptor(4, 0, 0xBFFFF, (uint8_t)(GDT_CODE_PL3),  FLAG_D_32);	// user code
	create_descriptor(5, 0, 0xBFFFF, (uint8_t)(GDT_DATA_PL3),  FLAG_D_32);	// user data
	create_descriptor(6, 0, 0xBFFFF, (uint8_t)(GDT_STACK_PL3), FLAG_D_32);	// user stack

	load_gdt(((uint32_t)gdt_ptr));
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
