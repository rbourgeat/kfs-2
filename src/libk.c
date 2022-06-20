/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libk.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:58:54 by user42            #+#    #+#             */
/*   Updated: 2022/06/20 16:48:39 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

void    kputstr(const char* data) 
{
	for (size_t i = 0; i < kstrlen(data); i++)
		kputchar(data[i]);
}

void	kputnbr(int n)
{
	char	str[kintlen(n) + 1];

	kitoa(n, str);
	kputstr(str);
}

void	kitoa(int n, char *str)
{
	int	nb;
	int	i;
	int	len;

	nb = n;
	len = kintlen(n);
	kmemset(str, 0, len + 1);
	if (nb < 0) {
		nb = -nb;
	}
	i = len - 1;
	if (nb == 0) {
		str[i] = '0';
		return;
	}
	while (nb != 0)
	{
		str[i--] = (nb % 10) + '0';
		nb = nb / 10;
	}
	if (n < 0)
		str[i] = '-';
}

int		kintlen(int n)
{
	int	i;

	i = 1;
	if (n < 0) {
		n = -n;
		i++;
	}
	while (n >= 10)
	{
		n = n / 10;
		i++;
	}
	return (i);
}

void	*kmemset(void *b, int c, unsigned int len)
{
	unsigned int	i;

	i = 0;
	while (i < len)
	{
		((char *)(b))[i] = c;
	i++;
	}
	return b;
}

void	kputchar(char c)
{
	if (c == '\n') {
		tty_column = 0;
		tty_row++;
		if (tty_row == VGA_HEIGHT)
			terminal_initialize(-1);
		return;
	}
	ttys[tty_nb][tty_column][tty_row] = c; // save to tty
	terminal_putentryat(c, tty_color, tty_column, tty_row);
	if (++tty_column == VGA_WIDTH) {
		tty_column = 0;
		if (++tty_row == VGA_HEIGHT)
			tty_row = 0;
	}
}

void	hex_to_str(unsigned int addr, char *result, int size)
{
	int	len;
	char	base_str[16] = "0123456789abcdef";

	len = size - 1;
	kmemset(result, '0', size);
	result[size - 1] = 0;
	while (addr != 0)
	{
		result[--len] = base_str[addr % 16];
		addr = addr / 16;
	}
}

size_t	kstrlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char	*kstrjoin(char const *s1, char const *s2)
{
	size_t	count;
	size_t	size_s1;
	char	*tab;

	count = -1;
	if (!s1 || !s2)
		return (NULL);
	size_s1 = kstrlen(s1);
	while (s1[++count])
		tab[count] = s1[count];
	count = -1;
	while (s2[++count])
		tab[size_s1 + count] = s2[count];
	tab[size_s1 + count] = '\0';
	return (tab);
}
