/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_option.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 18:43:44 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline int	select_available_option(char *av, int *flag)
{
	int	i;

	while (*(++av))
	{
		if (!(i = ft_strchri("mpcG", av[0])))
			return (-1);
		*flag |= (1 << (i - 1));
	}
	if (*flag & SELECT_C)
		*flag &= ~SELECT_M;
	return (0);
}


int					select_option(int ac, char **av, int *flag)
{
	int	i;

	*flag = 0;
	i = 0;
	while (++i < ac && av[i][0] == '-' && av[i][1])
	{
		if (av[i][1] == '-' && !av[i][2])
			return (i + 1);
		if (select_available_option(av[i], flag))
			return (0);
	}
	return (i);
}

/*
** The global variable is a pointer to our structure. Used for signal handling.
*/

/*
** return different error if usage ? should return 0 in the end
*/
int					init_select(t_term *term, int ac, char **av)
{
	int	ret;

	if (ac < 2 || !(ret = select_option(ac, av, &(term->flag))))
		return (errmsg(ERR_USAGE));
	term->name = 0;
	term->selected = 0;
	term->ac = ac - ret;
	term->av = av + ret;
	term->maxlen = 1;
	term->dlist = 0;
	term->dcursor = 0;
	term->up = (term->flag & SELECT_M) ? arrow_up_mat : arrow_up_cir;
	term->down = (term->flag & SELECT_M) ? arrow_down_mat : arrow_down_cir;
	term->left = (term->flag & SELECT_M) ? arrow_left_mat : arrow_left_cir;
	term->right = (term->flag & SELECT_M) ? arrow_right_mat : arrow_right_cir;
	if ((ret = feed_dlist(term, av + ret - 1)))
		return (errmsg(ret));
	g_term = term;
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


int				errmsg(int error)
{
	if (error == ERR_USAGE)
		ft_dprintf(STDOUT_FILENO,
			"usage: ./ft_select [-mpc] [--] [arg1 arg2 ...]\n");
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
			"ft_select: STDIN not linked to terminal. Try the -t option.\n"); //bonus
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

	if (!(t = getenv("TERM")))
	{
		if (!term->name)
			return (errmsg(ERR_TERMENV));
	}
	else
		term->name = t;
	term->fd = STDIN_FILENO;
	if (!isatty(term->fd))
		return (term->fd == STDIN_FILENO ?
			errmsg(ERR_BADSTDIN) : errmsg(ERR_BADFDTTY));
	else if (tgetent(buff, term->name) < 1)
		return (errmsg(ERR_NOTERMINFO));
	else
		return (0);
	return (-1);
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
		return (errmsg(ERR_TCGET));
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	ret += tcsetattr(term->fd, TCSANOW, &term->current);
	if (ret)
		return (errmsg(ERR_TCSET));
	ret += tputs(tgetstr("ti", NULL), 1, putchar_in);
	ret += tputs(tgetstr("vi", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ho", NULL), 1, putchar_in);
	if (ret)
		return (errmsg(ERR_TPUTS));
	return (0);
}

int		load_saved_terminal(t_term *term)
{
	int	ret;

	if ((ret = tcsetattr(term->fd, TCSANOW, &term->saved)))
		return (errmsg(ERR_TCSET));
	term->saved.c_lflag |= (ICANON | ECHO);
	ret = tputs(tgetstr("te", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ve", NULL), 1, putchar_in);
	if (ret)
		return (errmsg(ERR_TPUTS));
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
