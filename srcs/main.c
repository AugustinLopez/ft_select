/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/15 15:19:30 by aulopez          ###   ########.fr       */
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
		{
			if (!(av[i + 1]))
				return (0);
			return (i + 1);
		}
		if (select_available_option(av[i], flag))
			return (0);
	}
	return (i);
}

/*
** The global variable is a pointer to our structure. Used for signal handling.
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
	if ((ret = get_terminal(term)))
		return (ret);
	g_term = term;
	return (0);
}

void				finish_select(t_term *term, int key)
{
	int	ret;

	term->dcursor = term->dlist;
	ret = 0;
	while (key)
	{
		if (term->dcursor->flag & FT_SELECTED)
		{
			if (!ret)
			{	
				ft_dprintf(STDOUT_FILENO, "%s", term->dcursor->txt);
				ret = 1;
			}
			else
				ft_dprintf(STDOUT_FILENO, " %s", term->dcursor->txt);
		}
		term->dcursor = term->dcursor->next;
		if (term->dcursor->flag & FT_FIRST)
			break ;
	}
	ft_dlistdel(&(term->dlist));
}

int					main(int ac, char **av)
{
	t_term	term;
	long	key;
	int		ret;
	
	if ((ret = init_select(&term, ac, av)))
		return (ret == ERR_USAGE ? 0 : ret);
	if ((ret = load_new_terminal(&term)))
	{
		load_saved_terminal(&term);
		ft_dlistdel(&(term.dlist));
		return (ret);
	}
	signal_setup();
	key = read_keypress(&term);
	load_saved_terminal(&term);
	finish_select(&term, key);
	return (0);
}
