/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/06 12:35:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

inline static int	parse_flags(t_term *term, int *ac, char ***av)
{
	int		i;
	int		len;

	if (*ac < 2)
	{
		ft_dprintf(STDOUT_FILENO, "usage: ./ft_select [argument ...]\n");
		return (0);
	}
	(*av)++;
	(*ac)--;
	i = 0;
	term->colsize = 1;
	len = 0;
	while ((*av)[i])
	{
		if ((*av)[i][0])
			len = ft_strlen((*av)[i]);
		if (len > term->colsize)
			term->colsize = len;
		i++;
	}
	return (1);
}

long				read_keypress(t_term *term)
{
	long	key;
	long	mem;
	int		ret;
	int		i;

	//(void)term;
	i = 0;
	mem = 0;
	display_arg(term);
	/*while (term->av[i])
	{
		term->av[i + 1] ? ft_dprintf(STDIN_FILENO, "%s ", term->av[i]) : ft_dprintf(STDIN_FILENO, "%s\n", term->av[i]);
		i++;
	}*/
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

void	signal_test(void)
{
	signal(SIGWINCH, s_resize);
}

int					main(int ac, char **av)
{
	t_term term;
	long	mem;

	if (!parse_flags(&term, &ac, &av))
		return (1);
	if (!(term.name = get_terminal()))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		return (1);
	}
	term.ac = ac;
	term.av = av;
	g_term = &term;
	mem = 0;
	signal_test();
	mem = read_keypress(&term);
	load_saved_terminal(&term);
	//ft_printf("%zu %lc\n",term.colsize, mem);
	return (0);
}
