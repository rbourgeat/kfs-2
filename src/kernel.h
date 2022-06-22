/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:59:25 by user42            #+#    #+#             */
/*   Updated: 2022/06/22 17:43:33 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
# define KERNEL_H

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ************************************************************************** */
/* Pre-processor constants                                                    */
/* ************************************************************************** */

# define LINES 25 // 2 bytes each
# define COLUMNS_IN_LINE 80

# define IDT_SIZE 256 // Specific to x86 architecture
# define KERNEL_CODE_SEGMENT_OFFSET 0x8
# define IDT_INTERRUPT_GATE_32BIT 0x8e

# define PIC1_COMMAND_PORT 0x20
# define PIC1_DATA_PORT 0x21
# define PIC2_COMMAND_PORT 0xA0
# define PIC2_DATA_PORT 0xA1

# define KEYBOARD_DATA_PORT 0x60
# define KEYBOARD_STATUS_PORT 0x64

# pragma GCC diagnostic ignored "-Waddress-of-packed-member"

/* ************************************************************************** */
/* Structs                                                                    */
/* ************************************************************************** */

struct IDT_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct IDT_entry {
	unsigned short offset_lowerbits; // 16 bits
	unsigned short selector; // 16 bits
	unsigned char zero; // 8 bits
	unsigned char type_attr; // 8 bits
	unsigned short offset_upperbits; // 16 bits
} __attribute__((packed));

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* ************************************************************************** */
/* Globals                                                                    */
/* ************************************************************************** */

extern struct IDT_entry IDT[IDT_SIZE]; // This is our entire IDT. Room for 256 interrupts

extern const size_t	VGA_WIDTH;
extern const size_t	VGA_HEIGHT;
 
extern size_t		tty_row;
extern size_t		tty_column;
extern uint8_t		tty_color;
extern uint16_t*	terminal_buffer;

extern char*		prompt_buffer;
extern int		tty_nb;
extern int		tty_pos;
extern int		tty_prompt_pos;
extern char		ttys[10][256][256];
extern size_t		ttys_row[10];
extern size_t		ttys_column[10];

/* ************************************************************************** */
/* boots.s functions                                                          */
/* ************************************************************************** */

extern void print_char_with_asm(char c, int row, int col);
extern void keyboard_handler();
extern char ioport_in(unsigned short port);
extern void ioport_out(unsigned short port, unsigned char data);
extern void load_idt(unsigned int* idt_address);
extern void enable_interrupts();

/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */

// libk.c
void    kputnbr(int n);
int	kintlen(int n);
void	kputchar(char c);
void	kitoa(int n, char *str);
void	kputstr(const char* data);
void	khexdump(uint32_t addr, int limit);
void*	kmemset(void *b, int c, unsigned int len);
void	hex_to_str(unsigned int addr, char *result, int size);
char*	kstrjoin(char const *s1, char const *s2);
size_t	kstrlen(const char* str);
void	printk(char *str, ...);

// keyboard.c
void	kb_init();
void	init_idt();
void	handle_keyboard_interrupt();

// terminal.c
void	kprompt(char c);
void	switch_screen(int nb);
void	terminal_initialize(int init);
void	set_cursor_position(uint16_t position);

// kernel.c
void	kcolor(uint8_t color);
void	khello(void);
void    terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void	khexdump(uint32_t addr, int limit);

#endif