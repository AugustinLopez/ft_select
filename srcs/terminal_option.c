/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_option.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/05 14:20:00 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/05 14:20:00 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** Why using STDIN instead of STDOUT or STDERR?
** 1. Writing to STDOUT will cause issue with command redirection.
**    Moreover, with ``, isatty(STDOUT_FILENO) return 0.
** 2. Writing to STDERR may works, but it's a dirty solution.
** 3. Writing to STDIN seems the most logical choice.
*/

char	*get_terminal(void)
{
	char	*s;
	int		ret;
	char	buff[TTY_NAME_MAX * 2];

	if (!(s = getenv("TERM")))
		ft_dprintf(STDERR_FILENO, "ft_select: Could not get terminal name.\n");
	else if (!isatty(STDIN_FILENO))
		ft_dprintf(STDERR_FILENO, "ft_select: STDIN not linked to terminal.\n");
	else if ((ret = tgetent(buff, s)) < 1)
		ret < 0 ?
			ft_dprintf(STDERR_FILENO, "ft_select: no terminfo data found.\n") :
			ft_dprintf(STDERR_FILENO, "ft_select: could not find %s.\n", s);
	else
		return (s);
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

	ret = tcgetattr(STDIN_FILENO, &term->saved);
	ret += tcgetattr(STDIN_FILENO, &term->current);
	term->current.c_lflag &= ~(ICANON | ECHO);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	ret += tcsetattr(STDIN_FILENO, TCSANOW, &term->current);
	if (ret)
	{
		ft_dprintf(STDERR_FILENO, "ft_select: error when setting terminal.\n");
		return (ret);
	}
	ret += tputs(tgetstr("ti", NULL), 1, putchar_in);
	ret += tputs(tgetstr("vi", NULL), 1, putchar_in);
	ret += tputs(tgetstr("cl", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ho", NULL), 1, putchar_in);
	if (ret)
		ft_dprintf(STDERR_FILENO, "ft_select: error when loading screen.\n");
	return (ret);
}

int		load_saved_terminal(t_term *term)
{
	int	ret;

	if ((ret = tcsetattr(STDIN_FILENO, TCSANOW, &term->saved)))
		ft_dprintf(STDERR_FILENO,
			"ft_select: error when resetting terminal.\n");
	ret += tputs(tgetstr("te", NULL), 1, putchar_in);
	ret += tputs(tgetstr("ve", NULL), 1, putchar_in);
	if (ret)
		ft_dprintf(STDERR_FILENO, "ft_select: error when resetting screen.\n");
	return (ret);
}

int		putchar_in(int c)
{
	return (write(STDIN_FILENO, &c, 1));
}
