/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/02 13:49:52 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

inline static int	parse_flags(int *ac, char ***av)
{
	if (*ac < 2)
	{
		ft_dprintf(STDERR_FILENO, "usage: ./ft_select [argument ...]\n");
		return (0);
	}
	(*av)++;
	(*ac)--;
	return (1);
}

int					putschar(int c)
{
	return (write(STDOUT_FILENO, &c, 1));
}

inline static char	*get_terminal(void)
{
	char	*s;
	int		ret;
	char	buff[1024];

	if (!(s = getenv("TERM")))
		ft_dprintf(STDERR_FILENO, "ft_select: Could not get terminal name.\n");
	else if (!isatty(STDOUT_FILENO))
		ft_dprintf(STDERR_FILENO, "ft_select: unexpected fd for stdout.\n");
	else if ((ret = tgetent(buff, s)) < 1)
		ret < 0 ?
			ft_dprintf(STDERR_FILENO, "ft_select: not terminfo data found.\n") :
			ft_dprintf(STDERR_FILENO, "ft_select: could not find %s.\n", s);
	else
		return (s);
	return (NULL);
}

inline static void	load_new_terminal(t_term *term)
{
	tcgetattr(STDOUT_FILENO, &term->saved);
	tcgetattr(STDOUT_FILENO, &term->current);
	term->current.c_lflag &= ~(ICANON);
	term->current.c_cc[VMIN] = 1;
	term->current.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &term->current);
	tputs(tgetstr("ti", NULL), 1, putschar);
	tputs(tgetstr("vi", NULL), 1, putschar);
}

inline static void	load_saved_terminal(t_term *term)
{
	tcsetattr(STDOUT_FILENO, TCSANOW, &term->saved);
	tputs(tgetstr("te", NULL), 1, putschar);
	tputs(tgetstr("ve", NULL), 1, putschar);
}

// VMIN and VTIME: http://www.unixwiz.net/techtips/termios-vmin-vtime.html
//`ti' : https://www.gnu.org/software/termutils/manual/termcap-1.3/html_mono/termcap.html
//use to move the cursor around the screen. exit with `te'
//`vi' : make the cursor invisible
int					main(int ac, char **av)
{
	t_term term;
	char d[10];
	int		i;

	i = 0;
	if (!parse_flags(&ac, &av))
		return (1);
	if (!(term.name = get_terminal()))
		return (1);
	load_new_terminal(&term);
	while (av[i])
	{
		av[i + 1] ? ft_printf("%s ", av[i]) : ft_printf("%s\n", av[i]);
		i++;
	}
	ft_putendl(term.name);
	read(0, d, 10);
	ft_putstr(d);
	read(0, d, 10);
	load_saved_terminal(&term);
	return (0);
}
