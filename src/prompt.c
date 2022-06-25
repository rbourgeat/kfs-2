/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 16:53:34 by user42            #+#    #+#             */
/*   Updated: 2022/06/25 13:11:41 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

#define GET_STACK_POINTER(x)	asm volatile("mov %%esp, %0" : "=r"(x) ::)
#define GET_STACK_FRAME(x)	asm volatile("mov %%ebp, %0" : "=r"(x) ::)

void	kexec()
{
	int	sp;
	int	sf;

	prompt_buffer[prompt_buffer_i] = 0;
	if (kstrcmp(prompt_buffer, "hello") == 0)
		printk("Hello, %d!", 42);
	else if (kstrcmp(prompt_buffer, "stack") == 0)
	{
		GET_STACK_POINTER(sp);
		GET_STACK_FRAME(sf);
		khexdump(sp, sf - sp + 1);
	}
	else if (kstrcmp(prompt_buffer, "clear") == 0)
		printk("\n\n\n\n\n\n\n\n\n\n");
	else if (kstrcmp(prompt_buffer, "push42") == 0)
	{
		// asm volatile("push 42 \n\t"
		// 		"pop eax");
	}
	else {
		kcolor(VGA_COLOR_RED);
		printk("Command not found: %s", prompt_buffer);
		kcolor(VGA_COLOR_LIGHT_GREY);
	}
	kputchar('\n');

	// for (int i = 0; i < prompt_buffer_i; i++)
	// 	prompt_buffer[i] = 0;
	kmemset(prompt_buffer, 0, 1024);
	prompt_buffer_i = 0;

	tty_pos = tty_column + tty_row * VGA_WIDTH;
	set_cursor_position((uint16_t)(tty_pos));
}

void	kprompt(char c)
{
	char *tmp;

	tmp[0] = c;
	if (c == '\n')			// Enter
		kputchar('\n');
	else if (c == '\b')		// Delete
	{
		if (tty_pos <= tty_prompt_pos)
			return;
		if (tty_column == 0)
			return;
		tty_column--;
		kputchar(' ');
		tty_column--;
		tty_pos = tty_column + tty_row * VGA_WIDTH;
		set_cursor_position((uint16_t)(tty_pos));
		
		prompt_buffer[prompt_buffer_i] = 0;
		prompt_buffer_i--;
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
		if (tty_pos <= tty_prompt_pos)
			return;
		tty_column--;
		tty_pos = tty_column + tty_row * VGA_WIDTH;
		set_cursor_position((uint16_t)(tty_pos));
		return;
	}
	else if (c == -14)		// right arrow
	{
		if (tty_pos > tty_prompt_pos + prompt_buffer_i - 1)
			return;
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
		else
			kexec();
		kcolor(VGA_COLOR_RED);
		kputstr("[@rbourgea] \7 ");
		kcolor(VGA_COLOR_LIGHT_GREY);
		tty_prompt_pos = tty_column + tty_row * VGA_WIDTH;
	}
	if (c != '\n' && c != 0)
	{
		kputchar((const char)c);
		prompt_buffer[prompt_buffer_i] = c;
		prompt_buffer_i++;
	}
	tty_pos = tty_column + tty_row * VGA_WIDTH;
	set_cursor_position((uint16_t)(tty_pos));
}

void	switch_screen(int nb)
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

void	set_cursor_position(uint16_t position)
{
	ioport_out(0x3D4, 14);
	ioport_out(0x3D5, (position >> 8));
	ioport_out(0x3D4, 15);
	ioport_out(0x3D5, position);
}
