/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_option.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 12:39:39 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** Why use STDIN instead of STDOUT or STDERR or /dev/tty?
** 1. Writing to STDOUT will cause issue with command redirection.
**    Moreover, with ``, isatty(STDOUT_FILENO) return 0.
** 2. Writing to STDERR may works, but i'd rather keep it for error handling.
** 3. /dev/tty should be the proper option and I use it with the -t option.
**    But we can't use global (forbidden by exercice).
** 4. Writing to STDIN seems the most logical choice.
*/

/*
** Note that if term->name already exist (after Ctrl->Z for instance) it will
** be used if TERM has ben unset before relaunching the program.
** Normally we would use TTY_NAME_MAX, but it is not always defined and
** using a conditional define is a pain with the norm.
** we use t and s so we can reload if suspended + TERM unset
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
