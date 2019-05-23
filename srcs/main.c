/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 14:22:32 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** SELECT_OPTION: set flag for shell option. Works like `ls` flags.
*/

static inline int	select_available_option(char *av, int *flag)
{
	int	i;

	while (*(++av))
	{
		if (!(i = ft_strchri(SELECT_OPTION, av[0])))
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
		if (*flag & SELECT_H)
			return (1);
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
** INIT_SELECT:
** print usage if wrong arguments or not enough argument
** print help if -h
** set a lot of variable, see structure in header.
** feed a circular list with av if possible
** get terminal
** set global variable for signal.
** set signal
*/

int					init_select(t_term *term, int ac, char **av)
{
	int	ret;

	if (ac < 2 || !(ret = select_option(ac, av, &(term->flag))))
		return (errmsg(ERR_USAGE));
	if (term->flag & SELECT_H)
		return (print_help());
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
	signal_setup(1);
	return (0);
}

/*
** FINISH_SELECT:
** Restore terminal session. Continue even in case of error.
** Print selected argument if last keystroke was '\n'
** delete circular list.
** check if /dev/tty has been used and close the associated fd.
*/

void				finish_select(t_term *term, long key)
{
	int	ret;

	ret = 0;
	term->dcursor = term->dlist;
	load_saved_terminal(term);
	if (key == '\n')
	{
		while (1)
		{
			if (term->dcursor->flag & FT_SELECTED)
			{
				if (!ret && (ret = 1))
					ft_dprintf(STDOUT_FILENO, "%s", term->dcursor->txt);
				else
					ft_dprintf(STDOUT_FILENO, " %s", term->dcursor->txt);
			}
			term->dcursor = term->dcursor->next;
			if (term->dcursor->flag & FT_FIRST)
				break ;
		}
	}
	ft_dlistdel(&(term->dlist));
	if (term->flag & SELECT_T)
		close(term->fd);
}

/*
** MAIN:
** load_new_terminal might fail: in that case we attempt to restore the session
*/

int					main(int ac, char **av)
{
	t_term	term;
	int		ret;

	if ((ret = init_select(&term, ac, av)))
		return (ret == ERR_USAGE ? 0 : ret);
	if ((ret = load_new_terminal(&term)))
	{
		finish_select(&term, 0);
		return (ret);
	}
	ret = keypress(&term);
	finish_select(&term, ret);
	return (0);
}
