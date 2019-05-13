/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_option.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/13 14:01:35 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline int	feed_dlist(t_term *term, char **av)
{
	int	i;
	int	len;

	len = 0;
	i = 0;
	while (av[++i])
	{
		if (!av[i][0])
		{
			(term->ac)--;
			continue ;
		}
		if (i == 1)
		{
			term->dcursor = ft_dlistnew(av[i], FT_CURSOR | FT_FIRST, 0);
			term->dlist = term->dcursor;
		}
		else
			term->dlist = ft_dlistnew(av[i], 0, term->dlist);
		if (!term->dlist)
			return (-1);
		len = av[i][0] ? ft_strlen(av[i]) : 0; //do i need to condition ?
		term->maxlen = len > term->maxlen ? len : term->maxlen;
	}
	term->dlist->next = term->dcursor;
	term->dcursor->prev = term->dlist;
	term->dlist = term->dcursor;
	return (0);
}

int					init_select(t_term *term, int ac, char **av)
{
	if (ac < 2)
	{
		ft_dprintf(STDOUT_FILENO, "usage: ./ft_select [argument ...]\n");
		return (-1);
	}
	term->ac = ac - 1;
	term->av = av + 1;
	term->maxlen = 1;
	g_term = term;
	if (feed_dlist(term, av))
	{
		ft_dprintf(STDERR_FILENO, "ft_select: not enough memory to start.\n");
		return (-1);
	}
	return (0);
}

/*
 * UPDATE: Not using /dev/tty: I would need to use global outside of signal.
 * I will NOT use singleton in C: it is just an obfuscated global.
** Why using STDIN instead of STDOUT or STDERR?
** 1. Writing to STDOUT will cause issue with command redirection.
**    Moreover, with ``, isatty(STDOUT_FILENO) return 0.
** 2. Writing to STDERR may works, but it's a dirty solution.
** 3. Writing to STDIN seems the most logical choice.
** Normally we would use TTY_NAME_MAX, but it is not always defined and
** using a conditional define is a pain with the norm.
** we use t and s so we can reload if suspended + TERM unset
*/

char				*get_terminal(t_term *term)
{
	int		ret;
	char	buff[512];
	char	*t;

	if (!(t = getenv("TERM")))
	{
		if (!term->name)
			ft_dprintf(STDERR_FILENO, "ft_select: could not get TERM.\n");
		else
			t = term->name;
	}
	term->name = t;
	term->fd = STDIN_FILENO;
	if (!isatty(term->fd))
		ft_dprintf(STDERR_FILENO, "ft_select: STDIN not linked to terminal.\n");
	else if ((ret = tgetent(buff, term->name)) < 1)
		ret < 0 ?
			ft_dprintf(STDERR_FILENO, "ft_select: no terminfo data found.\n") :
			ft_dprintf(STDERR_FILENO, "ft_select: could not find %s.\n", term->name);
	else
		return (term->name);
	return (NULL);
}

/*
**  VMIN and VTIME: read() option.
** See: http://www.unixwiz.net/techtips/termios-vmin-vtime.html
**
** `ti': terminal mode to move the cursor non-sequentially. Undo with `te'
** `vi': make the cursor invisible. Undo with `ve'
** `cl': clear the entire screen.
** `ho': move the cursor in the upper left corner of the screen.
** See: https://www.gnu.org/software/termutils/
**       manual/termcap-1.3/html_mono/termcap.html
*/

int		load_new_terminal(t_term *term)
{
	int	ret;

	ret = tcgetattr(term->fd, &term->saved);
	ret += tcgetattr(term->fd, &term->current);
	if (ret)
	{
		ft_dprintf(STDERR_FILENO, "ft_select: error when getting terminal.\n");
		return (ret);
	}
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	ret += tcsetattr(term->fd, TCSANOW, &term->current);
	if (ret)
	{
		ft_dprintf(STDERR_FILENO, "ft_select: error when setting terminal.\n");
		return (ret);
	}
	ret += tputs(tgetstr("ti", NULL), 1, putchar_in);
	ret += tputs(tgetstr("vi", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ho", NULL), 1, putchar_in);
	if (ret)
		ft_dprintf(STDERR_FILENO, "ft_select: error when loading screen.\n");
	return (ret);
}

int		load_saved_terminal(t_term *term)
{
	int	ret;

	if ((ret = tcsetattr(term->fd, TCSANOW, &term->saved)))
		ft_dprintf(STDERR_FILENO,
			"ft_select: error when resetting terminal.\n");
	ret += tputs(tgetstr("te", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ve", NULL), 1, putchar_in);
	if (ret)
		ft_dprintf(STDERR_FILENO, "ft_select: error when resetting screen.\n");
	return (ret);
}

/*
** If I used /dev/tty, i would need a global to do it correctly
** (IE: even if i could do without global it would be hard, ugly and hacky)
*/

int		putchar_in(int c)
{
	return (write(STDIN_FILENO, &c, 1));
}
