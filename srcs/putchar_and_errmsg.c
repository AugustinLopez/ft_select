/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putchar_and_errmsg.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 12:27:57 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					errmsg(int error)
{
	if (error == ERR_USAGE)
		ft_dprintf(STDOUT_FILENO,
			"usage: ./ft_select [-CGhmcpt] [--] [arg1 arg2 ...]\n");
	else if (error == ERR_MEM)
		ft_dprintf(STDERR_FILENO, "ft_select: not enough memory.\n");
	else if (error == ERR_EMPTYARG)
		ft_dprintf(STDERR_FILENO, "ft_select: arguments are empty.\n");
	else if (error == ERR_TERMENV)
		ft_dprintf(STDERR_FILENO,
			"ft_select: could not get terminal environment variable.\n");
	else if (error == ERR_BADFDTTY)
		ft_dprintf(STDERR_FILENO, "ft_select: fd not linked to terminal.\n");
	else if (error == ERR_BADSTDIN)
		ft_dprintf(STDERR_FILENO,
			"ft_select: STDIN not linked to terminal. Try the -t option.\n");
	else if (error == ERR_NOTERMINFO)
		ft_dprintf(STDERR_FILENO, "ft_select: no information on terminal.\n");
	else if (error == ERR_TCGET || error == ERR_TCSET || error == ERR_TPUTS)
		ft_dprintf(STDERR_FILENO, "ft_select: cannot modify terminal.\n");
	else if (error == ERR_KEYREAD)
		ft_dprintf(STDERR_FILENO, "ft_select: cannot read from terminal.\n");
	return (error);
}

/*
** I am only using a "singleton" because global are forbidden outside of signal.
** It also why "this" is not used outside of the -t option:
** I don't want "this" in my main program.
*/

int					singleton_fd(int c)
{
	static int a = STDIN_FILENO;

	if (c != -1)
		a = c;
	return (a);
}

int					putchar_fd(int c)
{
	return (write(singleton_fd(-1), &c, 1));
}

int					putchar_in(int c)
{
	return (write(STDIN_FILENO, &c, 1));
}
