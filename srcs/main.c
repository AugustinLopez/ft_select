/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/06 17:29:45 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

long				read_keypress(t_term *term)
{
	long	key;
	long	mem;
	int		ret;
	int		i;

	i = 0;
	mem = 0;
	display_arg(term);
	while (1)
	{
		key = 0;
		ret = read(STDIN_FILENO, &key, sizeof(key));
		ret = 0;
		(void)ret;
		if (key == KEY_LEFT)
			ft_dprintf(STDIN_FILENO, "<\n");
		else if (key == KEY_UP)
			ft_dprintf(STDIN_FILENO, "/\\\n");
		else if (key == KEY_DOWN)
			ft_dprintf(STDIN_FILENO, "\\/\n");
		else if (key == KEY_RIGHT)
			ft_dprintf(STDIN_FILENO, ">\n");
		else if (key == KEY_ESCAPE)
		{
			return (mem);
		}
		else
		{
			mem = key;
			ft_dprintf(STDIN_FILENO, "%lc\n",key);
		}
	}
	return (0);
}

void	s_resize(int signo)
{
	if (signo == SIGWINCH)
		display_arg(g_term);
}

void	s_ctrl_z(int signo)
{
	if (signo == SIGTSTP)
	{
		load_saved_terminal(g_term);
		signal(SIGTSTP, SIG_DFL);
		ioctl(STDIN_FILENO, TIOCSTI, "\x1A");
	}
}

void	s_fg(int signo)
{
	if (signo == SIGCONT)
	{
		if (!(g_term->name = get_terminal(g_term->name))
			|| load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			load_saved_terminal(g_term);
			exit(1);
		}
		signal_test();
		display_arg(g_term);
	}
}

void	signal_test(void)
{
	signal(SIGWINCH, s_resize);
	signal(SIGTSTP, s_ctrl_z);
	signal(SIGCONT, s_fg);
}

int					main(int ac, char **av)
{
	t_term term;
	long	mem;

	term.name = 0;
	if (!init_select(&term, &ac, &av) || !(term.name = get_terminal(term.name)))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		return (1);
	}
	mem = 0;
	signal_test();
	mem = read_keypress(&term);
	load_saved_terminal(&term);
	ft_printf("%zu %lc\n",term.maxlen, mem);
	return (0);
}
