/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putchar_and_errmsg.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 17:24:27 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					print_usage(void)
{
	ft_dprintf(STDOUT_FILENO, MSG_USAGE);
	return (END_USAGE);
}

int					errmsg(int error)
{
	if (error == ERR_MEM)
		ft_dprintf(STDERR_FILENO, MSG_MEM);
	else if (error == ERR_EMPTYARG)
		ft_dprintf(STDERR_FILENO, MSG_EMPTYARG);
	else if (error == ERR_TERMENV)
		ft_dprintf(STDERR_FILENO, MSG_TERMENV);
	else if (error == ERR_BADFDTTY)
		ft_dprintf(STDERR_FILENO, MSG_BADFDTTY);
	else if (error == ERR_BADSTDIN)
		ft_dprintf(STDERR_FILENO, MSG_BADSTDIN);
	else if (error == ERR_NOTERMINFO)
		ft_dprintf(STDERR_FILENO, MSG_NOTERMINFO);
	else if (error == ERR_TCGET || error == ERR_TCSET || error == ERR_TPUTS)
		ft_dprintf(STDERR_FILENO, MSG_TTY);
	else if (error == ERR_KEYREAD)
		ft_dprintf(STDERR_FILENO, MSG_KEYREAD);
	else if (error == ERR_RELOAD)
		ft_dprintf(STDERR_FILENO, MSG_RELOAD);
	else if (error == END_USAGE)
		return (0);
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
