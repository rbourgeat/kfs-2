/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:59:25 by user42            #+#    #+#             */
/*   Updated: 2022/06/20 16:23:50 by user42           ###   ########.fr       */
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
/* Functions                                                                  */
/* ************************************************************************** */

void	kputstr(const char* data);
void	kputchar(char c);
size_t	kstrlen(const char* str);
void	kcolor(uint8_t color);
void	kprompt(char c);
char	*kstrjoin(char const *s1, char const *s2);
void	terminal_initialize(int init);
void	khello(void);
void	*kmemset(void *b, int c, unsigned int len);
int		kintlen(int n);
void	kputnbr(int n);
void	printk(char *str, ...);

#endif