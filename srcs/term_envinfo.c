/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term_envinfo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/24 15:38:26 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** Why use STDIN instead of STDOUT or STDERR or /dev/tty?
** 1. Writing to STDOUT will cause issue with command redirection.
**    Moreover, with ``, isatty(STDOUT_FILENO) return 0.
** 2. Writing to STDERR may works, but i'd rather keep it for error handling.
** 3. /dev/tty is the proper option and I use it with the -t option.
**    Problem: we can't use global and I don't want singleton in my main prog.
** 4. Writing to STDIN seems the most logical choice.
*/

/*
** GET_TERMINAL: get env data related to the terminal.
** Normally we would use TTY_NAME_MAX, but it is not always defined. So we
** use a big buffer of 4096 and call it a day.
** 1st if: if term->name already exist (after Ctrl->Z for instance) it will
** be used (assuming TERM has ben unset before relaunching the program).
*/

int					get_terminal(t_term *term)
{
	char	buff[4096];
	char	*t;
	int		fd;

	if (!(t = getenv("TERM")) && !term->name)
		return (ERR_TERMENV);
	term->name = t;
	if (term->flag & SELECT_T)
	{
		if ((fd = open("/dev/tty", O_RDWR)) == -1)
			return (ERR_BADFDTTY);
		term->fd = singleton_fd(fd);
		term->putchar = putchar_fd;
	}
	else
	{
		term->fd = STDIN_FILENO;
		term->putchar = putchar_in;
	}
	if (!isatty(term->fd))
		return (term->fd == STDIN_FILENO ? ERR_BADSTDIN : ERR_BADFDTTY);
	else if (tgetent(buff, term->name) < 1)
		return (ERR_NOTERMINFO);
	return (0);
}

/*
**  VMIN and VTIME: read() option.
** `ti': terminal mode adapted for full-screen display. Undo with `te'
** `vi': make the cursor invisible. Undo with `ve'
** We do not return error if `vi' fails because we view it as a bonus.
** We do not report if `vi' successes and `ve' fails. Could be a problem,
** but I may worry too much.
*/

int					load_new_terminal(t_term *term)
{
	if (tcgetattr(term->fd, &term->saved)
			|| tcgetattr(term->fd, &term->current))
		return (ERR_TCGET);
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	if (tcsetattr(term->fd, TCSANOW, &term->current))
		return (ERR_TCSET);
	if (tputs(tgetstr("ti", NULL), 1, term->putchar))
		return (ERR_TPUTS);
	if (term->flag & SELECT_CC)
		tputs(tgetstr("vi", NULL), 1, term->putchar);
	return (0);
}

int					load_saved_terminal(t_term *term)
{
	if (tcsetattr(term->fd, TCSANOW, &term->saved))
		return (ERR_TCSET);
	term->saved.c_lflag |= (ICANON | ECHO);
	tputs(tgetstr("ve", NULL), 1, term->putchar);
	if (tputs(tgetstr("te", NULL), 1, term->putchar))
		return (ERR_TPUTS);
	return (0);
}

/*
** This function was made to solve the following problematic sequence:
** kill -SIGTSTP PID
** kill -SIGCONT PID
** At this point, doing fg on the program will fail AND block SIGTSTP/Ctrl + Z
*/

int					reload_terminal(t_term *term)
{
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	if (tcsetattr(term->fd, TCSANOW, &term->current))
		return (ERR_RELOAD);
	if (tputs(tgetstr("ti", NULL), 1, term->putchar))
		return (ERR_RELOAD);
	if (term->flag & SELECT_CC)
		tputs(tgetstr("vi", NULL), 1, term->putchar);
	return (0);
}
