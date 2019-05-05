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
		ft_dprintf(STDOUT_FILENO, "usage: ./ft_select [argument ...]\n");
		return (0);
	}
	(*av)++;
	(*ac)--;
	return (1);
}

void				read_keypress(t_term *term, char **av)
{
	long	key;
	int		ret;
	int		i;

	(void)term;
	i = 0;
	while (av[i])
	{
		av[i + 1] ? ft_dprintf(STDIN_FILENO, "%s ", av[i]) : ft_dprintf(STDIN_FILENO, "%s\n", av[i]);
		i++;
	}
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
			return ;
		else
			ft_dprintf(STDIN_FILENO, "%lc\n",key);
	}
}

int					main(int ac, char **av)
{
	t_term term;

	if (!parse_flags(&ac, &av))
		return (1);
	if (!(term.name = get_terminal()))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		return (1);
	}
	read_keypress(&term, av);
	load_saved_terminal(&term);
	return (0);
}
