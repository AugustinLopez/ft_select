/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_option.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/21 17:05:55 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** UPDATE: Not using /dev/tty: I would need to use global outside of signal.
** I will NOT use singleton in C: it is just an obfuscated global.
** Why using STDIN instead of STDOUT or STDERR?
** 1. Writing to STDOUT will cause issue with command redirection.
**    Moreover, with ``, isatty(STDOUT_FILENO) return 0.
** 2. Writing to STDERR may works, but it's a dirty solution.
** 3. Writing to STDIN seems the most logical choice.
** Normally we would use TTY_NAME_MAX, but it is not always defined and
** using a conditional define is a pain with the norm.
** we use t and s so we can reload if suspended + TERM unset
*/

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
** Note that if term->name already exist (after Ctrl->Z for instance) it will
** be used if TERM has ben unset before relaunching the program.
*/

int					get_terminal(t_term *term)
{
	char	buff[512];
	char	*t;
	int		fd;

	if (!(t = getenv("TERM")) && !term->name)
		return (errmsg(ERR_TERMENV));
	term->name = t;
	if (term->flag & SELECT_T)
	{
		if ((fd = open("/dev/tty", O_RDWR)) == -1)
			return (errmsg(ERR_BADFDTTY));
		term->fd = singleton_fd(fd);
		term->putchar = putchar_fd;
	}
	else
	{
		term->fd = STDIN_FILENO;
		term->putchar = putchar_in;
	}
	if (!isatty(term->fd))
		return (term->fd == STDIN_FILENO ?
			errmsg(ERR_BADSTDIN) : errmsg(ERR_BADFDTTY));
	else if (tgetent(buff, term->name) < 1)
		return (errmsg(ERR_NOTERMINFO));
	return (0);
}

/*
**  VMIN and VTIME: read() option.
** `ti': terminal mode adapted for full-screen display. Undo with `te'
** `vi': make the cursor invisible. Undo with `ve'
** `cl': clear the entire screen.
** `ho': move the cursor in the upper left corner of the screen.
*/

int					load_new_terminal(t_term *term)
{
	int	ret;

	ret = tcgetattr(term->fd, &term->saved);
	ret += tcgetattr(term->fd, &term->current);
	if (ret)
		return (errmsg(ERR_TCGET));
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	ret += tcsetattr(term->fd, TCSANOW, &term->current);
	if (ret)
		return (errmsg(ERR_TCSET));
	ret += tputs(tgetstr("ti", NULL), 1, term->putchar);
	if (!(term->flag & SELECT_CC))
		ret += tputs(tgetstr("vi", NULL), 1, term->putchar);
	if (ret)
		return (errmsg(ERR_TPUTS));
	return (0);
}

int					load_saved_terminal(t_term *term)
{
	int	ret;

	if ((ret = tcsetattr(term->fd, TCSANOW, &term->saved)))
		return (errmsg(ERR_TCSET));
	term->saved.c_lflag |= (ICANON | ECHO);
	ret += tputs(tgetstr("te", NULL), 1, term->putchar);
	ret += tputs(tgetstr("ve", NULL), 1, term->putchar);
	if (ret)
		return (errmsg(ERR_TPUTS));
	return (ret);
}

/*
** If I used /dev/tty, i would need a global or singleton to do it correctly
** (IE: even if i could do without global it would be hard, ugly and hacky)
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
